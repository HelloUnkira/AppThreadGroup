# 控件资源打包
import re
import os
import sys
import json


# 流式处理所有widget文件
def scui_widget_parser_scene_list(scene_list, scui_widget_parser_list, scui_widget_parser_table, defaults_map):
    scui_widget_parser_h = scui_widget_parser_list[0]
    scui_widget_parser_c = scui_widget_parser_list[1]
    # 头文件添加前缀, 源文件添加前缀
    scui_widget_parser_h.write('#ifndef SCUI_WIDGET_PARSER_H\n')
    scui_widget_parser_h.write('#define SCUI_WIDGET_PARSER_H\n\n')
    scui_widget_parser_h.write('/*一个通过scui_widget_parser脚本生成的widget表\n */\n\n')
    scui_widget_parser_c.write('/*一个通过scui_widget_parser脚本生成的widget表\n */\n\n')
    scui_widget_parser_c.write('#include \"scui.h\"\n\n')
    # 填充数据表
    offset_name = scui_widget_parser_table['offset_name']
    offset_value = scui_widget_parser_table['offset_value']
    scui_widget_parser_h.write('typedef enum {\n')
    scui_widget_parser_h.write('\t%s = %s,\n' % (offset_name, offset_value))
    scui_widget_parser_handle_num = 0
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_widget_parser_handle_num += 1
                scui_widget_parser_handle_ofs = eval(offset_value) + scui_widget_parser_handle_num
                scui_widget_parser_h.write('\t%s, // %s, %s\n' % (widget['widget.myself'], scui_widget_parser_handle_ofs, hex(scui_widget_parser_handle_ofs)))
            except Exception as e:
                print(widget)
                print(e)
                return
    scui_widget_parser_h.write('} scui_widget_parser_handle_t;\n\n')
    scui_widget_parser_h.write('extern const void * const scui_widget_parser_table[%s];\n\n' % scui_widget_parser_handle_num)
    scui_widget_parser_h.write('#endif\n')
    # 填充函数定义或者声明
    scui_widget_parser_c.write('#if defined(SCUI_WIDGET_PARSER_EVENT_CB_EMPTY) && SCUI_WIDGET_PARSER_EVENT_CB_EMPTY == 1\n')
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_event_cb = 'static void %s(scui_event_t *event)'
                scui_widget_parser_c.write('%s\n{\n}\n' % scui_event_cb % widget['widget.event_cb'])
            except Exception as e:
                pass
    scui_widget_parser_c.write('#else\n')
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_event_cb = 'extern void %s(scui_event_t *event);'
                scui_widget_parser_c.write('%s\n' % scui_event_cb % widget['widget.event_cb'])
            except Exception as e:
                pass
    scui_widget_parser_c.write('#endif\n\n')
    # 对目标控件集合进行流式处理,提取数据内容
    for scene in scene_list:
        for widget in scene['widget']:
            print(widget['widget.myself'])
            # 写入结构
            scui_widget_tag = 'scui_widget_' + widget['widget.myself']
            scui_widget_type_unknown = True
            for widget_maker in scui_widget_parser_table['widget']:
                if widget['widget.type'] == widget_maker['class']:
                    scui_widget_parser_c.write('static const %s %s = {\n' % (widget_maker['maker'], scui_widget_tag))
                    scui_widget_type_unknown = False
                    break
            # 未知类型
            if scui_widget_type_unknown:
                print('\nwidget type unknown\n')
                print(widget)
                continue
            # 统计本控件有多少布局孩子
            scui_widget_field_child_num = 0
            for field in widget:
                if field == 'widget.child_num':
                    scui_widget_field_child_num = eval(widget[field])
                    break
            # 统计本控件有多少布局孩子
            for target in scene['widget']:
                try:
                    if widget['widget.myself'] == target['widget.parent']:
                        scui_widget_field_child_num += 1
                except Exception as e:
                    pass
            # 填充默认数据目标（使用 C 指定初始化器的后覆盖前机制）
            scui_widget_parser_c.write('\t/* 默认配置 */\n')
            wtype = widget['widget.type']
            if wtype in defaults_map:
                for def_field, def_value in defaults_map[wtype].items():
                    scui_widget_parser_c.write('\t.%-30s = %s,\n' % (def_field, def_value))
            scui_widget_parser_c.write('\n\t/* 自定义配置 */\n')
            # 填充指定数据目标（跳过与默认配置重复的字段减少冗余）
            if wtype in defaults_map:
                defs = defaults_map[wtype]
            else:
                defs = {}
            for field in widget:
                if field == 'annotation':
                    continue
                if field == 'widget.child_num':
                    continue
                # 跳过与默认配置重复的字段（.c 输出中包括 widget.type）
                if field in defs and str(widget[field]) == str(defs[field]):
                    continue
                scui_widget_parser_c.write('\t.%-30s = %s,\n' % (field, widget[field]))
            if scui_widget_field_child_num != 0:
                scui_widget_parser_c.write('\t.%-30s = %s,\n' % ("widget.child_num", str(scui_widget_field_child_num)))
            scui_widget_parser_c.write('};\n\n')
    # 填充数据表
    scui_widget_parser_c.write('const void * const scui_widget_parser_table[%s] = {\n' % scui_widget_parser_handle_num)
    for scene in scene_list:
        for widget in scene['widget']:
            scui_widget_tag = 'scui_widget_' + widget['widget.myself']
            scui_widget_parser_c.write('\t(void *)&%s,\n' % scui_widget_tag)
    scui_widget_parser_c.write('};\n')
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
def scui_widget_maker_generate(dst_path, def_path, defaults_map, parser_table):
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
    scui_widget_maker_h.write('void scui_widget_maker_def_cfg(void *maker, scui_widget_type_t type);\n\n')
    scui_widget_maker_h.write('#endif\n')
    scui_widget_maker_h.close()

    # 建立 class → maker 类型映射
    class_to_maker = {}
    for entry in parser_table.get('widget', []):
        class_to_maker[entry['class']] = entry['maker']

    scui_widget_maker_c.write('/*一个通过scui_widget_parser脚本生成的widget动态构造器配置\n */\n\n')
    scui_widget_maker_c.write('#include "scui.h"\n\n')
    scui_widget_maker_c.write('/*@brief 控件构造器默认初始化\n')
    scui_widget_maker_c.write(' *@param maker 控件构造器实例指针\n')
    scui_widget_maker_c.write(' *@param type  控件类型(scui_widget_type_t)\n')
    scui_widget_maker_c.write(' */\n')
    scui_widget_maker_c.write('void scui_widget_maker_def_cfg(void *maker, scui_widget_type_t type)\n')
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
        for field, value in wdefault.items():
            c_field = field.replace('.', '.')
            scui_widget_maker_c.write('\t\t%s->%-30s = %s;\n' % (var_name, c_field, value))
        scui_widget_maker_c.write('\t\tbreak;\n')
        scui_widget_maker_c.write('\t}\n')

    scui_widget_maker_c.write('\tdefault:\n')
    scui_widget_maker_c.write('\t\tbreak;\n')
    scui_widget_maker_c.write('\t}\n')
    scui_widget_maker_c.write('}\n')
    scui_widget_maker_c.close()


# JSON 对齐格式化：将 dict 内字段按最长 key 对齐 value 列
def _json_aligned_format(obj, indent=0, step=4):
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
            val_str = _json_aligned_format(v, indent + 1, step)
            comma = ',' if i < len(items) - 1 else ''
            lines.append('%s%s: %s%s%s' % (isp + sp, key_str, pad, val_str, comma))
        lines.append(isp + '}')
        return '\n'.join(lines)

    elif isinstance(obj, list):
        if not obj:
            return '[]'
        lines = ['[']
        for i, item in enumerate(obj):
            val_str = _json_aligned_format(item, indent + 1, step)
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


def scui_json_realign(src_path, def_name):
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
                formatted = _json_aligned_format(data, step=4)
                with open(full, 'w', encoding='utf-8') as f:
                    f.write(formatted)
                    f.write('\n')
                count += 1
            except Exception as e:
                print('  [JSON realign] SKIP %s: %s' % (os.path.basename(full), e))
    if count > 0:
        print('[JSON realign] reformatted %d file(s)\n' % count)


# 清理 scene_src 下所有 JSON，去除与默认配置重复的字段（保留 widget.type）
def scui_json_cleanup(src_path, def_name, defaults_map):
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
                        f.write(_json_aligned_format(data, step=4))
                        f.write('\n')
                    print('  [JSON cleanup] %s: -%d field(s)' % (os.path.basename(full), file_removed))
                    removed_total += file_removed
            except Exception as e:
                print('  [JSON cleanup] SKIP %s: %s' % (os.path.basename(full), e))
    print('[JSON cleanup] total removed: %d fields\n' % removed_total)


# 清理 .c 文件中 scui_widget_maker_def_cfg() 已覆盖的冗余字段赋值
def scui_c_cleanup(c_root, defaults_map):
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

            # 第一遍：收集 def_cfg(&var, type) → (var_name, widget_type)
            def_cfg_map = {}
            for line in lines:
                m = re.match(
                    r'^\s*scui_widget_maker_def_cfg\(&(\w+),\s*(scui_widget_type_\w+)\);',
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


# 主流程
def scui_widget_parser():
    # json转Python字符串并转标准字典
    parser_path = os.path.join(os.path.dirname(__file__), 'scui_widget_parser.json')
    json_file = open(parser_path, 'r', encoding='utf-8')
    json_dict = json.loads(json_file.read())
    json_file.close()
    if json_dict['type'] != r'scui widget parser table':
        print('parser table unknown')
        return
    scui_widget_parser_table = json_dict
    # print(scui_widget_parser_table)
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
        scui_json_cleanup(src_path, os.path.basename(def_path), defaults_map)
        scui_json_realign(src_path, os.path.basename(def_path))

    # print重定向
    sys.stdout = ScuiRedirectPrint(sys.stdout, file=os.path.join(dst_path, 'scui_widget_parser.out'))   # redirect print
    sys.stderr = ScuiRedirectPrint(sys.stderr, file=os.path.join(dst_path, 'scui_widget_parser.err'))   # redirect print
    print('src path:', src_path)
    print('dst path:', dst_path)
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
    scui_widget_parser_scene_list(scene_list, scui_widget_parser_list, scui_widget_parser_table, defaults_map)
    scui_widget_parser_h.close()
    scui_widget_parser_c.close()
    # 生成 maker 默认初始化 .c/.h
    scui_widget_maker_generate(dst_path, def_path, defaults_map, scui_widget_parser_table)
    # 步骤最后：清理手写 .c 文件中 scui_widget_maker_def_cfg() 已覆盖的冗余字段
    if do_cleanup:
        c_root = os.path.normpath(os.path.join(src_path, os.pardir, os.pardir))
        scui_c_cleanup(c_root, defaults_map)


if __name__ == '__main__':
    try:
        scui_widget_parser()
    except Exception as e:
        print(e)
    print('scui widget parser finish')
    input('请按任意键退出...')
