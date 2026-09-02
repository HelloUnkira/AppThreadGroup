# 控件资源打包
import re
import os
import sys
import json


# ============================================================
# 配置(原 scui_widget_parser.json 内联到脚本头部)
# 句柄表偏移:控件
SCUI_WIDGET_PARSER_OFFSET_NAME = 'SCUI_HANDLE_OFFSET_WIDGET'
SCUI_WIDGET_PARSER_OFFSET_VALUE = '0x1000 - 1'
# ============================================================

# 分析结果(脚本启动时由内联分析函数生成)
SCUI_WIDGET_PARSER_PREFIXES = []
SCUI_WIDGET_PARSER_FIRST_FIELDS = []
SCUI_WIDGET_PARSER_PATH_SLOTS = {}
SCUI_WIDGET_PARSER_CLASS_MAKER = {}
# tools 目录基准: 打包(exe)环境下由装入方注入真实路径; 源码运行默认取 __file__
SCUI_WIDGET_TOOLS = None
# ui(scui_ui_res) 基准: GUI 注入, 日志路径相对化显示
SCUI_UI_ROOT = None
# tools 基准: GUI 注入(读句柄偏移配置)
SCUI_TOOLS = None


# 句柄偏移: 前端"设置句柄"写入的 json 覆盖, 无则用头部默认
def _handle_offset(tag, name, value):
    try:
        cfg = os.path.join(SCUI_TOOLS or SCUI_WIDGET_TOOLS or os.path.dirname(__file__),
                           'scui_pack_handle.json')
        if os.path.isfile(cfg):
            j = json.load(open(cfg, encoding='utf-8'))
            item = j.get(tag, {})
            if item.get('name'):
                name = item['name']
            if item.get('value'):
                value = item['value']
    except Exception:
        pass
    return name, value


# 日志路径显示: 相对 ui(scui_ui_res) 打印, 命令行(GUI未注入)保持绝对
def _rel_ui(p):
    if not SCUI_UI_ROOT:
        return p
    try:
        r = os.path.relpath(p, SCUI_UI_ROOT).replace('\\', '/')
        if r.startswith('..'):
            return p
        return 'scui_ui_res => %s' % r
    except Exception:
        return p


# 启动准备: 生成动态参数表(内存, 不写 tmp)
def scui_widget_parser_ready():
    global SCUI_WIDGET_PARSER_PREFIXES, SCUI_WIDGET_PARSER_FIRST_FIELDS, SCUI_WIDGET_PARSER_PATH_SLOTS, SCUI_WIDGET_PARSER_CLASS_MAKER
    global SCUI_WIDGET_PARSER_OFFSET_NAME, SCUI_WIDGET_PARSER_OFFSET_VALUE
    try:
        SCUI_WIDGET_PARSER_OFFSET_NAME, SCUI_WIDGET_PARSER_OFFSET_VALUE = \
            _handle_offset('widget', SCUI_WIDGET_PARSER_OFFSET_NAME, SCUI_WIDGET_PARSER_OFFSET_VALUE)
        base = SCUI_WIDGET_TOOLS or os.path.dirname(__file__)
        result = scui_widget_analyze_result(base)
        SCUI_WIDGET_PARSER_PREFIXES = result['prefixes']
        SCUI_WIDGET_PARSER_FIRST_FIELDS = [tuple(x) for x in result['first_fields']]
        SCUI_WIDGET_PARSER_PATH_SLOTS = result['path_slots']
        SCUI_WIDGET_PARSER_CLASS_MAKER = result.get('class_maker', {})
        return True
    except Exception as e:
        print('[cfg ready] failed: %s' % e)
        return False


# 前置字段列表(启动时从分析结果加载)
def scui_widget_parser_first_fields():
    return SCUI_WIDGET_PARSER_FIRST_FIELDS


# 控件类型 → maker 类型映射(启动时从分析结果加载, 不import模块)
SCUI_WIDGET_PARSER_CLASS_MAKER = {}
def scui_widget_parser_class_maker():
    return SCUI_WIDGET_PARSER_CLASS_MAKER


# 字段路径转C标识符段
def scui_widget_parser_field_ident(path):
    return re.sub(r'[\.\[\]]', '_', path).rstrip('_')


# 生成字段setter名
def scui_widget_parser_setter_name(field, prefix):
    if field.startswith('widget.'):
        return 'scui_widget_json_' + scui_widget_parser_field_ident(field)
    return 'scui_widget_json_%s_%s' % (prefix, scui_widget_parser_field_ident(field))


# 字段路径规范化(去数组索引: sibling[2] → sibling)
def scui_widget_parser_path_normalize(field):
    return re.sub(r'\[\d+\]', '', field)


# 字段值槽成员选择(查分析表: 按控件类型查完整路径 → 值槽; 未命中校验报错)
def scui_widget_parser_val_member(field, wtype):
    if field == 'widget.event_cb':
        return 'event'
    if 'color' in field and field.endswith('.full'):
        return 'color'
    maker = scui_widget_parser_class_maker().get(wtype, '')
    path = scui_widget_parser_path_normalize(field)
    slot = SCUI_WIDGET_PARSER_PATH_SLOTS.get(maker, {}).get(path)
    if slot is None:
        print('[json check] field not in code: %s (type %s)' % (field, wtype))
        return 'handle'
    return slot


# 流式处理所有widget文件
# 输出: parser.h(句柄枚举+表键声明+一级表声明)
#       parser.c(event_cb空入口+static setter+cfg数组+val数组+表键定义+一级表)
# 字段枚举与key_find为源维护(scui_widget.h / scui_widget_map.c), 脚本只同步生成映射
def scui_widget_parser_scene_list(scene_list, scui_widget_parser_list, defaults_map):
    parser_h = scui_widget_parser_list[0]
    parser_c = scui_widget_parser_list[1]
    # 前置字段(未构建阶段获取, 固定前段): 从分析结果加载
    first_fields = scui_widget_parser_first_fields()
    first_set = set(f for (scope, f) in first_fields)
    # 头文件添加前缀, 源文件添加前缀
    parser_h.write('#ifndef SCUI_WIDGET_PARSER_H\n')
    parser_h.write('#define SCUI_WIDGET_PARSER_H\n\n')
    parser_h.write('/*一个通过scui_widget_parser脚本生成的widget表\n */\n\n')
    parser_c.write('/*一个通过scui_widget_parser脚本生成的widget表\n */\n\n')
    parser_c.write('#include "scui.h"\n\n')
    # 填充句柄枚举
    offset_name = SCUI_WIDGET_PARSER_OFFSET_NAME
    offset_value = SCUI_WIDGET_PARSER_OFFSET_VALUE
    parser_h.write('typedef enum {\n')
    parser_h.write('\t%s = %s,\n' % (offset_name, offset_value))
    scui_widget_parser_handle_num = 0
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_widget_parser_handle_num += 1
                scui_widget_parser_handle_ofs = eval(offset_value) + scui_widget_parser_handle_num
                parser_h.write('\t%s, // %s, %s\n' % (widget['widget.myself'], scui_widget_parser_handle_ofs, hex(scui_widget_parser_handle_ofs)))
            except Exception as e:
                print(widget)
                print(e)
                return
    parser_h.write('} scui_widget_parser_handle_t;\n\n')
    parser_h.write('extern const void * const scui_widget_parser_table[%s];\n\n' % scui_widget_parser_handle_num)
    # 填充event_cb空入口(parser.c)
    # EMPTY: 统一一个空函数, 本地宏替换各事件名(去重)
    event_cb_list = []
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                cb = widget['widget.event_cb']
                if cb not in event_cb_list:
                    event_cb_list.append(cb)
            except Exception as e:
                pass
    parser_c.write('#if defined(SCUI_WIDGET_PARSER_EVENT_CB_EMPTY) && SCUI_WIDGET_PARSER_EVENT_CB_EMPTY == 1\n')
    parser_c.write('static void scui_widget_parser_event_cb_empty(scui_event_t *event)\n{\n}\n')
    # 垂直对齐: 按最长宏名补白 + 固定间隙(不在乎整体长度)
    cb_pad = max(len(cb) for cb in event_cb_list) + 1
    for cb in event_cb_list:
        parser_c.write('#define %-*s scui_widget_parser_event_cb_empty\n' % (cb_pad, cb))
    parser_c.write('#else\n')
    for cb in event_cb_list:
        parser_c.write('extern void %s(scui_event_t *event);\n' % cb)
    parser_c.write('#endif\n\n')
    # 类名 → maker 映射
    class_maker = {}
    prefix_class = {}
    for wclass, maker in scui_widget_parser_class_maker().items():
        prefix = maker
        if prefix.startswith('scui_'):
            prefix = prefix[5:]
        if prefix.endswith('_maker_t'):
            prefix = prefix[:-len('_maker_t')]
        class_maker[wclass] = (maker, prefix + '_maker', prefix)
        prefix_class[prefix] = (wclass, maker)
    # 前置字段setter: 从源枚举(scui_widget_json_field_t)推导, 不硬编码
    setter_map = {}
    for (scope, f) in first_fields:
        if scope == 'base':
            key = ('base', f)
            fvar = 'widget_maker'
            fcast = 'scui_widget_maker_t'
            fpre = 'widget'
            member = f[len('widget.'):]
            # 基域字段查任意类型表(widget基域所有类型都有), 用window
            wclass = 'scui_widget_type_window'
        else:
            if scope not in prefix_class:
                print('[field setter] unknown prefix: %s' % scope)
                continue
            wclass, maker = prefix_class[scope]
            key = (wclass, f)
            fvar = scope + '_maker'
            fcast = maker
            fpre = scope
            member = f
        setter_map[key] = {
            'name':   scui_widget_parser_setter_name(f, fpre),
            'fvar':   fvar,
            'fcast':  fcast,
            'member': member,
            'val':    scui_widget_parser_val_member(f, wclass),
        }
    # child_num特殊setter(自动统计字段, 不在源枚举)
    setter_map[('base', 'widget.child_num')] = {'name': 'scui_widget_json_widget_child_num', 'fvar': 'widget_maker', 'fcast': 'scui_widget_maker_t', 'member': 'child_num', 'val': 'handle'}
    # 收集字段setter定义集合(从场景json)
    for scene in scene_list:
        for widget in scene['widget']:
            wtype = widget['widget.type']
            if wtype not in class_maker:
                print('\nwidget type unknown\n')
                print(widget)
                continue
            maker, var, prefix = class_maker[wtype]
            for field in widget:
                if field == 'annotation':
                    continue
                if field.startswith('widget.'):
                    key = ('base', field)
                    fvar = 'widget_maker'
                    fcast = 'scui_widget_maker_t'
                    fpre = 'widget'
                    member = field[len('widget.'):]
                else:
                    key = (wtype, field)
                    fvar = var
                    fcast = maker
                    fpre = prefix
                    member = field
                setter_map[key] = {
                    'name':   scui_widget_parser_setter_name(field, fpre),
                    'fvar':   fvar,
                    'fcast':  fcast,
                    'member': member,
                    'val':    scui_widget_parser_val_member(field, wtype),
                }
    # 其余字段顺序(按字典序, 不含枚举前置字段)
    rest_order = []
    rest = set()
    for key in setter_map:
        f = key[1]
        if f not in first_set:
            rest.add(f)
    rest_order.extend(sorted(rest))
    # 填充字段setter定义(parser.c, static; 供cfg数组引用)
    for key in sorted(setter_map):
        s = setter_map[key]
        parser_c.write('static void %s(void *maker, void *field)\n' % s['name'])
        parser_c.write('{\n')
        parser_c.write('\t%s *%s = (%s *)maker;\n' % (s['fcast'], s['fvar'], s['fcast']))
        parser_c.write('\t%s->%s = ((scui_widget_json_val_t *)field)->%s;\n' % (s['fvar'], s['member'], s['val']))
        parser_c.write('}\n\n')
    # 对目标控件集合进行流式处理,生成cfg数组+val数组+表键(parser.c)
    for scene in scene_list:
        for widget in scene['widget']:
            myself = widget['widget.myself']
            print(myself)
            wtype = widget['widget.type']
            if wtype not in class_maker:
                continue
            maker, var, prefix = class_maker[wtype]
            # 统计本控件有多少布局孩子
            child_num = 0
            for field in widget:
                if field == 'widget.child_num':
                    child_num = eval(widget[field])
                    break
            for target in scene['widget']:
                try:
                    if myself == target['widget.parent']:
                        child_num += 1
                except Exception as e:
                    pass
            # 收集字段条目(前段枚举槽对齐+后段其余字段紧凑)
            entry_list = []
            # 前段: 枚举槽(与字段枚举下标对齐, 控件无该字段则NULL占位)
            for (scope, f) in first_fields:
                if scope == 'base':
                    fkey = ('base', f)
                    applicable = True
                else:
                    fkey = (wtype, f)
                    applicable = (scope == prefix)
                if not applicable:
                    entry_list.append((None, 'handle', '0'))
                    continue
                if f in widget:
                    value = widget[f]
                elif f == 'widget.parent':
                    value = 'SCUI_HANDLE_INVALID'
                elif f == 'preload':
                    value = '0'
                else:
                    entry_list.append((None, 'handle', '0'))
                    continue
                s = setter_map[fkey]
                entry_list.append((s['name'], s['val'], value))
            # 后段: 其余字段(按字典序)
            for f in rest_order:
                if f not in widget:
                    continue
                value = widget[f]
                if f.startswith('widget.'):
                    s = setter_map[('base', f)]
                else:
                    s = setter_map[(wtype, f)]
                entry_list.append((s['name'], s['val'], value))
            # 自动统计的child_num(json未显式但存在静态子)
            if child_num != 0 and 'widget.child_num' not in widget:
                entry_list.append(('scui_widget_json_widget_child_num', 'handle', str(child_num)))
            # 填充字段函数钩子数组
            parser_c.write('static void (*const scui_widget_%s_cfg[])(void *maker, void *field) = {\n' % myself)
            for (sname, vmember, value) in entry_list:
                if sname is None:
                    parser_c.write('\tNULL,\n')
                else:
                    parser_c.write('\t%s,\n' % sname)
            parser_c.write('};\n\n')
            # 填充字段值数组
            parser_c.write('static const scui_widget_json_val_t scui_widget_%s_val[] = {\n' % myself)
            for (sname, vmember, value) in entry_list:
                if sname is None:
                    parser_c.write('\t{ .handle = 0, },\n')
                else:
                    parser_c.write('\t{ .%s = %s, },\n' % (vmember, value))
            parser_c.write('};\n\n')
            # 填充表键定义(parser.c)
            parser_c.write('const scui_widget_json_key_t scui_widget_%s_key = {\n' % myself)
            parser_c.write('\t.num = scui_arr_len(scui_widget_%s_cfg),\n' % myself)
            parser_c.write('\t.val = scui_widget_%s_val,\n' % myself)
            parser_c.write('\t.cfg = scui_widget_%s_cfg,\n' % myself)
            parser_c.write('};\n\n')
            # 填充表键声明(parser.h)
            parser_h.write('extern const scui_widget_json_key_t scui_widget_%s_key;\n' % myself)
        print()
    print()
    parser_h.write('\n#endif\n')
    # 填充一级数据表(parser.c)
    parser_c.write('const void * const scui_widget_parser_table[%s] = {\n' % scui_widget_parser_handle_num)
    for scene in scene_list:
        for widget in scene['widget']:
            parser_c.write('\t(void *)&scui_widget_%s_key,\n' % widget['widget.myself'])
    parser_c.write('};\n')
    pass


# 遍历整个文件夹,提取目标文件
def scui_widget_parser_collect(file_path_list, path, default_json):
    if os.path.isfile(path):
        if os.path.splitext(path)[1] == '.json':
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', path):
                # 排除默认配置文件，它由单独参数传入
                if os.path.basename(path) != default_json:
                    file_path_list.append(path)
    if os.path.isdir(path):
        for item in os.listdir(path):
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', os.path.join(path, item)):
                scui_widget_parser_collect(file_path_list, os.path.join(path, item), default_json)


# 打印重定向到文件
class ScuiRedirectPrint(object):
    def __init__(self, stream=sys.stdout, path='.', file='log.txt'):
        if not os.path.exists(path):
            os.makedirs(path)
        self.log = open(os.path.join(path, file), mode='w+', encoding='utf-8')
        self.terminal = stream

    def write(self, message):
        self.log.write(message)
        self.terminal.write(message)

    def flush(self):
        pass


# 生成 scui_xxx_maker.c / scui_xxx_maker.h（从默认配置 JSON 自动生成）
def scui_widget_maker_generate(dst_path, def_path, defaults_map):
    base_name = os.path.splitext(os.path.basename(def_path))[0]
    scui_widget_maker_h = open(os.path.join(dst_path, base_name + '.h'), mode='w', encoding='utf-8')
    scui_widget_maker_c = open(os.path.join(dst_path, base_name + '.c'), mode='w', encoding='utf-8')

    guard = base_name.upper() + '_H'
    scui_widget_maker_h.write('#ifndef %s\n' % guard)
    scui_widget_maker_h.write('#define %s\n\n' % guard)
    scui_widget_maker_h.write('/*一个通过scui_widget_parser脚本生成的widget动态构造器配置\n */\n\n')
    scui_widget_maker_h.write('/*@brief 控件构造器默认初始化\n')
    scui_widget_maker_h.write(' *@param maker 控件构造器实例指针\n')
    scui_widget_maker_h.write(' *@param type  控件类型(scui_widget_type_t)\n')
    scui_widget_maker_h.write(' */\n')
    scui_widget_maker_h.write('void scui_ui_maker(void *maker, scui_widget_type_t type);\n\n')
    scui_widget_maker_h.write('#endif\n')
    scui_widget_maker_h.close()

    class_to_maker = scui_widget_parser_class_maker()

    scui_widget_maker_c.write('/*一个通过scui_widget_parser脚本生成的widget动态构造器配置\n */\n\n')
    scui_widget_maker_c.write('#include "scui.h"\n\n')
    scui_widget_maker_c.write('/*@brief 控件构造器默认初始化\n')
    scui_widget_maker_c.write(' *@param maker 控件构造器实例指针\n')
    scui_widget_maker_c.write(' *@param type  控件类型(scui_widget_type_t)\n')
    scui_widget_maker_c.write(' */\n')
    scui_widget_maker_c.write('void scui_ui_maker(void *maker, scui_widget_type_t type)\n')
    scui_widget_maker_c.write('{\n')
    scui_widget_maker_c.write('\tswitch (type) {\n')

    for wclass, wdefault in defaults_map.items():
        maker_type = class_to_maker.get(wclass, '')
        if not maker_type:
            # 从类名推导 maker 类型: scui_widget_type_xxx → scui_xxx_maker_t
            suffix = wclass.replace('scui_widget_type_', '')
            maker_type = 'scui_' + suffix + '_maker_t'
        if not wdefault:
            continue
        # 从 maker_type 提取变量名: scui_xxx_maker_t → xxx_maker
        var_name = maker_type
        if var_name.startswith('scui_'):
            var_name = var_name[5:]
        if var_name.endswith('_t'):
            var_name = var_name[:-2]
        scui_widget_maker_c.write('\tcase %s: {\n' % wclass)
        scui_widget_maker_c.write('\t\t%s *%s = (%s *)maker;\n\t\t\n' % (maker_type, var_name, maker_type))
        # 垂直对齐: 按最长字段名补白 + 固定间隙(不在乎整体长度)
        field_max = max((len(field) for field, _ in wdefault.items()), default=0) + 1
        for field, value in wdefault.items():
            c_field = field.replace('.', '.')
            scui_widget_maker_c.write('\t\t%s->%-*s = %s;\n' % (var_name, field_max, c_field, value))
        scui_widget_maker_c.write('\t\tbreak;\n')
        scui_widget_maker_c.write('\t}\n')

    scui_widget_maker_c.write('\tdefault:\n')
    scui_widget_maker_c.write('\t\tbreak;\n')
    scui_widget_maker_c.write('\t}\n')
    scui_widget_maker_c.write('}\n')
    scui_widget_maker_c.close()


# JSON 对齐格式化：将 dict 内字段按最长 key 对齐 value 列
def scui_widget_parser_json_aligned_format(obj, indent=0, step=4):
    """递归格式化 JSON 对象，对每个 dict 内的字段做 value 列对齐"""
    sp = ' ' * step
    isp = sp * indent

    if isinstance(obj, dict):
        if not obj:
            return '{}'
        # 计算最长 key 长度（含引号）
        max_klen = max(len(json.dumps(k, ensure_ascii=False)) for k in obj.keys())
        lines = ['{']
        items = list(obj.items())
        for i, (k, v) in enumerate(items):
            key_str = json.dumps(k, ensure_ascii=False)
            pad = ' ' * (max_klen - len(key_str))
            val_str = scui_widget_parser_json_aligned_format(v, indent + 1, step)
            comma = ',' if i < len(items) - 1 else ''
            lines.append('%s%s: %s%s%s' % (isp + sp, key_str, pad, val_str, comma))
        lines.append(isp + '}')
        return '\n'.join(lines)

    elif isinstance(obj, list):
        if not obj:
            return '[]'
        lines = ['[']
        for i, item in enumerate(obj):
            val_str = scui_widget_parser_json_aligned_format(item, indent + 1, step)
            comma = ',' if i < len(obj) - 1 else ''
            lines.append('%s%s%s' % (isp + sp, val_str, comma))
        lines.append(isp + ']')
        return '\n'.join(lines)

    elif isinstance(obj, bool):
        return 'true' if obj else 'false'
    elif isinstance(obj, (int, float)):
        return json.dumps(obj, ensure_ascii=False)
    elif obj is None:
        return 'null'
    else:
        return json.dumps(obj, ensure_ascii=False)


def scui_widget_parser_json_realign(src_path, def_name):
    """重新对齐 scene_src 下所有 JSON 文件的字段"""
    count = 0
    for dirpath, dirnames, filenames in os.walk(src_path):
        dirnames[:] = [d for d in dirnames if '#' not in os.path.join(dirpath, d)]
        for fname in filenames:
            full = os.path.join(dirpath, fname)
            if '#' in full or not fname.endswith('.json'):
                continue
            if fname == def_name:
                continue
            try:
                with open(full, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                formatted = scui_widget_parser_json_aligned_format(data, step=4)
                with open(full, 'w', encoding='utf-8') as f:
                    f.write(formatted)
                    f.write('\n')
                count += 1
            except Exception as e:
                print('  [JSON realign] SKIP %s: %s' % (os.path.basename(full), e))
    if count > 0:
        print('[JSON realign] reformatted %d file(s)\n' % count)


# 清理 scene_src 下所有 JSON，去除与默认配置重复的字段（保留 widget.type）
def scui_widget_parser_json_cleanup(src_path, def_name, defaults_map):
    removed_total = 0
    for dirpath, dirnames, filenames in os.walk(src_path):
        dirnames[:] = [d for d in dirnames if '#' not in os.path.join(dirpath, d)]
        for fname in filenames:
            full = os.path.join(dirpath, fname)
            if '#' in full or not fname.endswith('.json'):
                continue
            if fname == def_name:
                continue
            try:
                with open(full, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                if data.get('type') != 'scene':
                    continue
                file_removed = 0
                for widget in data.get('widget', []):
                    wtype = widget.get('widget.type', '')
                    if not wtype or wtype not in defaults_map:
                        continue
                    defs = defaults_map[wtype]
                    fields_to_remove = []
                    for field, value in list(widget.items()):
                        if field == 'widget.type':
                            continue
                        if field in defs and str(value) == str(defs[field]):
                            fields_to_remove.append(field)
                    for field in fields_to_remove:
                        del widget[field]
                        file_removed += 1
                if file_removed > 0:
                    with open(full, 'w', encoding='utf-8') as f:
                        f.write(scui_widget_parser_json_aligned_format(data, step=4))
                        f.write('\n')
                    print('  [JSON cleanup] %s: -%d field(s)' % (os.path.basename(full), file_removed))
                    removed_total += file_removed
            except Exception as e:
                print('  [JSON cleanup] SKIP %s: %s' % (os.path.basename(full), e))
    print('[JSON cleanup] total removed: %d fields\n' % removed_total)


# 清理 .c 文件中 scui_ui_maker() 已覆盖的冗余字段赋值
def scui_widget_parser_c_cleanup(c_root, defaults_map):
    total_removed = 0
    for dirpath, dirnames, filenames in os.walk(c_root):
        if 'scene_out' in dirpath:
            dirnames.clear()
            continue
        for fname in filenames:
            if not fname.endswith('.c'):
                continue
            filepath = os.path.join(dirpath, fname)
            try:
                with open(filepath, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
            except UnicodeDecodeError:
                try:
                    with open(filepath, 'r', encoding='gbk') as f:
                        lines = f.readlines()
                except Exception:
                    continue

            # 第一遍：收集默认配置已覆盖的构造器 (var_name → widget_type)
            # 兼容两种形式:
            #   旧: scui_ui_maker(&var, scui_widget_type_TYPE);
            #   新: scui_TYPE_maker_define(var);
            def_cfg_map = {}
            for line in lines:
                # 新范式宏: scui_TYPE_maker_define(var);
                m = re.match(r'^\s*scui_(\w+)_maker_define\((\w+)\)\s*;\s*$', line)
                if m:
                    var = m.group(2)
                    wtype = 'scui_widget_type_' + m.group(1)
                    if var not in def_cfg_map:
                        def_cfg_map[var] = wtype
                    continue
                # 旧范式调用: scui_ui_maker(&var, scui_widget_type_TYPE);
                m = re.match(
                    r'^\s*scui_ui_maker\(&(\w+),\s*(scui_widget_type_\w+)\);',
                    line
                )
                if m:
                    var = m.group(1)
                    wtype = m.group(2)
                    if var not in def_cfg_map:
                        def_cfg_map[var] = wtype
            if not def_cfg_map:
                continue

            # 第二遍：匹配 xxx_maker.field = value; 与默认值重复的行
            to_remove = set()
            for i, line in enumerate(lines):
                m = re.match(r'^\s*(\w+)\.(\S+)\s*=\s*(.+);\s*$', line)
                if not m:
                    continue
                var = m.group(1)
                field = m.group(2)
                value = m.group(3).strip()
                if var not in def_cfg_map:
                    continue
                defs = defaults_map.get(def_cfg_map[var], {})
                if field in defs:
                    def_val = str(defs[field])
                    if value == def_val or (def_val == '0' and value == '0'):
                        to_remove.add(i)

            if to_remove:
                for idx in sorted(to_remove, reverse=True):
                    del lines[idx]
                try:
                    with open(filepath, 'w', encoding='utf-8') as f:
                        f.writelines(lines)
                except Exception:
                    with open(filepath, 'w', encoding='gbk') as f:
                        f.writelines(lines)
                print('  [C cleanup] %s: -%d line(s)' % (os.path.basename(filepath), len(to_remove)))
                total_removed += len(to_remove)

    print('[C cleanup] total removed: %d lines\n' % total_removed)


# ============================================================
# 内联控件分析(原 scui_widget_analyze.py, 合并且不写 tmp)
# 从代码解析动态参数表: 控件名前缀/字段枚举/字段路径→值槽映射
# 供 scui_widget_parser_ready / 前端(GUI) 直接调用
# ============================================================
# 控件类型 → maker 类型映射(前缀从maker名推导)
SCUI_WIDGET_CLASS_MAKER = {
    'scui_widget_type_window':  'scui_window_maker_t',
    'scui_widget_type_custom':  'scui_custom_maker_t',
    'scui_widget_type_scroll':  'scui_scroll_maker_t',
    'scui_widget_type_string':  'scui_string_maker_t',
    'scui_widget_type_roller':  'scui_roller_maker_t',
    'scui_widget_type_xvedio':  'scui_xvedio_maker_t',
    'scui_widget_type_xwatch':  'scui_xwatch_maker_t',
    'scui_widget_type_object':  'scui_object_maker_t',
    'scui_widget_type_menial':  'scui_menial_maker_t',
}
# 字段枚举定义源 / 控件源码根(相对 scui/tools)
SCUI_WIDGET_JSON_FIELD_SOURCE = '../widget/widget/scui_widget.h'
SCUI_WIDGET_SOURCE_ROOT = '..'
# 继承域宏(typedef块内剥离)
SCUI_WIDGET_ANALYZE_EXTEND_MACROS = ('SCUI_EXTEND_FIELD_S', 'SCUI_EXTEND_FIELD_E')
# 成员声明正则: 类型 指针 名 [数组] [位域];
SCUI_WIDGET_ANALYZE_MEMBER_RE = re.compile(
    r'^\s*([A-Za-z_]\w*)\s+(\**)\s*([A-Za-z_]\w*)\s*(\[[^\]]*\])?\s*(?::\s*\d+)?\s*;?\s*$')
# 位域声明正则(判断成员是否为位域)
SCUI_WIDGET_ANALYZE_BITFIELD_RE = re.compile(r':\s*\d+\s*;?\s*$')
# 匿名嵌套块正则: struct { ... } [name];
SCUI_WIDGET_ANALYZE_ANON_RE = re.compile(r'^\s*(struct|union)\s*\{')


# 控件名前缀: 0号 widget 占位(基域), 其余从 maker 名推导
def scui_widget_analyze_prefixes():
    prefixes = ['widget']
    for maker in SCUI_WIDGET_CLASS_MAKER.values():
        prefix = maker
        if prefix.startswith('scui_'):
            prefix = prefix[5:]
        if prefix.endswith('_maker_t'):
            prefix = prefix[:-len('_maker_t')]
        if prefix not in prefixes:
            prefixes.append(prefix)
    return prefixes


# 解析字段枚举(scui_widget_json_field_t)
# 返回 [(scope, field_path)]: scope为'base'(widget基域)或控件前缀(如window)
def scui_widget_analyze_first_fields(prefixes, src=None):
    if src is None:
        src = os.path.normpath(os.path.join(os.path.dirname(__file__), SCUI_WIDGET_JSON_FIELD_SOURCE))
    with open(src, 'r', encoding='utf-8') as fp:
        text = fp.read()
    m = re.search(r'typedef enum \{(.*?)\} scui_widget_json_field_t;', text, re.S)
    if m is None:
        print('[analyze] field enum not found in %s' % os.path.basename(src))
        return []
    entries = re.findall(r'scui_widget_json_field_(\w+)\s*,', m.group(1))
    first = []
    for entry in entries:
        if entry == 'num':
            continue
        for p in prefixes:
            if entry == p:
                first.append(('base' if p == 'widget' else p, ''))
                break
            if entry.startswith(p + '_'):
                path = entry[len(p) + 1:].replace('_', '.')
                if p == 'widget':
                    first.append(('base', 'widget.' + path) if path else ('base', ''))
                else:
                    first.append((p, path))
                break
        else:
            print('[analyze] unknown prefix: %s' % entry)
    return first


# 去掉注释
def scui_widget_analyze_strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    text = re.sub(r'//[^\n]*', '', text)
    return text


def scui_widget_analyze_split_top_level(body):
    stmts = []
    depth = 0
    cur = ''
    for c in body:
        if c == '{':
            depth += 1
            cur += c
        elif c == '}':
            depth -= 1
            cur += c
        elif c == ';' and depth == 0:
            if cur.strip():
                stmts.append(cur)
            cur = ''
        else:
            cur += c
    if cur.strip():
        stmts.append(cur)
    return stmts


def scui_widget_analyze_parse_members(body):
    """解析结构体/联合体块体, 返回 [(ctype, path, is_array, is_ptr, is_bitfield)]"""
    members = []
    for macro in SCUI_WIDGET_ANALYZE_EXTEND_MACROS:
        body = body.replace(macro, '')
    for stmt in scui_widget_analyze_split_top_level(body):
        s = stmt.strip()
        if not s:
            continue
        m = SCUI_WIDGET_ANALYZE_ANON_RE.match(s)
        if m:
            open_i = s.find('{')
            depth = 1
            i = open_i + 1
            while i < len(s) and depth:
                if s[i] == '{':
                    depth += 1
                elif s[i] == '}':
                    depth -= 1
                i += 1
            inner = s[open_i + 1:i - 1]
            rest = s[i:].strip()
            nm = re.match(r'([A-Za-z_]\w*)\s*(\[[^\]]*\])?\s*;?', rest)
            cname = nm.group(1) if nm else ''
            for sub in scui_widget_analyze_parse_members(inner):
                if cname:
                    sub = (sub[0], cname + '.' + sub[1], sub[2], sub[3], sub[4])
                members.append(sub)
            continue
        m = SCUI_WIDGET_ANALYZE_MEMBER_RE.match(s)
        if m:
            ctype = m.group(1)
            is_ptr = bool(m.group(2))
            cname = m.group(3)
            is_array = bool(m.group(4))
            is_bitfield = bool(SCUI_WIDGET_ANALYZE_BITFIELD_RE.search(s))
            members.append((ctype, cname, is_array, is_ptr, is_bitfield))
    return members


def scui_widget_analyze_collect_types(root):
    """扫描所有.h, 收集 typedef struct/union { } name; 与 typedef X Y; 别名"""
    types = {}
    aliases = {}
    for dirpath, dirnames, filenames in os.walk(root):
        for fname in filenames:
            if not fname.endswith('.h'):
                continue
            full = os.path.join(dirpath, fname)
            try:
                with open(full, 'r', encoding='utf-8') as fp:
                    text = fp.read()
            except Exception:
                continue
            text = scui_widget_analyze_strip_comments(text)
            for m in re.finditer(r'typedef\s+(struct|union)\s*\w*\s*\{', text):
                open_i = text.find('{', m.start())
                depth = 1
                i = open_i + 1
                while i < len(text) and depth:
                    if text[i] == '{':
                        depth += 1
                    elif text[i] == '}':
                        depth -= 1
                    i += 1
                rest = text[i:]
                nm = re.match(r'\s*([A-Za-z_]\w*)\s*;', rest)
                if nm:
                    types[nm.group(1)] = (m.group(1), text[open_i + 1:i - 1])
            for m in re.finditer(r'typedef\s+([A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*;', text):
                if m.group(1) != m.group(2):
                    aliases[m.group(2)] = m.group(1)
    return types, aliases


def scui_widget_analyze_slot_of_type(ctype):
    if re.match(r'scui_coord_t$', ctype):
        return 'coord'
    if re.match(r'scui_multi_t$', ctype):
        return 'multi'
    if re.match(r'scui_event_cb_t$', ctype):
        return 'event'
    if re.match(r'scui_(color|color32|color_wt|color8888|color565|color8565)_t$', ctype):
        return 'color'
    if re.match(r'scui_sbitfd_t$', ctype):
        return 'sbitfd'
    return 'handle'


def scui_widget_analyze_expand_type(tname, prefix, out, types, aliases, seen, depth=0):
    if depth > 10 or tname in seen or tname not in types:
        return
    seen = seen | {tname}
    _, body = types[tname]
    for (ctype, cname, is_array, is_ptr, is_bitfield) in scui_widget_analyze_parse_members(body):
        path = (prefix + '.' + cname) if prefix else cname
        if is_bitfield:
            out[path] = 'sbitfd'
            continue
        if is_ptr:
            out[path] = 'handle'
            continue
        if is_array:
            out[path] = scui_widget_analyze_slot_of_type(ctype)
            continue
        if ctype in types:
            scui_widget_analyze_expand_type(ctype, path, out, types, aliases, seen, depth + 1)
        elif ctype in aliases:
            target = aliases[ctype]
            if target in types:
                scui_widget_analyze_expand_type(target, path, out, types, aliases, seen, depth + 1)
            else:
                out[path] = scui_widget_analyze_slot_of_type(ctype)
        else:
            out[path] = scui_widget_analyze_slot_of_type(ctype)


def scui_widget_analyze_path_slots(types, aliases):
    """递归展开全部maker结构, 按类型分离: {maker类型: {完整路径: 值槽}}"""
    result = {}
    for maker in SCUI_WIDGET_CLASS_MAKER.values():
        paths = {}
        scui_widget_analyze_expand_type(maker, '', paths, types, aliases, set())
        result[maker] = paths
    return result


# 纯内存分析(不写 tmp; base 为 tools 目录, 打包环境由装入方传入)
def scui_widget_analyze_result(base):
    prefixes = scui_widget_analyze_prefixes()
    src = os.path.normpath(os.path.join(base, SCUI_WIDGET_JSON_FIELD_SOURCE))
    first_fields = scui_widget_analyze_first_fields(prefixes, src)
    root = os.path.normpath(os.path.join(base, SCUI_WIDGET_SOURCE_ROOT))
    types, aliases = scui_widget_analyze_collect_types(root)
    path_slots = scui_widget_analyze_path_slots(types, aliases)
    return {
        'prefixes':     prefixes,
        'first_fields': first_fields,
        'path_slots':   path_slots,
        'class_maker':  SCUI_WIDGET_CLASS_MAKER,
    }


# 主流程
def scui_widget_parser():
    # 脚本启动: 先准备动态参数表(代码分析)
    if not scui_widget_parser_ready():
        print('cfg ready failed')
        return
    # 参数列表:文件根目录,输出目录,默认配置json[, cleanup]
    if len(sys.argv) < 4 or len(sys.argv) > 5:
        print('argv list not match, need: src_path dst_path default_json [cleanup]')
        return
    src_path = sys.argv[1]
    dst_path = sys.argv[2]
    def_path = sys.argv[3]
    do_cleanup = (len(sys.argv) == 5 and sys.argv[4] == 'cleanup')
    # 获得文件处理根路径
    if not os.path.exists(src_path):
        print('src path is not exist')
        return
    if not os.path.exists(dst_path):
        print('dst path is not exist')
        return
    if not os.path.exists(def_path):
        print('default config not found:')
        return
    # 解析默认配置json
    defaults_map = {}
    def_file = open(def_path, 'r', encoding='utf-8')
    def_dict = json.loads(def_file.read())
    def_file.close()
    if def_dict.get('type') == 'scui widget default config':
        for wdef in def_dict.get('widget', []):
            wclass = wdef.get('class', '')
            wdefault = wdef.get('default', {})
            if wclass:
                defaults_map[wclass] = wdefault
    # print('default config loaded:', list(defaults_map.keys()), '\n\n')

    # 步骤1：清理 JSON 源文件中与默认配置重复的字段（在读取之前）
    if do_cleanup:
        scui_widget_parser_json_cleanup(src_path, os.path.basename(def_path), defaults_map)
        scui_widget_parser_json_realign(src_path, os.path.basename(def_path))

    print('src path:', _rel_ui(src_path))
    print('dst path:', _rel_ui(dst_path))
    # 遍历整个文件夹,获取指定扩展名的文件
    file_path_list = []
    scui_widget_parser_collect(file_path_list, src_path, os.path.basename(def_path))
    # check:
    # for item in file_path_list:
    #     print(item)
    # 读取所有的json文件并将其打包到一起
    json_dict_list = []
    for file in file_path_list:
        try:
            json_file = open(file, 'r', encoding='utf-8')
            json_dict = json.loads(json_file.read())
            if json_dict['type'] == 'scene':
                json_dict_list.append(json_dict)
            json_file.close()
        except Exception as e:
            print(file)
            print(e)
            return
    scene_list = json_dict_list
    # print(json_dict_list)
    # 核查文件支持
    scui_widget_parser_h = open(os.path.join(dst_path, 'scui_widget_parser.h'), mode='w', encoding='utf-8')
    scui_widget_parser_c = open(os.path.join(dst_path, 'scui_widget_parser.c'), mode='w', encoding='utf-8')
    scui_widget_parser_list = [scui_widget_parser_h, scui_widget_parser_c]
    scui_widget_parser_scene_list(scene_list, scui_widget_parser_list, defaults_map)
    scui_widget_parser_h.close()
    scui_widget_parser_c.close()
    # 生成 maker 默认初始化 .c/.h
    scui_widget_maker_generate(dst_path, def_path, defaults_map)

    # 步骤最后：清理手写 .c 文件中 scui_ui_maker() 已覆盖的冗余字段
    if do_cleanup:
        c_root = os.path.normpath(os.path.join(src_path, os.pardir, os.pardir))
        scui_widget_parser_c_cleanup(c_root, defaults_map)


if __name__ == '__main__':
    try:
        scui_widget_parser()
    except Exception as e:
        print(e)
    print('scui widget parser finish')
    input('请按任意键退出...')
