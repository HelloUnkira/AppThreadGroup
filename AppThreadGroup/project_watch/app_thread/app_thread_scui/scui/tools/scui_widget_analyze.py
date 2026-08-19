# 控件配置代码分析
# 从代码解析动态参数表: 控件名前缀/字段枚举/字段路径→值槽映射
# 生成 scui_widget_analyze.tmp 供 scui_widget_parser.py 使用
import re
import os
import sys
import json


# ============================================================
# 配置(与 scui_widget_parser.py 保持一致)
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
# 字段枚举定义源(相对本脚本)
SCUI_WIDGET_JSON_FIELD_SOURCE = '../widget/widget/scui_widget.h'
# 控件源码根(相对本脚本, 扫描结构定义)
SCUI_WIDGET_SOURCE_ROOT = '..'
# 分析结果输出(相对本脚本)
SCUI_WIDGET_ANALYZE_TMP = 'scui_widget_analyze.tmp'
# ============================================================

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
def scui_widget_analyze_first_fields(prefixes):
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


# ---------- 递归结构解析 ----------

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
            # 匿名嵌套 struct/union: 内联展开成员, 前缀用结尾名
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
                kind = m.group(1)
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
                    types[nm.group(1)] = (kind, text[open_i + 1:i - 1])
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
    kind, body = types[tname]
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
            # 别名: 目标为结构类型才继续递归, 否则按叶子处理
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


# 生成分析结果到 tmp
def scui_widget_analyze(out_path):
    prefixes = scui_widget_analyze_prefixes()
    first_fields = scui_widget_analyze_first_fields(prefixes)
    root = os.path.normpath(os.path.join(os.path.dirname(__file__), SCUI_WIDGET_SOURCE_ROOT))
    types, aliases = scui_widget_analyze_collect_types(root)
    path_slots = scui_widget_analyze_path_slots(types, aliases)
    result = {
        'prefixes':     prefixes,
        'first_fields': first_fields,
        'path_slots':   path_slots,
        'class_maker':  SCUI_WIDGET_CLASS_MAKER,
    }
    with open(out_path, 'w', encoding='utf-8') as fp:
        json.dump(result, fp, ensure_ascii=False, indent=1)
    print('[analyze] tmp: %s' % os.path.basename(out_path))
    print('[analyze] prefixes:')
    for item in prefixes:
        print('  %s' % item)
    print()
    print('[analyze] first_fields:')
    for item in first_fields:
        print('  %s' % (item,))
    print()
    print('[analyze] path_slots: %d' % len(path_slots))
    print()
    return result


if __name__ == '__main__':
    out_path = os.path.join(os.path.dirname(__file__), SCUI_WIDGET_ANALYZE_TMP)
    if len(sys.argv) > 1:
        out_path = sys.argv[1]
    scui_widget_analyze(out_path)
