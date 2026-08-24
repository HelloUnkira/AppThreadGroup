# encoding=utf-8
# json 协议 codegen 脚本
# 输入: nanopb 生成的 .pb.h (FIELDLIST 宏)
# 输出: app_json_xfer_pack.c / app_json_xfer_pack.h
#       每个消息一对 AppPB_XXX <-> cJSON 的通用 pack/unpack 函数
#
# 用法: python app_json_xfer_parser.py <nanopb_out_dir> <output_dir>
#   - nanopb_out_dir: 存放 app_nanopb_*.pb.h 的目录
#   - output_dir:     生成 app_json_xfer_pack.c/h 的输出目录


import os
import os.path
import re
import sys


# 解析 FIELDLIST 宏中的一行: X(a, STATIC,   SINGULAR, STRING,   model,             1)
FIELD_RE = re.compile(
    r'X\(\s*[a-z]\s*,\s*([A-Z]+)\s*,\s*([A-Z]+)\s*,\s*([A-Z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*,\s*(\d+)\s*\)'
)
# 解析 MSGTYPE 定义: #define AppPB_Weather_day_MSGTYPE AppPB_WeatherDay
MSGTYPE_RE = re.compile(r'#define\s+AppPB_(\w+)_(\w+)_MSGTYPE\s+AppPB_(\w+)')

# 解析消息名: #define AppPB_DevInfo_FIELDLIST(X, a) \
MSGNAME_RE = re.compile(r'#define\s+AppPB_(\w+)_FIELDLIST')

# 标量数字类型 -> cJSON 用 AddNumber / GetNumberValue
NUM_TYPES = {'UINT64', 'SINT64', 'UINT32', 'SINT32', 'FIXED64', 'SFIXED64',
             'FIXED32', 'SFIXED32', 'UENUM', 'BOOL', 'INT64', 'INT32', 'BOOL'}


def to_snake(name):
    """PascalCase 消息名转 snake_case 函数后缀
    DevInfo->dev_info  ElecCard->elec_card  ACK->ack  Ota->ota
    """
    result = []
    for i, c in enumerate(name):
        if c.isupper():
            if i > 0 and (name[i - 1].islower()
                          or (name[i - 1].isupper() and i + 1 < len(name) and name[i + 1].islower())):
                result.append('_')
            result.append(c.lower())
        else:
            result.append(c)
    return ''.join(result)


def parse_pb_h(path):
    """解析一个 .pb.h 文件, 返回 (messages, msgtype_map)
    messages: {消息名: [(字段类型, 字段名, 是否repeated, tag), ...]}
    msgtype_map: {'消息名_字段名': '子消息名', ...} (用于嵌套消息)
    """
    messages = {}
    msgtype_map = {}
    with open(path, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()

    # 找所有 MSGTYPE 映射
    for m in MSGTYPE_RE.finditer(content):
        msgtype_map['%s_%s' % (m.group(1), m.group(2))] = m.group(3)

    # 按消息切分 FIELDLIST
    # 找所有 AppPB_XXX_FIELDLIST 块
    blocks = list(MSGNAME_RE.finditer(content))
    for i, m in enumerate(blocks):
        msg_name = m.group(1)
        # 该消息的字段块从 m.start() 到下一个 FIELDLIST 或 "#define AppPB_XXX_CALLBACK"
        seg_end = content.find('#define AppPB_%s_CALLBACK' % msg_name, m.start())
        if seg_end == -1:
            continue
        seg = content[m.start():seg_end]
        fields = []
        skip = False
        for fm in FIELD_RE.finditer(seg):
            dtype = fm.group(1)      # STATIC/CALLBACK
            rule = fm.group(2)       # SINGULAR/REPEATED
            ftype = fm.group(3)      # STRING/UINT64/MESSAGE/...
            fname = fm.group(4)
            tag = int(fm.group(5))
            if dtype != 'STATIC':
                continue
            # BYTES 字段(文件数据块)暂不支持,跳过该消息
            if ftype == 'BYTES':
                skip = True
                break
            fields.append((rule, ftype, fname, tag))
        if fields and not skip:
            messages[msg_name] = fields

    return messages, msgtype_map


def c_field_type(ftype, rule, msgtype_map, msg_name, fname):
    """返回 C 访问字段时的类型信息"""
    if ftype == 'STRING':
        return ('str', 'char*', None)
    if ftype == 'MESSAGE':
        sub = msgtype_map.get('%s_%s' % (msg_name, fname))
        return ('msg', 'AppPB_%s*' % sub, sub)
    if ftype == 'BOOL':
        return ('num', 'bool', None)
    # 其它数字类型(包括枚举 UENUM)
    return ('num', 'int64_t', None)


def gen_pack_func(msg_name, fields, msgtype_map):
    """生成单个消息的 pack 函数体 (AppPB_XXX -> cJSON)"""
    lines = []
    lines.append('/*@brief 打包转换 %s 消息为 JSON 对象' % msg_name)
    lines.append(' *@param msg 源消息结构体')
    lines.append(' *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)')
    lines.append(' */')
    lines.append('cJSON *app_json_xfer_pack_%s(const AppPB_%s *msg)' % (to_snake(msg_name), msg_name))
    lines.append('{')
    lines.append('    cJSON *obj = cJSON_CreateObject();')
    lines.append('    if (obj == NULL || msg == NULL)')
    lines.append('        return obj;')
    for rule, ftype, fname, tag in fields:
        kind, ctype, sub = c_field_type(ftype, rule, msgtype_map, msg_name, fname)
        repeated = (rule == 'REPEATED')
        cnt_field = fname + '_count'  # nanopb repeated 用 xxx_count 表示个数
        if repeated:
            if kind == 'str':
                lines.append('    cJSON *%s_arr = cJSON_AddArrayToObject(obj, "%s");' % (fname, fname))
                lines.append('    for (pb_size_t %s_i = 0; %s_i < msg->%s; %s_i++)' % (fname, fname, cnt_field, fname))
                lines.append('        cJSON_AddItemToArray(%s_arr, cJSON_CreateString(msg->%s[%s_i]));' % (fname, fname, fname))
            elif kind == 'msg':
                lines.append('    cJSON *%s_arr = cJSON_AddArrayToObject(obj, "%s");' % (fname, fname))
                lines.append('    for (pb_size_t %s_i = 0; %s_i < msg->%s; %s_i++)' % (fname, fname, cnt_field, fname))
                lines.append('        cJSON_AddItemToArray(%s_arr, app_json_xfer_pack_%s(&msg->%s[%s_i]));' % (fname, to_snake(sub), fname, fname))
            else:  # num
                lines.append('    cJSON *%s_arr = cJSON_AddArrayToObject(obj, "%s");' % (fname, fname))
                lines.append('    for (pb_size_t %s_i = 0; %s_i < msg->%s; %s_i++)' % (fname, fname, cnt_field, fname))
                lines.append('        cJSON_AddItemToArray(%s_arr, cJSON_CreateNumber(msg->%s[%s_i]));' % (fname, fname, fname))
        else:
            if kind == 'str':
                lines.append('    cJSON_AddStringToObject(obj, "%s", msg->%s);' % (fname, fname))
            elif kind == 'msg':
                lines.append('    cJSON_AddItemToObject(obj, "%s", app_json_xfer_pack_%s(&msg->%s));' % (fname, to_snake(sub), fname))
            else:  # num
                lines.append('    cJSON_AddNumberToObject(obj, "%s", msg->%s);' % (fname, fname))
    lines.append('    return obj;')
    lines.append('}')
    return '\n'.join(lines)


def gen_unpack_func(msg_name, fields, msgtype_map):
    """生成单个消息的 unpack 函数体 (cJSON -> AppPB_XXX)"""
    lines = []
    lines.append('/*@brief 解析转换 JSON 对象为 %s 消息结构体' % msg_name)
    lines.append(' *@param obj 源 JSON 对象')
    lines.append(' *@param msg 目标消息结构体')
    lines.append(' *@retval 解析是否成功')
    lines.append(' */')
    lines.append('bool app_json_xfer_unpack_%s(cJSON *obj, AppPB_%s *msg)' % (to_snake(msg_name), msg_name))
    lines.append('{')
    lines.append('    if (obj == NULL || msg == NULL)')
    lines.append('        return false;')
    for rule, ftype, fname, tag in fields:
        kind, ctype, sub = c_field_type(ftype, rule, msgtype_map, msg_name, fname)
        repeated = (rule == 'REPEATED')
        cnt_field = fname + '_count'
        if repeated:
            lines.append('    cJSON *%s_arr = cJSON_GetObjectItem(obj, "%s");' % (fname, fname))
            lines.append('    if (%s_arr == NULL)' % fname)
            lines.append('        return false;')
            if kind == 'str':
                lines.append('    msg->%s = cJSON_GetArraySize(%s_arr);' % (cnt_field, fname))
                lines.append('    for (int %s_i = 0; %s_i < msg->%s; %s_i++)' % (fname, fname, cnt_field, fname))
                lines.append('        msg->%s[%s_i][0] = 0;' % (fname, fname))
            elif kind == 'msg':
                lines.append('    msg->%s = cJSON_GetArraySize(%s_arr);' % (cnt_field, fname))
                lines.append('    for (int %s_i = 0; %s_i < msg->%s; %s_i++)' % (fname, fname, cnt_field, fname))
                lines.append('        app_json_xfer_unpack_%s(cJSON_GetArrayItem(%s_arr, %s_i), &msg->%s[%s_i]);' % (to_snake(sub), fname, fname, fname, fname))
            else:  # num
                lines.append('    msg->%s = cJSON_GetArraySize(%s_arr);' % (cnt_field, fname))
                lines.append('    for (int %s_i = 0; %s_i < msg->%s; %s_i++)' % (fname, fname, cnt_field, fname))
                lines.append('        msg->%s[%s_i] = (uint64_t)cJSON_GetNumberValue(cJSON_GetArrayItem(%s_arr, %s_i));' % (fname, fname, fname, fname))
        else:
            lines.append('    cJSON *%s_item = cJSON_GetObjectItem(obj, "%s");' % (fname, fname))
            lines.append('    if (%s_item == NULL)' % fname)
            lines.append('        return false;')
            if kind == 'str':
                lines.append('    snprintf(msg->%s, sizeof(msg->%s), "%%s", cJSON_GetStringValue(%s_item));' % (fname, fname, fname))
            elif kind == 'msg':
                lines.append('    app_json_xfer_unpack_%s(%s_item, &msg->%s);' % (to_snake(sub), fname, fname))
            else:  # num
                lines.append('    msg->%s = (uint64_t)cJSON_GetNumberValue(%s_item);' % (fname, fname))
    lines.append('    return true;')
    lines.append('}')
    return '\n'.join(lines)


def main():
    if len(sys.argv) < 3:
        print('usage: python app_json_xfer_parser.py <nanopb_out_dir> <output_dir>')
        return
    pb_dir = os.path.abspath(sys.argv[1])
    out_dir = os.path.abspath(sys.argv[2])

    all_messages = {}     # 所有消息: {消息名: [(rule, ftype, fname, tag), ...]}
    all_msgtype = {}      # 所有嵌套映射
    for item in sorted(os.listdir(pb_dir)):
        if not item.endswith('.pb.h'):
            continue
        if not item.startswith('app_nanopb_'):
            continue
        path = os.path.join(pb_dir, item)
        messages, msgtype_map = parse_pb_h(path)
        all_messages.update(messages)
        all_msgtype.update(msgtype_map)

    # 传递性跳过含 BYTES 的消息: 某消息(或其嵌套消息)含 BYTES 字段时其整体
    # 无法用 JSON 表示,避免生成引用缺失子 pack/unpack 的坏代码(如 File 包裹 FilePkg)
    has_bytes = set()
    changed = True
    while changed:
        changed = False
        for name in list(all_messages.keys()):
            if name in has_bytes:
                continue
            for rule, ftype, fname, tag in all_messages[name]:
                if ftype == 'BYTES':
                    has_bytes.add(name)
                    changed = True
                    break
                if ftype == 'MESSAGE':
                    sub = all_msgtype.get('%s_%s' % (name, fname))
                    if sub and sub in has_bytes:
                        has_bytes.add(name)
                        changed = True
                        break
    for name in has_bytes:
        all_messages.pop(name, None)

    # 按依赖顺序: 嵌套消息先声明。简单起见按消息名排序
    names = sorted(all_messages.keys())

    # 生成 .h
    h_lines = []
    h_lines.append('#ifndef APP_JSON_XFER_PACK_H')
    h_lines.append('#define APP_JSON_XFER_PACK_H')
    h_lines.append('')
    h_lines.append('#if APP_MODULE_PROTOCOL_USE_JSON')
    h_lines.append('')
    h_lines.append('#include "cJSON.h"')
    h_lines.append('#include "app_nanopb_set.pb.h"')
    h_lines.append('')
    for n in names:
        h_lines.append('/*@brief 打包转换 %s 消息为 JSON 对象' % n)
        h_lines.append(' *@param msg 源消息结构体')
        h_lines.append(' *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)')
        h_lines.append(' */')
        h_lines.append('cJSON *app_json_xfer_pack_%s(const AppPB_%s *msg);' % (to_snake(n), n))
        h_lines.append('')
        h_lines.append('/*@brief 解析转换 JSON 对象为 %s 消息结构体' % n)
        h_lines.append(' *@param obj 源 JSON 对象')
        h_lines.append(' *@param msg 目标消息结构体')
        h_lines.append(' *@retval 解析是否成功')
        h_lines.append(' */')
        h_lines.append('bool app_json_xfer_unpack_%s(cJSON *obj, AppPB_%s *msg);' % (to_snake(n), n))
        h_lines.append('')
    h_lines.append('#endif')
    h_lines.append('')
    h_lines.append('#endif')
    h_lines.append('')

    # 生成 .c
    c_lines = []
    c_lines.append('/*实现目标:')
    c_lines.append(' *    nanopb消息结构体(AppPB_XXX)与JSON对象的通用转换层')
    c_lines.append(' *    由 app_json_xfer_parser.py 自动生成, 请勿手动修改')
    c_lines.append(' */')
    c_lines.append('')
    c_lines.append('#include "app_ext_lib.h"')
    c_lines.append('#include "app_sys_lib.h"')
    c_lines.append('#include "app_thread_group.h"')
    c_lines.append('#include "app_protocol_lib.h"')
    c_lines.append('')
    c_lines.append('#if APP_MODULE_PROTOCOL_USE_JSON')
    c_lines.append('')
    for n in names:
        c_lines.append(gen_pack_func(n, all_messages[n], all_msgtype))
        c_lines.append('')
        c_lines.append(gen_unpack_func(n, all_messages[n], all_msgtype))
        c_lines.append('')
    c_lines.append('#endif')
    c_lines.append('')

    # 写文件
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    try:
        with open(os.path.join(out_dir, 'app_json_xfer_pack.h'), 'w', encoding='utf-8', newline='\r\n') as f:
            f.write('\n'.join(h_lines))
        with open(os.path.join(out_dir, 'app_json_xfer_pack.c'), 'w', encoding='utf-8', newline='\r\n') as f:
            f.write('\n'.join(c_lines))
        print('json xfer pack generate success (%d messages)' % len(names))
    except Exception as e:
        print('json xfer pack generate failed: %s' % e)
        sys.exit(1)


if __name__ == '__main__':
    main()
