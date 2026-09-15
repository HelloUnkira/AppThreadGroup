# 图片打包文件
import re
import os
import sys
import json
import copy


# 句柄表偏移:字库描述
scui_font_offset_name = 'SCUI_HANDLE_OFFSET_FONT'
scui_font_offset_value = '0x4000 - 1'
# ui(scui_ui_res) 基准: GUI 注入, 日志路径相对化显示
SCUI_UI_ROOT = None
# tools 基准: GUI 注入(读句柄偏移配置)
SCUI_TOOLS = None


# 句柄偏移: 前端"设置句柄"写入的 json 覆盖, 无则用头部默认
def _handle_offset(tag, name, value):
    try:
        cfg = os.path.join(SCUI_TOOLS or os.path.dirname(__file__), 'scui_pack_handle.json')
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


# 流式处理所有font文件
# 头文件保存目标数据描述表,源文件保存数据源,二进制文件保存源文件的流式版本
def scui_font_package_all(file_path_list, scui_font_package_list, scui_font_json):
    scui_font_package_h = scui_font_package_list[0]
    scui_font_package_c = scui_font_package_list[1]
    scui_font_package_bin = scui_font_package_list[2]
    scui_font_package_sub = scui_font_package_list[3]
    # 检查子路径是否存在,不存在则创建它
    os.makedirs(scui_font_package_sub, exist_ok=True)
    # 头文件添加前缀, 源文件添加前缀
    scui_font_package_h.write('#ifndef SCUI_FONT_PACKAGE_H\n')
    scui_font_package_h.write('#define SCUI_FONT_PACKAGE_H\n\n')
    scui_font_package_h.write('/* 本文件由 scui_pack_tools.exe 生成 */\n\n')
    scui_font_package_c.write('/* 本文件由 scui_pack_tools.exe 生成 */\n\n')
    scui_font_package_c.write('#include \"scui.h\"\n\n')
    # 头文件添加类型
    scui_font_lang = scui_font_json['lang']
    # print(scui_font_lang)
    scui_font_package_h.write('typedef enum {\n')
    for font_lang in scui_font_json['lang']:
        scui_font_package_h.write('\tscui_font_lang_%s,\n' % font_lang)
    scui_font_package_h.write('} scui_font_lang_t;\n\n')
    # offset
    pixel_bin_ofs = 0
    scui_font_num = 0
    # 填充数据表
    offset_name = scui_font_offset_name
    offset_value = scui_font_offset_value
    scui_font_num = 0
    scui_font_package_h.write('typedef enum {\n')
    scui_font_package_h.write('\t%s = %s,\n' % (offset_name, offset_value))
    for font_item in scui_font_json['info']:
        if font_item['lang'] not in scui_font_lang:
            print('unknown lang type: \n')
            print(font_item)
            continue
        # 对目标字库集合进行流式处理,提取数据内容
        for font_idx, font_name in enumerate(font_item['name']):
            scui_font_num += 1
            scui_font_ofs = hex(eval(offset_value) + scui_font_num)
            scui_font_tag = font_name.replace('.', '').replace("font_", "")
            scui_font_package_h.write('\tscui_font_%s, // %s\n' % (scui_font_tag, scui_font_ofs))
            scui_font_tag = font_name.replace('.', '_')
            # 直接原模原样的copy即可
            file = next(path for path in file_path_list if os.path.basename(path) == font_name)
            with open(file, mode='rb') as file_raw:
                pixel_stream = file_raw.read()
                pixel_bin_len = len(pixel_stream)
                scui_font_package_bin.write(pixel_stream)
                scui_font_byte = pixel_stream
                print('font:' + scui_font_tag)
            # 写入结构, 更新pixel_offset
            scui_font_struct = ''
            scui_font_struct += 'static const scui_font_t %s = {\n' % scui_font_tag
            scui_font_struct += '\t.font_name\t = \"%s\",\n' % font_name
            scui_font_struct += '\t.font_lang\t = scui_font_lang_%s,\n' % font_item['lang']
            scui_font_struct += '\t.font_size\t = %d,\n' % font_item['size'][font_idx]
            scui_font_struct += '\t.data_bin\t = %s,\n' % hex(pixel_bin_ofs)
            scui_font_struct += '\t.size_bin\t = %s,\n' % hex(pixel_bin_len)
            # 尝试提取扩充字段(如果有的话)
            try:
                base_line_ext = font_item['ext'][font_idx]['base_line_ext']
                line_height_ext = font_item['ext'][font_idx]['line_height_ext']
                scui_font_struct += '\t\n'
                scui_font_struct += '\t.base_line_ext = %s,\n' % hex(base_line_ext)
                scui_font_struct += '\t.line_height_ext = %s,\n' % hex(line_height_ext)
                scui_font_struct += '\t\n'
            except Exception as e:
                pass
            scui_font_struct += '};\n\n'
            scui_font_package_c.write(scui_font_struct)
            # 我们生成一个子记录,用于外界解析时使用(内部只使用全部bin)
            scui_font_package_sub_file = os.path.join(scui_font_package_sub, scui_font_tag)
            with open(scui_font_package_sub_file, mode='w', encoding='utf-8') as file:
                file.write('\nconst uint8_t scui_font_array[] = {\n\t')
                # 迭代字节数据流, 将其转为hex字符
                scui_font_width = 100
                # 迭代字节数据流, 将其转为hex字符
                scui_font_byte_cnt = 0
                for byte in scui_font_byte:
                    scui_font_byte_cnt += 1
                    file.write('0x{:02x}, '.format(byte))
                    if scui_font_byte_cnt % scui_font_width == 0:
                        file.write('\n\t')
                file.write('\n\n};\n\n' + scui_font_struct + '\n')
            # 信息记录
            pixel_bin_ofs += pixel_bin_len
    scui_font_package_h.write('} scui_font_type_t;\n')
    scui_font_package_h.write('\nextern const void * const scui_font_package_table[%d];\n\n' % scui_font_num)
    scui_font_package_h.write('/*@brief 字库语言字号匹配\n')
    scui_font_package_h.write(' *@param lang 字库语言类型\n')
    scui_font_package_h.write(' *@param size 字库字号\n')
    scui_font_package_h.write(' *@retval 字库句柄\n */\n')
    scui_font_package_h.write('scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size);\n')
    scui_font_package_h.write('\n#endif\n')
    # 填充数据表
    scui_font_package_c.write('const void * const scui_font_package_table[%d] = {\n' % scui_font_num)
    for font_item in scui_font_json['info']:
        if font_item['lang'] not in scui_font_lang:
            continue
        for font_name in font_item['name']:
            scui_font_tag = font_name.replace('.', '_')
            scui_font_package_c.write('\t(void *)&%s,\n' % scui_font_tag)
    scui_font_package_c.write('};\n\n')
    # 填充函数定义或者声明
    scui_font_package_c.write('/*@brief 字库语言字号匹配\n')
    scui_font_package_c.write(' *@param lang 字库语言类型\n')
    scui_font_package_c.write(' *@param size 字库字号\n')
    scui_font_package_c.write(' *@retval 字库句柄\n */\n')
    scui_font_package_c.write('scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size)\n{\n')
    for font_item in scui_font_json['info']:
        if font_item['lang'] not in scui_font_lang:
            continue
        if font_item['lang'] == 'ascii' or font_item['lang'] == 'multi' or font_item['lang'] == 'symbol':
            continue
        # 对目标字库集合进行流式处理,提取数据内容
        for font_idx, font_name in enumerate(font_item['name']):
            if_check = '\tif (size == %3d && lang == scui_font_lang_%s)\n'
            scui_font_package_c.write(if_check % (font_item['size'][font_idx], font_item['lang']))
            scui_font_tag = font_name.replace('.', '').replace("font_", "")
            scui_font_package_c.write('\t\treturn scui_font_%s;\n' % scui_font_tag)
    scui_font_package_c.write('\t\n\treturn SCUI_HANDLE_INVALID;\n}\n')


# 遍历整个文件夹,提取目标文件
def scui_font_collect(file_path_list, file_ext_list, path):
    if os.path.isfile(path):
        # 获取指定扩展名的文件
        for ext in file_ext_list:
            if path.endswith(ext):
                # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
                if not re.findall(r'#', path):
                    file_path_list.append(path)
                break
    if os.path.isdir(path):
        for item in os.listdir(path):
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', os.path.join(path, item)):
                scui_font_collect(file_path_list, file_ext_list, os.path.join(path, item))


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


# 主流程
def scui_font_package():
    # 句柄偏移: 前端"设置句柄"json 覆盖头部默认
    global scui_font_offset_name, scui_font_offset_value
    scui_font_offset_name, scui_font_offset_value = \
        _handle_offset('font', scui_font_offset_name, scui_font_offset_value)
    # 参数列表:src相对路径 dst相对路径 项目名称
    if len(sys.argv) != 3:
        print('argv list not match')
        return
    src_path = sys.argv[1]
    dst_path = sys.argv[2]
    # 获得文件处理src相对路径
    if not os.path.exists(src_path):
        print('src path is not exist')
        return
    # 获得文件处理dst相对路径
    if not os.path.exists(dst_path):
        print('dst path is not exist')
        return
    print('src path:', _rel_ui(src_path))
    print('dst path:', _rel_ui(dst_path))
    # json转Python字符串并转标准字典
    parser_path = os.path.join(dst_path, 'scui_res_font.json')
    json_file = open(parser_path, 'r', encoding='utf-8')
    json_dict = json.loads(json_file.read())
    json_file.close()
    if json_dict['type'] != r'scui font package':
        print('font package unknown')
        return
    scui_font_json = json_dict
    # print(scui_font_json)
    # 遍历整个文件夹,获取指定扩展名的文件
    file_ext_list = ['.bin', '.ttf']
    file_path_list = []
    scui_font_collect(file_path_list, file_ext_list, src_path)
    # check:
    # for item in file_path_list:
    #     print(item)
    # 核查文件支持
    scui_font_package_h = open(os.path.join(dst_path, 'scui_res_font.h'), mode='w', encoding='utf-8')
    scui_font_package_c = open(os.path.join(dst_path, 'scui_res_font.c'), mode='w', encoding='utf-8')
    scui_font_package_bin = open(os.path.join(dst_path, 'scui_res_font.bin'), mode='wb')
    scui_font_package_sub = os.path.join(dst_path, 'font_array')   # 子文件单独生成到子路径
    scui_font_package_list = [
        scui_font_package_h,
        scui_font_package_c,
        scui_font_package_bin,
        scui_font_package_sub,
    ]
    scui_font_package_all(file_path_list, scui_font_package_list, scui_font_json)
    scui_font_package_h.close()
    scui_font_package_c.close()
    scui_font_package_bin.close()


# ttf 字库合并: 把候选 ttf 指定 range 内的字形, 合并(覆盖)到基版 ttf, 输出目标 ttf
#   调用: scui_font_merge(base, cand, out, range_text, log, skip_blank)
#     skip_blank=True(默认): 候选空字形不覆盖基版同码位的实心字形(避免把已有轮廓抹成空白)
#   前端弹窗与命令行 font_merge 均走这里
# range 处理: 多个以 空白/逗号/分号(含全角) 分隔; 单点或区间(闭合, 含两端)
#   例: "-r 0x20-0x7f" / "0x20-0x7f" / "U+0600-U+06FF" / "32-127" / "0x200C"
SCUI_FONT_RANGE_SPLIT = re.compile(r'[\s,;，、；]+')
# range 前缀标记(与命令行保持兼容, 解析时丢弃)
SCUI_FONT_RANGE_FLAG  = ('-r', '--range', '-R', '--symbols', '-s', '--unicode')
# 只认这些 cmap 子表格式(4: BMP 分段; 12/13: 全 Unicode)
SCUI_FONT_CMAP_FMT    = (4, 12, 13)
# 非 Unicode 子表只补齐这些区(工具链把阿拉伯/呈现形式写在这里);
#   其余条目一律忽略 —— (1,0)/(1,6) 是 Mac Roman 字节编码(0x80 = Ä),
#   (1,25) 里还混着 GB/Big5 字节对(0xA1A1 之类), 照抄会把字节对误当 Unicode 码点,
#   并让同一个字形被大量伪码点共用(破坏按码点就地覆盖)
SCUI_FONT_FILL_RANGES = ((0x0600, 0x06FF), (0x0750, 0x077F), (0x08A0, 0x08FF),
                         (0x200C, 0x200F), (0x202A, 0x202E), (0x2066, 0x2069),
                         (0xFB50, 0xFDFF), (0xFE70, 0xFEFF))


# 单个 token 数值解析(带 0x/U+ 前缀按16进制, 否则十进制)
def _range_num(s, tok):
    t = s.strip()
    low = t.lower()
    try:
        if low.startswith('0x'):
            return int(t[2:], 16)
        if low.startswith('u+'):
            return int(t[2:], 16)
        return int(t, 10)
    except ValueError:
        raise ValueError('range 数值无法解析: %s' % tok)


# 单个 token -> (lo, hi)
def _range_one(tok):
    parts = tok.split('-', 1)
    lo = _range_num(parts[0], tok)
    hi = lo
    if len(parts) == 2 and parts[1].strip():
        hi = _range_num(parts[1], tok)
    if lo > hi:
        raise ValueError('range 区间倒置: %s' % tok)
    if lo < 0 or hi > 0x10FFFF:
        raise ValueError('range 超出 Unicode 范围: %s' % tok)
    return lo, hi


# range 文本 -> [(lo, hi), ...](升序; 已合并重叠与相邻区间)
def scui_font_merge_parse_range(text):
    out = []
    for tok in SCUI_FONT_RANGE_SPLIT.split(text or ''):
        t = tok.strip()
        if not t or t in SCUI_FONT_RANGE_FLAG:
            continue
        for flag in SCUI_FONT_RANGE_FLAG:
            if t.startswith(flag) and len(t) > len(flag):
                t = t[len(flag):]        # 兼容 "-r0x20-0x7f" 粘连写法
                break
        if not t:
            continue
        out.append(_range_one(t))
    if not out:
        return []
    out.sort()
    merge = [list(out[0])]
    for lo, hi in out[1:]:
        if lo <= merge[-1][1] + 1:
            merge[-1][1] = max(merge[-1][1], hi)
        else:
            merge.append([lo, hi])
    return [(lo, hi) for lo, hi in merge]


# 区间列表 -> 码点总数
def scui_font_merge_range_codepoints(ranges):
    return sum(hi - lo + 1 for lo, hi in ranges)


# 非 Unicode 子表条目是否可补齐(阿拉伯/呈现形式/整形控制符区)
def _font_fill_ok(cp):
    for lo, hi in SCUI_FONT_FILL_RANGES:
        if lo <= cp <= hi:
            return True
    return False


# 码点表: 优先 Unicode 子表, 再由非 Unicode 子表补齐(仅 SCUI_FONT_FILL_RANGES 白名单)
#   工具链(lv_font_conv)会把 AP 呈现形式区写入 (1,25) 非 Unicode 子表, 只取 getBestCmap() 会漏掉整段阿拉伯字形
# 返回 (码点->字形名, 是否用了非 Unicode 子表补齐)
def scui_font_cmap_pick(font):
    uni = {}
    other = {}
    for sub in font['cmap'].tables:
        try:
            if sub.format not in SCUI_FONT_CMAP_FMT:
                continue
            if sub.isUnicode():
                uni.update(sub.cmap)
            else:
                for cp, gn in sub.cmap.items():
                    if _font_fill_ok(cp):
                        other[cp] = gn
        except Exception:
            continue
    if not uni:
        return dict(other), bool(other)
    fill = 0
    for cp, gn in other.items():
        if cp not in uni:
            uni[cp] = gn
            fill += 1
    return uni, bool(fill)


# 字库摘要预览的检查区(弹窗选择后显示 upem / 字形数 / 关键区覆盖)
SCUI_FONT_INFO_RANGES = ((0x20, 0x7F, 'ascii'), (0x0600, 0x06FF, 'ar-basic'),
                         (0xFB50, 0xFDFF, 'form-a'), (0xFE70, 0xFEFF, 'form-b'),
                         (0x2E80, 0x9FFF, 'cjk'))
SCUI_FONT_INFO_MARKS  = ('', 'K', 'M', 'G')


# 字节数 -> 可读体积
def _fmt_size(n):
    v, i = float(n), 0
    while v >= 1024 and i < 3:
        v /= 1024.0
        i += 1
    return '%.1f%s' % (v, SCUI_FONT_INFO_MARKS[i])


# 字库摘要 -> dict(upem, glyphs, size, glyf, cmap, cmap_fill, cover)
def scui_font_merge_font_info(path):
    try:
        from fontTools.ttLib import TTFont
    except ImportError:
        raise RuntimeError('缺模块 fontTools(请 pip install fonttools)')
    if not os.path.isfile(path):
        raise ValueError('字库不存在: %s' % path)
    ft = TTFont(path)
    try:
        cmap, fill = scui_font_cmap_pick(ft)
        cover = {}
        for lo, hi, tag in SCUI_FONT_INFO_RANGES:
            cover[tag] = (sum(1 for cp in range(lo, hi + 1) if cp in cmap), hi - lo + 1)
        return {
            'upem':      ft['head'].unitsPerEm,
            'glyphs':    ft['maxp'].numGlyphs,
            'size':      os.path.getsize(path),
            'glyf':      'glyf' in ft,
            'cmap':      len(cmap),
            'cmap_fill': fill,
            'cover':     cover,
        }
    finally:
        ft.close()


# 字库摘要 -> 单行文本(弹窗显示)
def scui_font_merge_info_text(info):
    if not info:
        return '-'
    cov = ' '.join('%s:%d/%d' % (t, info['cover'][t][0], info['cover'][t][1])
                   for _, _, t in SCUI_FONT_INFO_RANGES)
    return ('upem %d | 字形 %d | %s | cmap %d%s | %s'
            % (info['upem'], info['glyphs'], _fmt_size(info['size']), info['cmap'],
               '(含非Unicode补齐)' if info['cmap_fill'] else '', cov))


# 空字形判定(无轮廓且 bbox 全 0): 用于"不把基版实心字形覆盖成空白"
def _merge_glyph_blank(g, glyf):
    if hasattr(g, 'data'):
        g.expand(glyf)
    if getattr(g, 'numberOfContours', 0) != 0:
        return False
    if not hasattr(g, 'xMin'):
        try:
            g.recalcBounds(glyf)
        except Exception:
            pass
    return (getattr(g, 'xMin', 0), getattr(g, 'yMin', 0),
            getattr(g, 'xMax', 0), getattr(g, 'yMax', 0)) == (0, 0, 0, 0)


# 能否整块换掉基版字形名 bn: 只有引用它的码点全在本次目标里才行
#   (否则会连带改到 range 外的码点, 例如基版把 U+0020 与 U+00A0 共用 space)
def _merge_name_free(bn, base_name_cps, targets):
    return all(cp in targets for cp in base_name_cps.get(bn, ()))


# 确保字形已解压(懒加载下 glyphs[] 只有原始数据, 无 coordinates/components)
def _merge_glyph_expand(g, glyf):
    if hasattr(g, 'data'):
        g.expand(glyf)
    if getattr(g, 'numberOfContours', 0) == -1:
        return [comp.glyphName for comp in g.components]
    return []


# 接管候选字形: 先确保解压, 再按 factor 缩放(简单字形坐标 / 复合字形组件偏移;
#   空字形(numberOfContours==0)无轮廓, 跳过缩放)
def _merge_scale_glyph(g, glyf, factor):
    if hasattr(g, 'data'):
        g.expand(glyf)
    if factor == 1.0:
        return
    if getattr(g, 'numberOfContours', 0) == -1:
        for comp in g.components:
            comp.x = int(round(comp.x * factor))
            comp.y = int(round(comp.y * factor))
        return
    if not hasattr(g, 'coordinates'):
        return                       # 空字形: 无轮廓, 无需缩放
    from fontTools.ttLib.tables._g_l_y_f import GlyphCoordinates
    g.coordinates = GlyphCoordinates(
        [(int(round(x * factor)), int(round(y * factor))) for x, y in g.coordinates])


# 候选 ttf 的 range 字形 -> 覆盖到基版 ttf -> 输出目标 ttf; 返回统计字典
#   1) 目标码点: 候选 range 内命中; skip_blank 时跳过"会把基版实心字形抹成空白"的候选空字形
#   2) 落地名: 按码点优先复用基版既有字形名(不产生重复字形/孤儿); 基版无名或该名被 range 外码点共用则新建
#   3) 组件闭包: 复合字形的组件(递归)一并带入, 一律新建, 落地后把组件名改写到基版侧名
#   4) 同步: 字形数据(含单位缩放)/字宽/字形顺序/maxp/head/cmap
def scui_font_merge(base_path, cand_path, out_path, range_text, log=None, skip_blank=True):
    """候选 ttf 的 range 字形 -> 覆盖到基版 ttf -> 输出目标 ttf; 返回统计字典"""
    log = log or (lambda s: None)
    ranges = scui_font_merge_parse_range(range_text)
    if not ranges:
        raise ValueError('range 为空(示例: -r 0x20-0x7f -r 0x0600-0x06FF)')
    if not os.path.isfile(base_path):
        raise ValueError('基版目标不存在: %s' % base_path)
    if not os.path.isfile(cand_path):
        raise ValueError('候选目标不存在: %s' % cand_path)
    try:
        from fontTools.ttLib import TTFont
    except ImportError:
        raise RuntimeError('缺模块 fontTools(请 pip install fonttools)')

    log('range: %d 段, 合计 %d 个码点' % (len(ranges), scui_font_merge_range_codepoints(ranges)))
    # 懒加载即可: 只在"实际接管的字形"上按需解压(大字体加载/保存更快)
    base = TTFont(base_path)
    cand = TTFont(cand_path)
    try:
        if 'glyf' not in base or 'glyf' not in cand:
            raise ValueError('仅支持 TrueType(glyf)字库; CFF/OTF 请先转曲再合并')

        # 单位缩放: 基版与候选 unitsPerEm 不同则按基版缩放候选字形
        upem_b = base['head'].unitsPerEm
        upem_c = cand['head'].unitsPerEm
        factor = 1.0
        if upem_b != upem_c:
            factor = float(upem_b) / float(upem_c)
            log('unitsPerEm 不一致(基版 %d / 候选 %d): 候选字形按 %.6f 缩放'
                % (upem_b, upem_c, factor))

        base_cmap, _ = scui_font_cmap_pick(base)
        cand_cmap, cand_fill = scui_font_cmap_pick(cand)
        if cand_fill:
            log('候选码点表含非 Unicode 子表补齐(工具链 AP 区惯例)')
        base_glyf = base['glyf'].glyphs
        base_tab  = base['glyf']
        cand_glyf = cand['glyf'].glyphs
        cand_tab  = cand['glyf']
        base_hmtx = base['hmtx'].metrics
        cand_hmtx = cand['hmtx'].metrics
        base_order = list(base.getGlyphOrder())
        base_set   = set(base_order)
        taken      = set(base_order)
        # 基版字形名 -> 引用它的码点(判断能否整块换掉)
        base_name_cps = {}
        for cp, gn in base_cmap.items():
            base_name_cps.setdefault(gn, []).append(cp)

        # 1) 目标码点 -> 候选字形名(可选: 跳过会把基版实心字形抹成空白的空字形)
        targets = {}
        blank_skip = []
        for lo, hi in ranges:
            for cp in range(lo, hi + 1):
                gn = cand_cmap.get(cp)
                if gn is None or gn not in cand_glyf:
                    continue
                if skip_blank:
                    bn = base_cmap.get(cp)
                    if bn is not None and bn in base_glyf                             and _merge_glyph_blank(cand_glyf[gn], cand_tab)                             and not _merge_glyph_blank(base_glyf[bn], base_tab):
                        blank_skip.append(cp)
                        continue
                targets[cp] = gn
        if not targets:
            raise ValueError('候选目标在给定 range 内无任何字形')
        log('候选命中: %d 个码点%s'
            % (len(targets),
               (' ; 跳过候选空字形 %d 个(基版同码位原有轮廓)' % len(blank_skip)) if blank_skip else ''))

        # 2) 落地名: 按码点优先复用基版既有字形名(不新增重复字形)
        cp_out  = {}       # 码点 -> 基版侧字形名
        out_src = {}       # 基版侧字形名 -> 候选字形名
        shared  = 0        # 基版同形别名被 range 外码点引用 -> 另建副本
        for cp in sorted(targets):
            cn = targets[cp]
            bn = base_cmap.get(cp)
            if bn is not None and bn not in out_src and bn in base_glyf:
                if _merge_name_free(bn, base_name_cps, targets):
                    out_src[bn] = cn
                    cp_out[cp] = bn
                    continue
                shared += 1
            name = cn
            k = 1
            while name in taken:
                name = '%s.m%d' % (cn, k)
                k += 1
            taken.add(name)
            out_src[name] = cn
            cp_out[cp] = name
        if shared:
            log('基版 %d 个字形被 range 外码点共用(同形别名): 另建字形以保护原码点' % shared)

        # 3) 候选字形名 -> 首次落地名(P2: 一个候选字形被多个基版名分用时深拷贝)
        cand_map = {}
        cand_out = {}
        for on, cn in out_src.items():
            if cn not in cand_map:
                cand_map[cn] = on
            cand_out.setdefault(cn, []).append(on)
        replaced = [on for on in out_src if on in base_set]
        added    = [on for on in out_src if on not in base_set]
        missing  = []

        # 4) 组件闭包(候选字形名清单在遍历中增长)
        queue = list(cand_out.keys())
        while queue:
            cn = queue.pop(0)
            g = cand_glyf.get(cn)
            if g is None:
                missing.append(cn)
                continue
            for comp in _merge_glyph_expand(g, cand_tab):
                if comp in cand_map:
                    continue
                if comp not in cand_glyf:
                    missing.append(comp)
                    continue
                name = comp
                k = 1
                while name in taken:
                    name = '%s.m%d' % (comp, k)
                    k += 1
                taken.add(name)
                cand_map[comp] = name
                cand_out[comp] = [name]
                added.append(name)
                queue.append(comp)

        # 5) 落地: 字形数据 + 单位缩放 + 字宽(首个落地名用原字形, 其余深拷贝)
        landed = []
        for cn, outs in cand_out.items():
            g = cand_glyf.get(cn)
            if g is None:
                continue
            _merge_scale_glyph(g, cand_tab, factor)      # 先缩放源字形(副本随之)
            adv, lsb = cand_hmtx.get(cn, (0, 0))
            metrics = (int(round(adv * factor)), int(round(lsb * factor)))
            for i, on in enumerate(outs):
                base_glyf[on] = g if i == 0 else copy.deepcopy(g)
                base_hmtx[on] = metrics
                landed.append(on)
        # 6) 组件改名(候选字形名 -> 基版侧名)
        for on in landed:
            g = base_glyf.get(on)
            if getattr(g, 'numberOfContours', 0) == -1:
                for comp in g.components:
                    comp.glyphName = cand_map.get(comp.glyphName, comp.glyphName)

        # 7) 字形顺序 / maxp / head(loca 统一长格式: 合并后偏移可能超短格式上限)
        new_order = base_order + added
        base.setGlyphOrder(new_order)
        try:
            base['glyf'].glyphOrder = new_order
        except Exception:
            pass
        base['maxp'].numGlyphs = len(new_order)
        base['head'].indexToLocFormat = 1

        # 8) cmap: 逐码点写 Unicode 子表(若无 Unicode 子表则退回其它 format 4/12/13 子表)
        subs = [s for s in base['cmap'].tables
                if getattr(s, 'format', 0) in SCUI_FONT_CMAP_FMT and s.isUnicode()]
        if not subs:
            subs = [s for s in base['cmap'].tables
                    if getattr(s, 'format', 0) in SCUI_FONT_CMAP_FMT]
            log('基版无 Unicode 子表, 退回写入其它 cmap 子表')
        for sub in subs:
            for cp, on in cp_out.items():
                if sub.format == 4 and cp > 0xFFFF:
                    continue
                sub.cmap[cp] = on

        # 8.5) 度量重算: 行高必须覆盖全部字形纵跨
        #      (候选字形可能超出基版 hhea 范围, 否则运行时
        #      line_height < 字形组合纵跨 -> scui_string_args.c:151 断言死机;
        #      留 ±10% 取整余量: stb ifloor/iceil + base_line 取整外扩约 +3% 即够,
        #      10% 保证所有字号不超行高(且符合“真实 line_height”语义))
        g_max_y = -0x7FFF
        g_min_y = 0x7FFF
        base['glyf'].ensureDecompiled()
        # PUA 区字形(图标)独立显示(symbol 控件),不进 string 文本,
        # 不参与行高(否则图标纵跨撑大 hhea -> 全体文字变小);
        # 用 any(任一码点在 PUA): 图标字形被非 PUA 码点共享时也跳过
        pua_low = set(range(0xE000, 0xF900))
        cmap_rev = {}
        for s in base['cmap'].tables:
            if getattr(s, 'format', 0) in SCUI_FONT_CMAP_FMT:
                for cp, on in s.cmap.items():
                    cmap_rev.setdefault(on, []).append(cp)
        # 先 recalcBounds 全部字形: 头字段 = 坐标真实范围
        # (缩放/未缩放都正确), 之后直接读头字段,
        # 不再按 factor 折算(原版字形不受本步缩放)
        for gn0 in base.getGlyphOrder():
            g0 = base_glyf.get(gn0)
            if g0 is None or g0.numberOfContours == 0:
                continue
            try:
                g0.recalcBounds(base['glyf'])
            except Exception:
                pass
        for gn in base.getGlyphOrder():
            g = base_glyf.get(gn)
            if g is None or g.numberOfContours == 0:
                continue
            cps = cmap_rev.get(gn, [])
            if cps and any(cp in pua_low for cp in cps):
                continue
            if g.yMin < g_min_y:
                g_min_y = g.yMin
            if g.yMax > g_max_y:
                g_max_y = g.yMax
        if g_max_y != -0x7FFF and g_min_y != 0x7FFF:
            ascent  = int(round(g_max_y * 1.10))
            descent = int(round(g_min_y * 1.10))
            hhea = base['hhea']
            # 直接覆盖式设置: 遍历已覆盖全部非 PUA 字形,
            # max/min 保留旧值会被已写回的污染 hhea 阻挡
            hhea.ascent  = ascent
            hhea.descent = descent
            os2 = base.get('OS/2')
            if os2 is not None:
                os2.sTypoAscender  = hhea.ascent
                os2.sTypoDescender = hhea.descent
                os2.usWinAscent    = max(hhea.ascent, 0)
                os2.usWinDescent   = max(-hhea.descent, 0)
            head = base['head']
            head.yMin = min(head.yMin, g_min_y)
            head.yMax = max(head.yMax, g_max_y)
            log('度量重算: 字形纵跨 %d..%d -> hhea 行高 %d (ascent %d / descent %d)'
                % (g_min_y, g_max_y, hhea.ascent - hhea.descent, hhea.ascent, hhea.descent))

        # 9) 输出(同名即覆盖)
        if os.path.exists(out_path) and os.path.normcase(out_path) != os.path.normcase(base_path):
            log('输出目标已存在, 覆盖: %s' % os.path.basename(out_path))
        out_dir = os.path.dirname(os.path.abspath(out_path))
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        base.save(out_path)
    finally:
        base.close()
        cand.close()

    # 校验: 重新加载输出, 复核目标码点覆盖
    chk = TTFont(out_path)
    try:
        chk_cmap, _ = scui_font_cmap_pick(chk)
        hit = sum(1 for cp in targets if cp in chk_cmap)
        n_glyph = chk['maxp'].numGlyphs
    finally:
        chk.close()
    stat = {
        'range_seg':     len(ranges),
        'codepoint':     len(targets),
        'hit':           hit,
        'blank_skip':    len(blank_skip),
        'glyph_add':     len(added),
        'glyph_replace': len(replaced),
        'glyph_missing': len(missing),
        'glyph_total':   n_glyph,
        'upem':          upem_b,
        'scale':         factor,
        'out_size':      os.path.getsize(out_path),
    }
    log('码点覆盖: %d/%d ; 字形 新增%d 就地覆盖%d 缺失%d ; 总字形 %d'
        % (stat['hit'], stat['codepoint'], stat['glyph_add'],
           stat['glyph_replace'], stat['glyph_missing'], stat['glyph_total']))
    log('输出: %s (%d 字节)' % (os.path.basename(out_path), stat['out_size']))
    return stat


if __name__ == '__main__':
    try:
        scui_font_package()
    except Exception as e:
        print(e)
    print('scui font parser finish')
    input('请按任意键退出...')
