# 资源打包统一前端(GUI + 命令行)
# 位于 scui/tools/scui_pack/; 引用相对父路径 scui/tools 取用其它打包工具
# 支持: image / font / lang / cwf 四类
# 无参       -> 启动图形界面(顶部固定信息 + 四个子界面)
# 带参型     -> 命令行模式: python scui_pack_tools.py <type> [--src P] [--dst P] [--proj N]
# 说明: GUI 运行时把控制台 stdout/stderr 打印重定向输出到子界面 log 区;
#       运行产生的临时/缓存一律落在 ui 环境, 不在 tools 内引入 cache 与子文件
import os
import re
import json
import sys
import importlib
import shutil
import queue
import threading

#============================================================
# 路径定位(基于 ui 路径推导, 兼容 dev 与 PyInstaller 单文件 exe)
#   ui 目录(scui_ui_res)的父级为 app_thread_scui
#   -> scui/tools(打包脚本)  scui_plugs(cwf 脚本)
#   注意: 不能用 __file__ 推导(exe 运行时指向临时解包目录)
#============================================================
def _dirs(ui):
    ui   = os.path.normpath(ui)
    app  = os.path.dirname(ui)                      # app_thread_scui
    scui = os.path.join(app, 'scui')
    tools= os.path.join(scui, 'tools')
    plugs= os.path.join(app, 'scui_plugs')
    return ui, app, scui, tools, plugs

def _ui_root(ui):
    return os.path.normpath(ui or os.getcwd())

#============================================================
# 任务定义
#============================================================
_TASK = {
    'widget': {'module': 'scui_widget_parser', 'entry': 'scui_widget_parser',
               'as_main': ['scui_widget_parser.py'], 'dir': 'tools'},
    'image' : {'module': 'scui_image_parser', 'entry': 'scui_image_parser',
               'as_main': ['scui_image_parser.py'], 'dir': 'tools'},
    'font'  : {'module': 'scui_font_package', 'entry': 'scui_font_package',
               'as_main': ['scui_font_package.py'], 'dir': 'tools'},
    'lang'  : {'module': 'scui_lang_parser', 'entry': 'encode_scui_lang_parser',
               'as_main': ['scui_lang_parser.py'], 'dir': 'tools'},
    'cwf'   : {'module': 'scui_cwf_json_parser', 'entry': 'scui_cwf_json_parser',
               'as_main': ['scui_cwf_json_parser.py'], 'dir': 'plugs'},
}

# ui 默认子路径(src, dst)
_DEFAULT = {
    'widget': ('scene_src', 'scene_out'),
    'image' : ('image_src', 'image_out'),
    'font'  : ('font_bin',  'font_out'),
    'lang'  : ('lang_src',  'lang_out'),
    'cwf'   : ('cwf_json',  'cwf_json/bin'),
}

_IMG_EXT = ('.bmp', '.jpg', '.jpeg', '.png', '.gif', '.lottie.json', '.mp4')

#============================================================
# 脚本模块调用(注入 argv, 直接 import 复用主函数)
#============================================================
def _run_module(task, argv, ui):
    ui, app, scui, tools, plugs = _dirs(ui)
    script_dir = tools if task['dir'] == 'tools' else plugs
    if script_dir not in sys.path:
        sys.path.insert(0, script_dir)
    argv_old = sys.argv
    sys.argv = [task['as_main'][0]] + argv
    try:
        mod   = importlib.import_module(task['module'])
        if task['module'] == 'scui_widget_parser':
            mod.SCUI_WIDGET_TOOLS = tools     # 注入真实 tools 目录(打包环境下 __file__ 指向临时解包目录)
        entry = getattr(mod, task['entry'])
        entry()
        return 0
    except ImportError as e:
        print('[pack] 缺模块: %s' % e)
        print('[pack] 请确保依赖已安装(image需 pillow/lz4; image.7z解压需 py7zr)')
        return 1
    except Exception as e:
        print('[pack] 执行异常: %r' % e)
        return 1
    finally:
        sys.argv = argv_old
        shutil.rmtree(os.path.join(tools, '__pycache__'), ignore_errors=True)
        shutil.rmtree(os.path.join(plugs, '__pycache__'), ignore_errors=True)

def _collect_logs(dst, name):
    base = os.path.splitext(_TASK[name]['as_main'][0])[0]  # 如 scui_image_parser
    for tag in ('.out', '.err'):
        p = os.path.join(dst, base + tag)
        if not os.path.exists(p):
            continue
        try:
            with open(p, 'r', encoding='utf-8', errors='replace') as f:
                print('==== %s ====' % os.path.basename(p))
                print(f.read().rstrip())
        except OSError:
            pass

def _image_src_unpack(src, tag):
    if os.path.isdir(src):
        return src, None
    if src.lower().endswith('.7z'):
        tmp = os.path.join(os.path.dirname(src), tag + '.src_tmp')
        try:
            import py7zr
            shutil.rmtree(tmp, ignore_errors=True)
            with py7zr.SevenZipFile(src, 'r') as z:
                z.extractall(tmp)
            return tmp, tmp
        except ImportError:
            print('[pack] 解压 image.7z 需要 py7zr: pip3 install py7zr')
            return None, None
    return None, None

def _do_task(name, ui, src, dst, proj):
    task   = _TASK[name]
    src    = os.path.normpath(src)
    dst    = os.path.normpath(dst)

    print()
    print('[pack] type : %s' % name)
    print('[pack] src  : %s' % src)
    print('[pack] dst  : %s' % dst)

    tmp = None
    if name == 'image':
        src, tmp = _image_src_unpack(src, 'image')
        if src is None:
            return 1
        argv = [src, dst, proj or 'scui']
    elif name == 'widget':
        # widget: src dst default_json(scene_out 下的 maker 默认配置)
        argv = [src, dst, os.path.join(dst, 'scui_ui_maker.json')]
    else:
        argv = [src, dst]

    if not os.path.exists(src):
        print('[pack] src not exist: %s' % src)
        return 1
    if not os.path.isdir(dst):
        os.makedirs(dst, exist_ok=True)

    print('[pack] argv : %s' % argv)
    ret = _run_module(task, argv, ui)
    _collect_logs(dst, name)
    if tmp:
        shutil.rmtree(tmp, ignore_errors=True)
    print()
    print('[pack] %s 执行完成: %s' % (name, 'OK' if ret == 0 else 'FAIL'))
    return ret

#============================================================
# 图像信息探测(对齐 scui_image_t 结构体字段)
#============================================================
def _img_type_of(path):
    lo = path.lower()
    if lo.endswith('.gif'):           return 'scui_image_type_gif'
    if lo.endswith('.lottie.json'):   return 'scui_image_type_lottie'
    if lo.endswith('.mp4'):           return 'scui_image_type_mp4'
    if lo.endswith(('.jpg', '.jpeg')):return 'scui_image_type_jpg'
    if lo.endswith('.png'):           return 'scui_image_type_png'
    return 'scui_image_type_bmp'

def _image_probe(path):
    info = {'path': path, 'name': os.path.basename(path)}
    info['type'] = _img_type_of(path)
    # size
    w = h = 0
    try:
        import PIL.Image as _img
        im = _img.open(path); w, h = im.size; im.close()
    except Exception:
        pass
    info['pixel.width']  = w
    info['pixel.height'] = h
    info['pixel.size_bin'] = os.path.getsize(path)
    # format & 每像素内存字节
    fmt = 'scui_pixel_cf_none'; pb = 0
    if info['type'] == 'scui_image_type_bmp': fmt, pb = 'scui_pixel_cf_bmp565', 2
    elif info['type'] == 'scui_image_type_jpg': fmt, pb = 'scui_pixel_cf_bmp565', 2
    elif info['type'] == 'scui_image_type_png': fmt, pb = 'scui_pixel_cf_bmp8565', 3
    info['pixel.format'] = fmt
    info['pixel.size_mem'] = w * h * pb
    info['com_pct'] = (float(info['pixel.size_bin']) / info['pixel.size_mem']) if info['pixel.size_mem'] else 1.0
    return info

#============================================================
# GUI
#============================================================
def _launch_gui(ui):
    try:
        import tkinter as tk
    except Exception as e:
        print('[pack] 无图形环境, 请使用命令行模式: %s' % e)
        return 1
    app = PackApp(ui)
    app.root.mainloop()
    return 0

class PackApp(object):
    def __init__(self, ui):
        import tkinter as tk
        from tkinter import ttk
        self.ui  = ui
        self.ui, self.app, self.scui, self.tools, self.plugs = _dirs(ui)
        self._imgs = []          # 保持 PhotoImage 引用
        self.logs = {}           # name -> ScrolledText
        self._pv  = {}           # (name,side) -> StringVar
        self._current_name = 'image'
        self.current_log = None
        self._logq = None

        self.in_abs = {}; self.out_abs = {}
        for k, (si, so) in _DEFAULT.items():
            self.in_abs[k]  = os.path.join(ui, si)
            self.out_abs[k] = os.path.join(ui, so)

        self.root = tk.Tk()
        self.root.title('scui 打包工具')
        self.root.geometry('980x720')
        self.root.minsize(860, 620)

        menubar = tk.Menu(self.root)
        m_set = tk.Menu(menubar, tearoff=0)
        m_set.add_command(label='路径…', command=self._open_paths)
        menubar.add_cascade(label='设置', menu=m_set)
        m_log = tk.Menu(menubar, tearoff=0)
        m_log.add_command(label='打开 out/err 浏览…', command=self._open_logs)
        menubar.add_cascade(label='日志', menu=m_log)
        m_help = tk.Menu(menubar, tearoff=0)
        m_help.add_command(label='关于', command=self._about)
        menubar.add_cascade(label='帮助', menu=m_help)
        self.root.config(menu=menubar)

        self._build_header()
        self._build_notebook()

        self.root.protocol('WM_DELETE_WINDOW', self.root.destroy)
        self.root.after(120, self._poll)

    #--------------- 顶部信息条(只读, 无按钮) ---------------
    def _build_header(self):
        from tkinter import ttk
        bar = ttk.Frame(self.root, padding=(12, 6))
        bar.pack(fill='x')
        ttk.Label(bar, text='scui_ui: %s' % self.ui, foreground='#777').pack(side='left')
        self.path_status = ttk.Label(bar, text='', foreground='#888')
        self.path_status.pack(side='left', padx=(14, 0))

    #--------------- 路径设置子弹窗(显示所有路径, 前两项只读, 其余可改) ---------------
    def _open_paths(self):
        import tkinter as tk
        from tkinter import ttk, filedialog
        top = tk.Toplevel(self.root)
        top.title('路径设置')
        top.geometry('620x600')
        top.transient(self.root)
        f = ttk.Frame(top, padding=(12, 10)); f.pack(fill='both', expand=True)
        ttk.Label(f, text='scui 路径: %s' % self.scui, font=('Consolas', 9)).pack(anchor='w')
        ttk.Label(f, text='scui_ui 路径: %s' % self.ui, font=('Consolas', 9)).pack(anchor='w', pady=(2, 8))

        grid = ttk.Frame(f); grid.pack(fill='x')
        varman = {}
        def _browse(entryvar):
            abs_p = filedialog.askdirectory(title='选择路径', initialdir=entryvar.get() or self.ui)
            if abs_p:
                entryvar.set(self._rel(abs_p))     # 显示为相对
        r = 0
        for name in ('widget', 'image', 'font', 'lang', 'cwf'):
            for side, lab in (('in', '输入'), ('out', '输出')):
                cur = self.in_abs[name] if side == 'in' else self.out_abs[name]
                ttk.Label(grid, text='%s %s:' % (name, lab), width=14, anchor='e')\
                    .grid(row=r, column=0, padx=(0, 8), pady=3)
                var = tk.StringVar(value=self._rel(cur))   # 子类型显示相对路径
                e = ttk.Entry(grid, textvariable=var, font=('Consolas', 9))
                e.grid(row=r, column=1, sticky='ew', pady=3)
                ttk.Button(grid, text='浏览…', width=7, command=lambda v=var: _browse(v))\
                    .grid(row=r, column=2, padx=(8, 0))
                varman[(side, name)] = var
                r += 1
        grid.columnconfigure(1, weight=1)

        note = ttk.Label(f, text='以下为相对 scui_ui 路径, 可修改；scui / scui_ui 为绝对路径且固定。修改即时生效。',
                         foreground='#888').pack(anchor='w', pady=(8, 4))
        ttk.Button(f, text='关闭', command=top.destroy).pack(side='right')
        for (side, name), var in varman.items():
            var.trace_add('write', lambda *a, s=side, n=name, v=var: self._apply_path(s, n, v))
        top.protocol('WM_DELETE_WINDOW', top.destroy)

    def _apply_path(self, side, name, var):
        val = var.get().strip()
        if not val:
            return
        # 相对路径 -> 转绝对(相对 scui_ui)
        if not os.path.isabs(val):
            val = os.path.normpath(os.path.join(self.ui, val))
        if side == 'in':
            self.in_abs[name] = val
        else:
            self.out_abs[name] = val
        if hasattr(self, 'path_status'):
            self.path_status.config(text='%s %s: %s' % (name, '输入' if side == 'in' else '输出', self._rel(val)))
        pv = self._pv.get((name, side))
        if pv is not None:
            pv.set('%s: %s' % ('输入' if side == 'in' else '输出', self._rel(val)))

    def _rel(self, p):
        try:
            return os.path.relpath(p, self.ui).replace('\\', '/')
        except Exception:
            return p

    #--------------- 四个子界面(tab) ---------------
    def _build_notebook(self):
        import tkinter as tk
        from tkinter import ttk
        self.nb = ttk.Notebook(self.root)
        self.nb.pack(fill='both', expand=True, padx=12, pady=(0, 12))
        self.tabs = {}
        self._build_pack_tab('widget')   # widget: 简单打包(第一条)
        self._build_image_tab()          # image: 完整
        self._build_font_tab()           # font: 完整
        self._build_pack_tab('lang')     # lang: 简单打包
        self._placeholder_tab('cwf')     # cwf: 建设中(占位)
        self.nb.bind('<<NotebookTabChanged>>', self._on_tab)
        self.current_log = self.logs.get('image')

    #--------------- 简单打包子界面(widget/lang 共用: 按钮 + 路径 + log) ---------------
    def _build_pack_tab(self, name):
        import tkinter as tk
        from tkinter import ttk
        f = ttk.Frame(self.nb, padding=8)
        self.tabs[name] = f

        # 顶部: 打包按钮 + 输入/输出相对路径
        bar = ttk.Frame(f); bar.pack(fill='x', pady=(2, 4))
        ttk.Button(bar, text='执行 %s 打包' % name, command=lambda: self._run(name)).pack(side='left')
        self._pv[(name, 'in')]  = tk.StringVar(value='输入: %s' % self._rel(self.in_abs[name]))
        self._pv[(name, 'out')] = tk.StringVar(value='输出: %s' % self._rel(self.out_abs[name]))
        ttk.Label(f, textvariable=self._pv[(name, 'in')],  foreground='#888').pack(anchor='w', padx=(2, 0))
        ttk.Label(f, textvariable=self._pv[(name, 'out')], foreground='#888').pack(anchor='w', padx=(2, 0))

        # LOG
        ln = ttk.LabelFrame(f, text=' LOG 输出 ', padding=(6, 4))
        ln.pack(fill='both', expand=True, pady=(6, 0))
        self.logs[name] = self._make_log(ln)

        self.nb.add(f, text=name + '  ')

    def _placeholder_tab(self, name):
        from tkinter import ttk
        f = ttk.Frame(self.nb, padding=12)
        f.pack(fill='both', expand=True)
        ttk.Label(f, text='子界面[%s] 建设中…' % name).pack(anchor='w', pady=(4, 0))
        ln = ttk.LabelFrame(f, text=' LOG 输出 ', padding=(6, 4))
        ln.pack(fill='both', expand=True, pady=(12, 0))
        self.logs[name] = self._make_log(ln)
        self.nb.add(f, text=name + '  ')

    def _build_font_tab(self):
        import tkinter as tk
        from tkinter import ttk
        f = ttk.Frame(self.nb, padding=8)
        self.tabs['font'] = f
        self.font_status = tk.StringVar(value='未加载')

        # 顶部工具条: json 三键 + 执行打包(同一列)
        bar = ttk.Frame(f); bar.pack(fill='x', pady=(2, 4))
        for t, c in (('加载 json', lambda: self._font_load(False)),
                     ('预览 json', self._font_preview),
                     ('保存 json', self._font_save)):
            ttk.Button(bar, text=t, command=c).pack(side='left')
        ttk.Button(bar, text='执行 font 打包', command=lambda: self._run('font')).pack(side='right')
        ttk.Label(bar, textvariable=self.font_status, foreground='#777').pack(side='left', padx=(12, 0))
        ttk.Label(f, text='font.bin 来源于 lv_font_conv 工具，见 font_src/lv_font_conv txt & py',
                  foreground='#888').pack(anchor='w', padx=(2, 0))

        # 主体: 垂直可调(上部 树/编辑 | 下部 LOG, 保证 LOG 空间)
        vp = ttk.Panedwindow(f, orient='vertical'); vp.pack(fill='both', expand=True, pady=(6, 0))
        mid = ttk.Panedwindow(vp, orient='horizontal'); vp.add(mid, weight=6)
        lf = ttk.LabelFrame(mid, text=' 字库(json) ', padding=(4, 4)); mid.add(lf, weight=1)
        self.ftree = ttk.Treeview(lf, columns=('size',), show='tree headings', selectmode='browse')
        self.ftree.heading('#0', text='字库条目'); self.ftree.heading('size', text='字号')
        self.ftree.column('size', width=60, anchor='e', stretch=False)
        fvs = ttk.Scrollbar(lf, orient='vertical', command=self.ftree.yview)
        self.ftree.configure(yscrollcommand=fvs.set)
        self.ftree.pack(side='left', fill='both', expand=True); fvs.pack(side='right', fill='y')
        self.ftree.bind('<<TreeviewSelect>>', self._on_font_sel)

        # 右: 六个操作按钮 与 编辑表单 水平并排
        rf = ttk.Frame(mid); mid.add(rf, weight=2)
        self.font_json = None
        self.font_sel = None       # (lang_idx, item_idx)
        ops = ttk.LabelFrame(rf, text='语言 / 字库'); ops.pack(side='left', fill='y', padx=(0, 8))
        for t, c in (('添加语言', self._font_addlang), ('删除语言', self._font_dellang),
                     ('修改语言', self._font_modlang), ('添加字库', self._font_add),
                     ('删除字库', self._font_del), ('修改字库', self._font_modlib)):
            ttk.Button(ops, text=t, command=c).pack(fill='x', pady=2)
        ef = ttk.LabelFrame(rf, text=' 编辑(json 特性) '); ef.pack(side='left', fill='both', expand=True)
        self._build_font_editor(ef)

        # 下部 LOG
        ln = ttk.LabelFrame(vp, text=' LOG 输出 ', padding=(6, 4)); vp.add(ln, weight=4)
        self.logs['font'] = self._make_log(ln)

        self.nb.add(f, text='font  ')
        self._font_load(False)

    def _build_font_editor(self, rf):
        import tkinter as tk
        from tkinter import ttk
        self._fe = {}
        grid = ttk.Frame(rf); grid.pack(fill='x')
        ttk.Label(grid, text='lang').grid(row=0, column=0, sticky='w', padx=(0, 8), pady=3)
        self._fe['lang'] = tk.Entry(grid, width=16, state='readonly')   # 用 tk.Entry, readonly 才生效
        self._fe['lang'].grid(row=0, column=1, sticky='ew', pady=3)
        ttk.Label(grid, text='name').grid(row=1, column=0, sticky='w', padx=(0, 8), pady=3)
        self._fe['name'] = ttk.Entry(grid, width=30)
        self._fe['name'].grid(row=1, column=1, sticky='ew', pady=3)
        ttk.Label(grid, text='size').grid(row=2, column=0, sticky='w', padx=(0, 8), pady=3)
        self._fe['size'] = ttk.Entry(grid, width=10)
        self._fe['size'].grid(row=2, column=1, sticky='w', pady=3)
        ttk.Label(grid, text='base_line_ext').grid(row=3, column=0, sticky='w', padx=(0, 8), pady=3)
        self._fe['base'] = ttk.Entry(grid, width=10)
        self._fe['base'].grid(row=3, column=1, sticky='w', pady=3)
        ttk.Label(grid, text='line_height_ext').grid(row=4, column=0, sticky='w', padx=(0, 8), pady=3)
        self._fe['line'] = ttk.Entry(grid, width=10)
        self._fe['line'].grid(row=4, column=1, sticky='w', pady=3)
        ttk.Label(rf, text='提示: 左树选中条目 → 在右侧表单就地编辑 → 点「修改字库」立即生效；点「保存 json」写文件',
                  foreground='#888', wraplength=340).pack(anchor='w', pady=(6, 0))
        grid.columnconfigure(1, weight=1)

    def _font_json_path(self):
        return os.path.join(self.out_abs['font'], 'scui_font_package.json')

    def _font_rebuild(self):
        """从内存副本重建左树(保留已展开的语言分组, 保证 iid 与数组下标同步)."""
        openli = set()
        if hasattr(self, '_font_lang_iid'):
            for top, li in list(self._font_lang_iid.items()):
                try:
                    if self.ftree.item(top, 'open'):
                        openli.add(li)
                except Exception:
                    pass
        self._font_lang_iid = {}
        self.ftree.delete(*self.ftree.get_children())
        for li, item in enumerate(self.font_json.get('info', [])):
            lang = item.get('lang', '?')
            top = self.ftree.insert('', 'end', text=lang, values=('',),
                                    open=(li in openli))
            self._font_lang_iid[top] = li
            sizes = item.get('size', [])
            for ei, name in enumerate(item.get('name', [])):
                sz = sizes[ei] if ei < len(sizes) else 0
                self.ftree.insert(top, 'end', text=name, values=(sz,),
                                  iid='f%d_%d' % (li, ei))

    def _font_normalize(self):
        """归一化: 保证每个条目的 name/size/ext 长度对齐, 缺失 ext 补默认 0."""
        info = self.font_json.setdefault('info', [])
        for item in info:
            n = len(item.setdefault('name', []))
            item.setdefault('size', [0] * n)
            if len(item['size']) < n:
                item['size'] += [0] * (n - len(item['size']))
            ext = item.setdefault('ext', [None] * n)
            for i in range(n):
                if not isinstance(ext[i], dict):
                    ext[i] = {'base_line_ext': 0, 'line_height_ext': 0}

    def _font_load(self, _notify):
        import tkinter as tk
        p = self._font_json_path()
        try:
            with open(p, 'r', encoding='utf-8') as fp:
                self.font_json = json.load(fp)
            self._font_normalize()
            self._font_rebuild()
            self.font_status.set('已加载: %s' % os.path.basename(p))
        except Exception as e:
            self.font_status.set('加载失败: %r' % e)

    def _on_font_sel(self, _ev=None):
        # 支持直接传 iid(添加条目后填充); 否则从当前 selection 取
        if isinstance(_ev, str):
            iid = _ev
        else:
            sel = self.ftree.selection()
            if not sel:
                return
            iid = sel[0]
        # 顶层 lang 分组: 记录 li, 供添加字号使用
        if iid in getattr(self, '_font_lang_iid', {}):
            self.font_sel = (self._font_lang_iid[iid], -1)
            for k in self._fe:
                self._fe[k].configure(state='normal'); self._fe[k].delete(0, 'end')
                self._fe[k].configure(state='readonly' if k == 'lang' else 'normal')
            self.font_status.set('已选择分组 %s(可点「添加字号」)' % self.font_json['info'][self._font_lang_iid[iid]]['lang'])
            return
        if not iid.startswith('f'):
            return
        li, ei = map(int, iid[1:].split('_'))
        self.font_sel = (li, ei)
        item = self.font_json['info'][li]
        name = item['name'][ei]
        size = item['size'][ei] if ei < len(item['size']) else 0
        self._fe['lang'].configure(state='normal'); self._fe['lang'].delete(0, 'end'); self._fe['lang'].insert(0, item['lang']); self._fe['lang'].configure(state='readonly')
        self._fe['name'].delete(0, 'end'); self._fe['name'].insert(0, name)
        self._fe['size'].delete(0, 'end'); self._fe['size'].insert(0, str(size))
        # ext(可选)
        base = line = 0
        ext = item.get('ext')
        if ext and ei < len(ext) and isinstance(ext[ei], dict):
            base = ext[ei].get('base_line_ext', 0)
            line = ext[ei].get('line_height_ext', 0)
        self._fe['base'].delete(0, 'end'); self._fe['base'].insert(0, str(base))
        self._fe['line'].delete(0, 'end'); self._fe['line'].insert(0, str(line))

    def _font_prune(self):
        """合规性: 移除没有字库条目(空 name)的语言分组, 并从 lang 白名单剔除未被引用的语言."""
        from tkinter import messagebox
        removed = []
        info = self.font_json.get('info', [])
        new_info = []
        for item in info:
            names = [n for n in item.get('name', []) if str(n).strip()]
            if names:
                item['name'] = names
                if len(item['size']) != len(names):
                    item['size'] = [0] * len(names)
                new_info.append(item)
            else:
                removed.append(item.get('lang', '?'))
        self.font_json['info'] = new_info
        used = set(i.get('lang') for i in new_info)
        lang = self.font_json.setdefault('lang', [])
        self.font_json['lang'] = [l for l in lang if l in used]
        if removed:
            messagebox.showinfo('保存 json', '已移除无效语言分组(未含字库条目): %s' % ', '.join(removed), parent=self.root)
        return removed

    def _font_save(self):
        if self.font_json is None:
            return
        if self.font_sel:
            li, ei = self.font_sel
            item = self.font_json['info'][li]
            item['name'][ei] = self._fe['name'].get().strip()
            try:
                item['size'][ei] = int(self._fe['size'].get().strip())
            except Exception:
                pass
            # ext(可选, 空=0)
            def _to_int(s):
                try:
                    return int(str(s).strip())
                except Exception:
                    return 0
            b = _to_int(self._fe['base'].get()); l = _to_int(self._fe['line'].get())
            item.setdefault('ext', [None] * len(item['name']))
            while ei >= len(item['ext']):
                item['ext'].append(None)
            item['ext'][ei] = {'base_line_ext': b, 'line_height_ext': l}
        # 合规性: 去掉只加语言未加字库的无效分组
        self._font_prune()
        try:
            with open(self._font_json_path(), 'w', encoding='utf-8') as fp:
                json.dump(self.font_json, fp, ensure_ascii=False, indent=4)
            # 若当前选中了条目, 就地同步树行文本(不重建, 保持展开)
            if self.font_sel:
                li, ei = self.font_sel
                if li < len(self.font_json['info']):
                    it = self.font_json['info'][li]
                    if ei < len(it['name']):
                        self.ftree.item('f%d_%d' % (li, ei), text=it['name'][ei],
                                        values=(it['size'][ei] if ei < len(it['size']) else 0,))
            self.font_status.set('已保存')
        except Exception as e:
            self.font_status.set('保存失败: %r' % e)

    def _font_add(self):
        from tkinter import messagebox
        if self.font_json is None:
            messagebox.showwarning('font', '尚未加载 json，请先点「加载 json」')
            return
        sel = self.ftree.selection()
        if not sel:
            messagebox.showwarning('font', '请先点选左侧一个语言分组或某个条目，再添加')
            return
        iid = sel[0]
        if iid.startswith('f'):
            li = int(iid[1:].split('_')[0])   # 点的是某 bin -> 所属分组
        else:
            li = self._font_lang_iid.get(iid)
            if li is None:
                messagebox.showwarning('font', '请先点选左侧一个语言分组')
                return
        item = self.font_json['info'][li]
        lang = item['lang']
        item.setdefault('name', [])
        item.setdefault('size', [])
        base = 'font_%s_new_%d.bin' % (lang, len(item['name']))
        item['name'].append(base)
        item['size'].append(0)
        # 就地加数组 + 重建树(iid 与数组同步, 且保留展开), 再选中新条目
        self._font_rebuild()
        new_iid = 'f%d_%d' % (li, len(item['name']) - 1)
        top_iid = next((k for k, v in self._font_lang_iid.items() if v == li), None)
        if top_iid is not None:
            self.ftree.item(top_iid, open=True)
        self.ftree.selection_set(new_iid)
        self._on_font_sel(new_iid)
        self.font_status.set('已添加(未保存，点「保存 json」同步)')

    def _font_addlang(self):
        from tkinter import simpledialog, messagebox
        if self.font_json is None:
            messagebox.showwarning('font', '尚未加载 json，请先点「加载 json」')
            return
        lang = simpledialog.askstring('添加语言', '新语言名(如 en2):', parent=self.root)
        if not lang:
            return
        lang = lang.strip()
        if not lang:
            return
        self.font_json.setdefault('lang', [])
        if lang not in self.font_json['lang']:
            self.font_json['lang'].append(lang)
        self.font_json.setdefault('info', [])
        # 同名已存在则直接选中
        for li, item in enumerate(self.font_json['info']):
            if item.get('lang') == lang:
                self._font_rebuild()
                top = next((k for k, v in self._font_lang_iid.items() if v == li), None)
                if top is not None:
                    self.ftree.item(top, open=True)
                    self.ftree.selection_set(top)
                self.font_status.set('语言 %s 已存在，已选中' % lang)
                return
        self.font_json['info'].append({'lang': lang, 'name': [], 'size': []})
        self._font_rebuild()
        li = len(self.font_json['info']) - 1
        top = next((k for k, v in self._font_lang_iid.items() if v == li), None)
        if top is not None:
            self.ftree.item(top, open=True)
            self.ftree.selection_set(top)
        self.font_status.set('已添加语言 %s(可再点「添加字号」补条目)' % lang)

    def _font_modlang(self):
        from tkinter import simpledialog, messagebox
        if self.font_json is None:
            messagebox.showwarning('font', '尚未加载 json，请先点「加载 json」')
            return
        sel = self.ftree.selection()
        if not sel:
            messagebox.showwarning('font', '请先选中一个语言分组')
            return
        iid = sel[0]
        if iid.startswith('f'):
            li = int(iid[1:].split('_')[0])
        else:
            li = self._font_lang_iid.get(iid)
            if li is None:
                messagebox.showwarning('font', '请先选中一个语言分组')
                return
        item = self.font_json['info'][li]
        old = item.get('lang', '')
        new = simpledialog.askstring('修改语言', '新语言名:', initialvalue=old, parent=self.root)
        if not new:
            return
        new = new.strip()
        if not new:
            return
        item['lang'] = new
        L = self.font_json.setdefault('lang', [])
        if old in L and not any(i.get('lang') == old for i in self.font_json['info']):
            L.remove(old)
        if new not in L:
            L.append(new)
        self._font_rebuild()
        top = next((k for k, v in self._font_lang_iid.items() if v == li), None)
        if top is not None:
            self.ftree.item(top, open=True)
            self.ftree.selection_set(top)
        self.font_status.set('语言已改为 %s(未保存)' % new)

    def _font_dellang(self):
        from tkinter import messagebox
        if self.font_json is None:
            messagebox.showwarning('font', '尚未加载 json，请先点「加载 json」')
            return
        sel = self.ftree.selection()
        if not sel:
            messagebox.showwarning('font', '请先选中一个语言分组')
            return
        iid = sel[0]
        if iid.startswith('f'):
            li = int(iid[1:].split('_')[0])
        else:
            li = self._font_lang_iid.get(iid)
            if li is None:
                messagebox.showwarning('font', '请先选中一个语言分组')
                return
        item = self.font_json['info'][li]
        lang = item.get('lang', '?')
        if not messagebox.askyesno('删除语言', '确认删除整个语言分组 [%s] 及其全部字库条目?' % lang, parent=self.root):
            return
        del self.font_json['info'][li]
        L = self.font_json.setdefault('lang', [])
        if lang in L and not any(i.get('lang') == lang for i in self.font_json['info']):
            L.remove(lang)
        self.ftree.selection_remove(self.ftree.selection())
        self.font_sel = None
        self._font_rebuild()
        self.font_status.set('已删除语言 %s(未保存)' % lang)

    def _font_modlib(self):
        from tkinter import messagebox
        if self.font_json is None:
            messagebox.showwarning('font', '尚未加载 json，请先点「加载 json」')
            return
        sel = self.ftree.selection()
        if not sel or not sel[0].startswith('f'):
            messagebox.showwarning('font', '请先在左侧选中一个具体字库条目(.bin 那一行)')
            return
        iid = sel[0]
        li, ei = map(int, iid[1:].split('_'))
        item = self.font_json['info'][li]
        self.font_sel = (li, ei)
        name = self._fe['name'].get().strip()
        if name:
            item['name'][ei] = name
        try:
            item['size'][ei] = int(self._fe['size'].get().strip())
        except Exception:
            pass
        def _to_int(s):
            try:
                return int(str(s).strip())
            except Exception:
                return 0
        b = _to_int(self._fe['base'].get()); l = _to_int(self._fe['line'].get())
        item.setdefault('ext', [None] * len(item['name']))
        while ei >= len(item['ext']):
            item['ext'].append(None)
        item['ext'][ei] = {'base_line_ext': b, 'line_height_ext': l}
        # 就地同步树行文本
        self.ftree.item('f%d_%d' % (li, ei), text=item['name'][ei],
                        values=(item['size'][ei],))
        self.font_status.set('已修改字库(未写文件，点「保存 json」同步)')

    def _font_del(self):
        from tkinter import messagebox
        if self.font_json is None:
            messagebox.showwarning('font', '尚未加载 json，请先点「加载 json」')
            return
        sel = self.ftree.selection()
        if not sel:
            messagebox.showwarning('font', '请先在左侧选中一个具体的字库条目(.bin 那一行)')
            return
        iid = sel[0]
        if not iid.startswith('f'):
            messagebox.showwarning('font', '请选中具体的字库条目(展开语言分组后点 .bin 那一行)')
            return
        li, ei = map(int, iid[1:].split('_'))
        item = self.font_json['info'][li]
        if ei < len(item['name']):
            del item['name'][ei]
        if ei < len(item['size']):
            del item['size'][ei]
        if ei < len(item.get('ext', [])):
            del item['ext'][ei]
        # 就地删数组 + 重建树(iid 与数组同步, 且保留展开)
        self.ftree.selection_remove(iid)
        self.font_sel = None
        self._font_rebuild()
        self.font_status.set('已删除(未保存，点「保存 json」同步)')

    def _font_preview(self):
        import tkinter as tk
        from tkinter import scrolledtext
        if self.font_json is None:
            return
        top = tk.Toplevel(self.root)
        top.title('json 预览(副本，未保存的修改在此可见)')
        top.geometry('720x560')
        t = scrolledtext.ScrolledText(top, wrap='none', font=('Consolas', 9), state='normal')
        t.pack(fill='both', expand=True, padx=8, pady=8)
        try:
            s = json.dumps(self.font_json, ensure_ascii=False, indent=4)
        except Exception as e:
            s = '%r' % e
        t.insert('1.0', s)
        t.configure(state='disabled')

    def _placeholder_tab(self, name):
        from tkinter import ttk
        f = ttk.Frame(self.nb, padding=12)
        f.pack(fill='both', expand=True)
        ttk.Label(f, text='子界面[%s] 建设中…' % name).pack(anchor='w', pady=(4, 0))
        ln = ttk.LabelFrame(f, text=' LOG 输出 ', padding=(6, 4))
        ln.pack(fill='both', expand=True, pady=(12, 0))
        self.logs[name] = self._make_log(ln)
        self.nb.add(f, text=name + '  ')

    def _make_log(self, parent):
        from tkinter.scrolledtext import ScrolledText
        t = ScrolledText(parent, state='disabled', wrap='none',
                         font=('Consolas', 9), height=8)
        t.pack(fill='both', expand=True)
        return t

    #--------------- image 子界面 ---------------
    def _build_image_tab(self):
        import tkinter as tk
        from tkinter import ttk
        f = ttk.Frame(self.nb, padding=8)
        self.tabs['image'] = f

        # row0: 顶部固定(项目名 + 执行)
        top = ttk.Frame(f); top.pack(fill='x')
        pr = ttk.Frame(top); pr.pack(fill='x')
        ttk.Label(pr, text='image 项目名称:').pack(side='left')
        self.proj_var = tk.StringVar(value='scui')
        ttk.Entry(pr, textvariable=self.proj_var, width=16).pack(side='left', padx=(6, 10))
        ttk.Button(pr, text='执行 image 打包', command=lambda: self._run('image')).pack(side='left')

        # 主体: 垂直 Panedwindow(上: 树|详情, 下: log) -> 可上下拖拽
        vpane = ttk.Panedwindow(f, orient='vertical')
        vpane.pack(fill='both', expand=True, pady=(4, 0))

        # 上部: 水平 Panedwindow(树 | 详情[图|文 水平]) -> 可左右拖拽
        mid = ttk.Panedwindow(vpane, orient='horizontal')
        vpane.add(mid, weight=1)

        lf = ttk.LabelFrame(mid, text=' 图片资源 ', padding=(4, 4))
        mid.add(lf, weight=3)
        self.tree = ttk.Treeview(lf, columns=('size',), show='tree headings',
                                 selectmode='browse')
        self.tree.heading('#0', text='路径/文件')
        self.tree.heading('size', text='大小')
        self.tree.column('size', width=80, anchor='e', stretch=False)
        vs = ttk.Scrollbar(lf, orient='vertical', command=self.tree.yview)
        self.tree.configure(yscrollcommand=vs.set)
        self.tree.pack(side='left', fill='both', expand=True)
        vs.pack(side='right', fill='y')

        root_src = self.in_abs['image'] if os.path.isdir(self.in_abs['image']) else self.ui
        self._tree_root_path = root_src
        try:
            root_show = os.path.relpath(root_src, self.in_abs['image'] or root_src)
        except Exception:
            root_show = root_src
        self._tree_root = self.tree.insert('', 'end', iid='root', text=root_show, open=True)
        self._vid_map = {}
        self._vid_count = 0
        self._load_dir(self._tree_root, root_src)
        self.tree.bind('<<TreeviewOpen>>', self._on_tree_open)
        self.tree.bind('<<TreeviewSelect>>', self._on_tree_sel)

        rf = ttk.LabelFrame(mid, text=' 详情(scui_image) ', padding=(6, 4))
        mid.add(rf, weight=2)
        dbody = ttk.Frame(rf); dbody.pack(fill='both', expand=True)
        self.preview = ttk.Label(dbody, text='选择左侧图片查看预览', anchor='center',
                                 width=26, relief='groove')
        self.preview.pack(side='left', fill='y', padx=(0, 6))
        self.field_text = tk.Text(dbody, font=('Consolas', 9), state='disabled', wrap='none')
        self.field_text.pack(side='left', fill='both', expand=True)

        # 下部: LOG(默认与上部各占一半, 可上下拖动)
        ln = ttk.LabelFrame(vpane, text=' LOG 输出 ', padding=(6, 4))
        vpane.add(ln, weight=1)
        self.logs['image'] = self._make_log(ln)

        self.nb.add(f, text='image  ')

    def _load_dir(self, parent, path):
        """懒加载: 只填一级目录 + 图片文件."""
        import tkinter as tk
        from PIL import Image, ImageTk
        try:
            entries = sorted(os.listdir(path),
                             key=lambda x: (not os.path.isdir(os.path.join(path, x)), x.lower()))
        except OSError:
            return
        for e in entries:
            full = os.path.join(path, e)
            if os.path.isdir(full):
                node = self.tree.insert(parent, 'end', text=e, values=(u'[目录]',), open=False)
                self.tree.insert(node, 'end', text='…', values=('',))
            elif e.lower().endswith(_IMG_EXT):
                try:
                    im = Image.open(full); im.thumbnail((18, 18))
                    ph = ImageTk.PhotoImage(im); self._imgs.append(ph)
                except Exception:
                    ph = None
                size = self._fsize(full)
                self.tree.insert(parent, 'end', text=e, image=ph, values=(size,),
                                 tags=('img',),
                                 iid='v%d' % self._vid_count)
                self._vid_map[self._vid_count] = full
                self._vid_count += 1

    def _fsize(self, p):
        try:
            n = os.path.getsize(p)
            if n >= 1024 * 1024:
                return '%.2fM' % (n / 1024 / 1024)
            return '%dK' % (n / 1024)
        except OSError:
            return ''

    def _on_tree_open(self, ev):
        node = ev.widget.focus()
        kids = self.tree.get_children(node)
        if len(kids) == 1:
            txt = self.tree.item(kids[0], 'text')
            if txt == '…':
                self.tree.delete(kids[0])
                path = self._node_path(node)
                self._load_dir(node, path)

    def _node_path(self, node):
        parts = []
        cur = node
        while cur:
            parts.insert(0, self.tree.item(cur, 'text'))
            cur = self.tree.parent(cur)
        return os.path.join(self._tree_root_path, *parts[1:])

    def _on_tree_sel(self, ev):
        sel = self.tree.selection()
        if not sel:
            return
        iid = sel[0]
        if iid.startswith('v'):
            path = self._vid_map[int(iid[1:])]
            self._show_img(path)

    def _show_img(self, path):
        import tkinter as tk
        from PIL import Image, ImageTk
        preview_err = None
        # 预览图无条件显示
        try:
            im = Image.open(path)
            im.load()
            im.thumbnail((180, 180))
            ph = ImageTk.PhotoImage(im)
            self._imgs.append(ph)
            self.preview.configure(image=ph, text='')
        except Exception as e:
            preview_err = '%r' % e
            self.preview.configure(image='', text=os.path.basename(path))

        # 构造 file_tag(与打包脚本同规则: 去点/去斜杠/去空格 -> 下划线)
        try:
            rel = os.path.relpath(path, self.in_abs['image'])
        except Exception:
            rel = os.path.basename(path)
        file_tag = rel.replace('.', '').replace('\\', '_').replace('/', '_').replace(' ', '_')

        lines = []
        if preview_err:
            lines.append('【预览失败】 %s' % preview_err)
            lines.append('')
        # 匹配 .h 句柄信息
        h_info = self._image_from_h(file_tag)
        if h_info:
            lines.append('【句柄信息（scui_image_parser.h）】')
            lines.append('    %-18s = %s;' % ('handle_name', h_info['name']))
            lines.append('    %-18s = %s;' % ('handle_value', h_info['value']))
            lines.append('    %-18s = %s;' % ('pixel.width', h_info['width']))
            lines.append('    %-18s = %s;' % ('pixel.height', h_info['height']))
            lines.append('    %-18s = %s;' % ('pixel.size_bin', h_info['size_bin']))
            lines.append('    %-18s = %s;' % ('pixel.size_mem', h_info['size_mem']))
            lines.append('    %-18s = %s;' % ('com_pct', h_info['com_pct']))
            lines.append('')
        # 匹配 .c 结构体信息
        c_info = self._image_from_c(file_tag)
        if c_info:
            lines.append('【资源信息（scui_image_parser.c）】')
            for k, v in c_info.items():
                lines.append('    %-18s = %s;' % (k, v))
            lines.append('')
        # 都没匹配到
        if not h_info and not c_info:
            lines.append('【未匹配到打包记录 — 请先执行 image 打包】')
            lines.append('    (file_tag: %s)' % file_tag)

        self.field_text.configure(state='normal')
        self.field_text.delete('1.0', 'end')
        self.field_text.insert('1.0', '\n'.join(lines))
        self.field_text.configure(state='disabled')

    def _image_from_h(self, file_tag):
        """从 scui_image_parser.h 匹配句柄信息(file_tag 为去点后的相对路径)."""
        out_h = os.path.join(self.out_abs.get('image', ''), 'scui_image_parser.h')
        if not os.path.exists(out_h):
            return None
        try:
            with open(out_h, 'r', encoding='utf-8', errors='replace') as hf:
                content = hf.read()
            # 枚举项以 file_tag 结尾: scui_image_<prefix><file_tag>, // 0xNNNN
            re_enum = r'(scui_image_[A-Za-z0-9_]*%s)\s*,\s*//\s*(0x[0-9a-fA-F]+)' % re.escape(file_tag)
            m = re.search(re_enum, content)
            if not m:
                return None
            name = m.group(1)
            value = m.group(2)
            # 压缩注释行也以同后缀结尾: //< w,h,size_bin,size_mem,com_pct> scui_image_<prefix><file_tag>
            re_comment = r'//<\s*([^>]+)>\s*scui_image_[A-Za-z0-9_]*%s\b' % re.escape(file_tag)
            m2 = re.search(re_comment, content)
            if not m2:
                return None
            # 注释里为十六进制径 w,h,size_bin,size_mem(0x...)
            nums = re.findall(r'0x([0-9a-fA-F]+)', m2.group(1))
            ws = [int(x, 16) for x in nums]
            if len(ws) < 4:
                return None
            w, h, bin_, mem = ws[0], ws[1], ws[2], ws[3]
            return {
                'name': name,
                'value': value,
                'width': w,
                'height': h,
                'size_bin': bin_,
                'size_mem': mem,
                'com_pct': '%.4f' % (float(bin_) / mem) if mem else 1.0,
            }
        except OSError:
            return None

    def _image_from_c(self, file_tag):
        """从 scui_image_parser.c 匹配 scui_image_t 结构体."""
        out_c = os.path.join(self.out_abs.get('image', ''), 'scui_image_parser.c')
        if not os.path.exists(out_c):
            return None
        try:
            with open(out_c, 'r', encoding='utf-8', errors='replace') as cf:
                content = cf.read()
            # 结构体名以 file_tag 结尾(无 scui_image_ 前缀): static const scui_image_t <prefix><file_tag>
            re_struct = r'static\s+const\s+scui_image_t\s+([A-Za-z0-9_]*%s)\s*=\s*\{([^}]+)\}' % re.escape(file_tag)
            m = re.search(re_struct, content, re.DOTALL)
            if not m:
                return None
            body = m.group(2)   # group(1)=结构体名, group(2)={...}体
            info = {}
            # 解析各字段(脚本生成以 ','/';' 结尾)
            for line in body.split('\n'):
                line = line.strip()
                if '=' in line and line[-1] in (';', ','):
                    k, v = line.split('=', 1)
                    k = k.strip().lstrip('.')
                    v = v.strip().rstrip(';').rstrip(',').strip()
                    info[k] = v
            return info
        except OSError:
            return None

    #--------------- 执行打包(子线程, UI 不卡) ---------------
    def _run(self, name):
        if getattr(self, '_busy', False):
            return
        self._busy = True
        self._status('执行中…(%s)' % name)
        src = self.in_abs[name]
        dst = self.out_abs[name]
        proj = getattr(self, 'proj_var', None)
        proj = proj.get() if proj else 'scui'
        self._append_log(name, '\n\n==== start: %s ====\n' % name)

        self._logq = queue.Queue()
        wire = _TabLog(self._logq)

        def worker():
            so, se = sys.stdout, sys.stderr
            si = sys.stdin
            sys.stdout = wire
            sys.stderr = wire
            sys.stdin = _NoStdin()          # 无控制台 exe 下避免 input() 阻塞
            try:
                _do_task(name, self.ui, src, dst, proj)
            finally:
                sys.stdout, sys.stderr, sys.stdin = so, se, si
                self._logq.put('__DONE__')

        threading.Thread(target=worker, daemon=True).start()

    def _append_log(self, name, txt):
        log = self.logs.get(name)
        if log is None:
            return
        log.configure(state='normal')
        log.insert('end', txt)
        log.see('end')
        log.configure(state='disabled')

    def _poll(self):
        if self._logq is not None:
            log = self.logs.get(self._current_name) or self.logs.get('image')
            # 每帧限流, 避免海量日志一次性插入拖慢主线程
            n = 0
            while n < 500:
                try:
                    txt = self._logq.get_nowait()
                except queue.Empty:
                    break
                n += 1
                if txt == '__DONE__':
                    self._busy = False
                    self._status('完成')
                    continue
                log.configure(state='normal')
                log.insert('end', txt)
                log.see('end')
                log.configure(state='disabled')
        self.root.after(120, self._poll)

    def _status(self, s):
        self.root.title('scui 打包工具 - %s' % s)

    def _on_tab(self, ev):
        idx = self.nb.index(self.nb.select())
        names = ['widget', 'image', 'font', 'lang', 'cwf']
        self._current_name = names[idx] if idx < len(names) else 'widget'

    def _about(self):
        from tkinter import messagebox
        messagebox.showinfo('scui 打包工具',
            'scui 资源打包统一前端\n\n类型: image / font / lang / cwf\n'
            '依赖: image需要 pillow/lz4; image.7z解压需要 py7zr\n来源: scui/tools 与 scui/plugs')

    # 工具内打开 out/err 日志浏览(左 out 右 err, 可滚动)
    def _open_logs(self):
        import tkinter as tk
        from tkinter import ttk, scrolledtext
        name = self._current_name
        base = os.path.splitext(_TASK[name]['as_main'][0])[0]
        dst  = self.out_abs[name]
        top = tk.Toplevel(self.root)
        top.title('日志浏览 - %s' % name)
        top.geometry('880x540')
        pan = ttk.Panedwindow(top, orient='horizontal')
        pan.pack(fill='both', expand=True, padx=8, pady=8)
        for side, ext in (('out', '.out'), ('err', '.err')):
            fr = ttk.LabelFrame(pan, text='  %s  ' % ext, padding=(4, 4))
            pan.add(fr, weight=1)
            t = scrolledtext.ScrolledText(fr, wrap='none', font=('Consolas', 9))
            t.pack(fill='both', expand=True)
            path = os.path.join(dst, base + ext)
            if os.path.exists(path):
                try:
                    with open(path, 'r', encoding='utf-8', errors='replace') as fh:
                        t.insert('1.0', fh.read())
                except Exception as e:
                    t.insert('1.0', '读取失败: %r' % e)
            else:
                t.insert('1.0', '(未找到: %s)' % os.path.basename(path))
            t.configure(state='disabled')

class _TabLog(object):
    """把 print 写入的文本重定向进指定队列(保留换行), 由 GUI 日志泵写日志区."""
    def __init__(self, q):
        self.q = q
    def write(self, s):
        if s:
            self.q.put(s)
    def flush(self):
        pass

class _NoStdin(object):
    """无控制台环境下替换 stdin, 使 input() 立即抛 EOFError 而非阻塞."""
    def read(self, *a, **k):
        raise EOFError
    def readline(self, *a, **k):
        raise EOFError
    def readlines(self, *a, **k):
        raise EOFError
    def __iter__(self):
        return self
    def __next__(self):
        raise StopIteration

#============================================================
# 入口
#============================================================
def main():
    # 无控制台(exe)下 stdout/stderr 为 None, 替换为 devnull 避免意外崩溃
    if sys.stdout is None:
        sys.stdout = open(os.devnull, 'w')
    if sys.stderr is None:
        sys.stderr = open(os.devnull, 'w')

    # 轻量手动解析: 首个位置参数作为类型(须为已知类型); 未知参数一律忽略, 回退 GUI
    argv = sys.argv[1:]
    typ = ui = src = dst = proj = ''
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == '--ui' and i + 1 < len(argv):
            ui = argv[i + 1]; i += 2
        elif a == '--src' and i + 1 < len(argv):
            src = argv[i + 1]; i += 2
        elif a == '--dst' and i + 1 < len(argv):
            dst = argv[i + 1]; i += 2
        elif a == '--proj' and i + 1 < len(argv):
            proj = argv[i + 1]; i += 2
        elif a.startswith('--'):
            i += 1                            # 未知选项忽略
        else:
            if typ == '' and a in _TASK:
                typ = a                       # 仅首个位置参数可为已知类型
            i += 1                            # 其它位置参数(如误拖入的路径)忽略

    ui_root = os.path.normpath(ui or os.getcwd())
    if typ:
        sdef, ddef = _DEFAULT[typ]
        src = src or os.path.join(ui_root, sdef)
        dst = dst or os.path.join(ui_root, ddef)
        return _do_task(typ, ui_root, src, dst, proj)
    return _launch_gui(ui_root)

if __name__ == '__main__':
    sys.exit(main())