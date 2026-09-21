import os
import sys
import json
import functools
import re


# tools目录基准/plugs目录: 打包(exe)环境下由前端注入真实路径; 源码运行默认由 __file__ 推导
SCUI_CWF_TOOLS = None
SCUI_CWF_PLUGS = None

# 字体条目标记: 复用info.format字段(0xFE为哨兵值, 不占像素格式枚举, 固件端按此识别字体条目)
SCUI_CWF_JSON_PIXEL_CF_FONT = 0xFE


# 解析器基础设施目录(读 协议 json)
def _scui_cwf_tools_dir():
    return SCUI_CWF_TOOLS or os.path.dirname(__file__)


# 固件解析器目录(写 生成头文件 scui_cwf_json_proto.h)
def _scui_cwf_plugs_dir():
    if SCUI_CWF_PLUGS:
        return SCUI_CWF_PLUGS
    return os.path.normpath(os.path.join(_scui_cwf_tools_dir(), '..', 'plugs'))


# 协议 json 完整路径
def _scui_cwf_proto_json():
    return os.path.join(_scui_cwf_tools_dir(), 'scui_pack_cwf.json')


# 缓存列表, 统一生成到 tools 目录的 __pack_tmp__ 下, 与提交无关(见 tools/.gitignore)
# 用函数取而不是模块常量: 打包(exe)环境下 tools 目录由前端在调用前注入
def scui_cwf_json_parser_tmp_dir():
    return os.path.join(_scui_cwf_tools_dir(), '__pack_tmp__')


def scui_cwf_json_parser_tmp_bin():
    tmp_dir = scui_cwf_json_parser_tmp_dir()
    return [
        os.path.join(tmp_dir, 'scui.cwf.json.bin'),
        os.path.join(tmp_dir, 'scui.cwf.image_info.bin'),
        os.path.join(tmp_dir, 'scui.cwf.image_data.bin'),
    ]


# 清洗协议json的annotation字段, 将其移除
def scui_cwf_json_parser_remove_annotations(json_obj):
    if isinstance(json_obj, dict):
        json_obj = {k: scui_cwf_json_parser_remove_annotations(v) for k, v in json_obj.items()
                    if not (k.endswith('annotation') or k == 'anno')}
    elif isinstance(json_obj, list):
        json_obj = [scui_cwf_json_parser_remove_annotations(item) for item in json_obj]
    return json_obj


# 协议枚举项 -> 名字(兼容纯字符串与 {"key": ...} 两种写法)
def scui_cwf_json_enum_key(item):
    return item['key'] if isinstance(item, dict) else item


# 协议枚举表 -> 名字表(过滤掉尾部哨兵)
def scui_cwf_json_enum_list(json_parser, table, sentinel=True):
    names = [scui_cwf_json_enum_key(item) for item in json_parser[table]]
    if not sentinel:
        names = [name for name in names if not name.endswith('_num')]
    return names


# 枚举字段: 名字 -> 下标(原地替换)
def scui_cwf_json_enum_remap(node, json_parser, field, table):
    for idx, name in enumerate(scui_cwf_json_enum_list(json_parser, table)):
        if name == node[field]:
            node[field] = idx
            return
    raise ValueError('json %s error:%r' % (field, node[field]))


# 对json进行预处理
def scui_cwf_json_parser_preprocess(c_file_list, json_obj):
    # 打开协议json
    parser_path = _scui_cwf_proto_json()
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_parser = json.load(file)
    # 清洗json的annotation字段, 将其移除
    json_parser = scui_cwf_json_parser_remove_annotations(json_parser)
    json_obj = scui_cwf_json_parser_remove_annotations(json_obj)
    # print(json.dumps(json_parser, indent=4))
    # print(json.dumps(json_obj, indent=4))
    # 清洗后缀, 提出图片的前缀
    image_list = [file_name.rsplit('.', 1)[0] for file_name in c_file_list]
    # 清洗img_res字段: 图集名 -> 图集下标; 同时登记image_num(实际张数)
    for idx, node in enumerate(json_obj['layout']):
        if 'img_res' in node:
            img_res = node['img_res']
            node['img_res'] = [image_list.index(image) for image in img_res]
            node['image_num'] = len(img_res)
    # 清洗font_res字段: 字库名 -> 资源表下标(字体条目在列表尾部)
    for idx, node in enumerate(json_obj['layout']):
        if 'font_res' in node:
            font_res = node['font_res']
            node['font_res'] = [image_list.index(font) for font in font_res]
    # 替换枚举字段: 名字 -> 下标
    for idx, node in enumerate(json_obj['layout']):
        for field, table in (('type',   'scui_cwf_json_type'),
                             ('source', 'scui_cwf_json_source'),
                             ('key',    'scui_cwf_json_key'),
                             ('font',   'scui_cwf_json_font'),
                             ('align',  'scui_cwf_json_align')):
            if field in node:
                scui_cwf_json_enum_remap(node, json_parser, field, table)
    # print(json.dumps(json_obj, indent=4))
    return json_obj


# 子流程:cwf版本号
def scui_cwf_json_parser_ver():
    # 打开协议json
    parser_path = _scui_cwf_proto_json()
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_data = json.load(file)
        print('version:', json_data['version'])  # 转为一个整数
    return functools.reduce(lambda x, y: x * 256 + y, json_data['version'])


# 子流程:生成协议c头文件
def scui_cwf_json_parser_proto():
    # 打开协议json
    parser_path = _scui_cwf_proto_json()
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_parser = json.load(file)
    # 清洗json的annotation字段, 将其移除
    json_parser = scui_cwf_json_parser_remove_annotations(json_parser)
    # 生成或更新protocol(输出到固件解析器目录, 供 scui_plugs.h 引用)
    os.makedirs(_scui_cwf_plugs_dir(), exist_ok=True)
    parser_path = os.path.join(_scui_cwf_plugs_dir(), 'scui_cwf_json_proto.h')
    # 协议枚举表 -> 固件枚举(序列一律由协议导出, 固件不再自行定义)
    enum_tables = (
        ('scui_cwf_json_pixel_cf', 'scui_cwf_json_pixel_cf_t'),
        ('scui_cwf_json_image_cf', 'scui_cwf_json_image_cf_t'),
        ('scui_cwf_json_type',     'scui_cwf_json_type_t'),
        ('scui_cwf_json_source',   'scui_cwf_json_source_t'),
        ('scui_cwf_json_key',      'scui_cwf_json_key_t'),
        ('scui_cwf_json_font',     'scui_cwf_json_font_t'),
        ('scui_cwf_json_align',    'scui_cwf_json_align_t'),
        ('scui_cwf_json_lang',     'scui_cwf_json_lang_t'),
    )
    with open(parser_path, mode='w', encoding='utf-8') as file:
        file.write('#ifndef SCUI_CWF_JSON_PROTO_H\n')
        file.write('#define SCUI_CWF_JSON_PROTO_H\n\n')
        file.write('#define SCUI_CWF_JSON_VER\t\t{%d, %d, %d, %d}\n\n' %
            (json_parser['version'][3], json_parser['version'][2],
             json_parser['version'][1], json_parser['version'][0]))
        
        for table, ctype in enum_tables:
            file.write('typedef enum {\n')
            for idx, name in enumerate(scui_cwf_json_enum_list(json_parser, table)):
                file.write('\t%s%s,\n' % (name, ' = 0' if idx == 0 else ''))
            file.write('} %s;\n\n' % ctype)
        # 字体条目标记: 复用info.format字段(0xFE为哨兵值, 不占像素格式枚举)
        file.write('#define SCUI_CWF_JSON_PIXEL_CF_FONT\t\t(0x%X)\n\n' % SCUI_CWF_JSON_PIXEL_CF_FONT)
        # seq 字符集(下标即字符在序列中的位置, 图集张数由序列长度决定)
        seq_set = json_parser['scui_cwf_json_seq_set']
        file.write('#define SCUI_CWF_JSON_SEQ_SET\t"%s"\n' % seq_set)
        file.write('#define SCUI_CWF_JSON_SEQ_NUM\t%d\n' % len(seq_set))
        file.write('\n')
        
        file.write('#endif\n')

# 收集图片文件生成列表
def scui_cwf_json_parser_image_collect(image_path):
    # 统计所有的image c文件
    c_file_list = []
    font_file_list = []
    for file_name in sorted(os.listdir(image_path)):
        # 字库文件(ttf/bin): 原样收集到列表尾部(固件端约定字体条目在资源表尾部)
        ext = file_name.rsplit('.', 1)[-1].lower()
        if ext in ('ttf', 'otf', 'bin'):
            font_file_list.append(file_name)
            continue
        # 处理每一个文件, 如果匹配到指定结构, 该文件便是我们要找的目标
        with open(os.path.join(image_path, file_name), mode='r', encoding='utf-8') as file:
            if re.search('const scui_image_t', file.read()):
                c_file_list.append(file_name)
    print('image src file num:%d' % len(c_file_list))
    if font_file_list:
        print('font src file num:%d' % len(font_file_list))
    return c_file_list + font_file_list


# 子流程:json文件处理,存入缓存列表中去
def scui_cwf_json_parser_json(src_path, dst_path, c_file_list, json_name):
    # 读出json文件预处理后序列化
    with open(os.path.join(src_path, json_name + '.json'), mode='r', encoding='utf-8') as file:
        json_data = json.dumps(scui_cwf_json_parser_preprocess(c_file_list, json.load(file)))
        json_bytes = bytes(re.sub(r'\s+', '', json_data), encoding='utf-8')
        print("parser json bytes:%d" % len(json_bytes))
    # 写一个处理后的json, 用于核对检查
    with open(os.path.join(dst_path, json_name + '_json.prog'), mode='w', encoding='utf-8') as file:
        file.write(json.dumps(json.loads(json_data), indent=4))
    # 写入json文件到达缓存目标文件
    with open(scui_cwf_json_parser_tmp_bin()[0], mode='wb') as file:
        file.write(json_bytes)
        file.write(int(0).to_bytes(byteorder='little', length=1))
    #
    return len(json_bytes) + 1


# 子流程:image info处理
def scui_cwf_json_parser_image_info(image_path, c_file_list, c_data_offset):
    # 打开协议json, 获取像素格式/图片类型枚举映射
    parser_path = _scui_cwf_proto_json()
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_parser = json.load(file)
    pixel_cf_list = json_parser['scui_cwf_json_pixel_cf']
    image_cf_list = json_parser['scui_cwf_json_image_cf']
    # 构建 scui_pixel_cf_xxx -> scui_cwf_json_pixel_cf_xxx 索引映射
    pixel_cf_map = {}
    for idx, item in enumerate(pixel_cf_list):
        tag = 'scui_pixel_cf_' + item[len('scui_cwf_json_pixel_cf_'):]
        pixel_cf_map[tag] = idx
    # 构建 scui_image_type_xxx -> scui_cwf_json_image_cf_xxx 索引映射
    image_cf_map = {}
    for idx, item in enumerate(image_cf_list):
        tag = 'scui_image_type_' + item[len('scui_cwf_json_image_cf_'):]
        image_cf_map[tag] = idx
    # 对所有的image info统计
    image_info_bytes = bytearray()
    for file_name in c_file_list:
        # 字库文件: 直接按文件大小登记(format=0xFE标记, 固件端识别为字体条目)
        ext = file_name.rsplit('.', 1)[-1].lower()
        if ext in ('ttf', 'otf', 'bin'):
            font_size = os.path.getsize(os.path.join(image_path, file_name))
            if font_size == 0:
                raise ValueError('incomplete font info %s' % file_name)
            scui_image_data = c_data_offset
            c_data_offset += font_size
            image_info_bytes.extend(int(SCUI_CWF_JSON_PIXEL_CF_FONT).to_bytes(byteorder='little', length=1))
            image_info_bytes.extend(int(0).to_bytes(byteorder='little', length=1))
            image_info_bytes.extend(int(0).to_bytes(byteorder='little', length=4))   # height
            image_info_bytes.extend(int(0).to_bytes(byteorder='little', length=4))   # width
            image_info_bytes.extend(font_size.to_bytes(byteorder='little', length=4))
            image_info_bytes.extend(int(0).to_bytes(byteorder='little', length=4))   # size_mem
            image_info_bytes.extend(scui_image_data.to_bytes(byteorder='little', length=4))
            continue
        scui_image_format = 0
        scui_image_type = 0
        scui_image_height = 0
        scui_image_width = 0
        scui_image_size = 0
        scui_image_size_mem = 0
        scui_image_data = 0
        # 对所有的image info统计
        with open(os.path.join(image_path, file_name), mode='r', encoding='utf-8') as file:
            for line in file.readlines():
                if re.search(r'\.format', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    if payload in pixel_cf_map:
                        scui_image_format = pixel_cf_map[payload]
                if re.search(r'\.type', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    if payload in image_cf_map:
                        scui_image_type = image_cf_map[payload]
                if re.search(r'\.pixel\.height', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_height = int(payload[2:], 16)
                if re.search(r'\.pixel\.width', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_width = int(payload[2:], 16)
                if re.search(r'\.pixel\.size_bin', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_size = int(payload[2:], 16)
                if re.search(r'\.pixel\.size_mem', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_size_mem = int(payload[2:], 16)
        scui_image_data = c_data_offset
        c_data_offset += scui_image_size
        # 简单过滤一下
        if scui_image_format == 0 or scui_image_height == 0 or scui_image_width == 0 or scui_image_size == 0:
            raise ValueError('incomplete image info %s' % file_name)
        # 打包成cwf约定结构 'u8u8u32u32u32u32u32'
        image_info_bytes.extend(scui_image_format.to_bytes(byteorder='little', length=1))
        image_info_bytes.extend(scui_image_type.to_bytes(byteorder='little', length=1))
        image_info_bytes.extend(scui_image_height.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_width.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_size.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_size_mem.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_data.to_bytes(byteorder='little', length=4))
    # 写入json文件到达缓存目标文件
    with open(scui_cwf_json_parser_tmp_bin()[1], mode='wb') as file:
        file.write(image_info_bytes)


# 子流程:image data处理
def scui_cwf_json_parser_image_data(image_path, c_file_list):
    image_data_bytes = bytearray()
    # 对所有的image data统计
    for file_name in c_file_list:
        # 字库文件: 原样字节流(与info的format=0xFE条目对齐)
        ext = file_name.rsplit('.', 1)[-1].lower()
        if ext in ('ttf', 'otf', 'bin'):
            with open(os.path.join(image_path, file_name), mode='rb') as file:
                image_data_bytes.extend(file.read())
            continue
        # 从c源文件的此区域内, 提取十六进制字符串的bin文件数据流
        image_size = 0
        context_size = 0
        context_pattern_s = "const uint8_t scui_image_array\[\] = {"
        context_pattern_e = "}"
        working = False
        # 对所有的image data打包
        with open(os.path.join(image_path, file_name), mode='r', encoding='utf-8') as file:
            for line in file.readlines():
                if re.search(context_pattern_e, line) and working:
                    # print("context_pattern_e")
                    working = False
                    break
                if working:
                    for char in line.strip().split(','):
                        if char.strip() == '':
                            continue
                        image_data_bytes.extend(int(char.strip(), 16).to_bytes(byteorder='little', length=1))
                        context_size += 1
                if re.search(context_pattern_s, line):
                    # print("context_pattern_s")
                    working = True
        # 对所有的image data打包
        with open(os.path.join(image_path, file_name), mode='r', encoding='utf-8') as file:
            for line in file.readlines():
                if re.search(r'\.pixel\.size_bin', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    image_size = int(payload[2:], 16)
        # 检查一下image data大小是否匹配
        if image_size != context_size:
            print("image array bin size unmatched:%s" % file_name)
            print("image array bin size diff: <%d, %d>" % (image_size, context_size))
    # 写入json文件到达缓存目标文件
    with open(scui_cwf_json_parser_tmp_bin()[2], mode='wb') as file:
        file.write(image_data_bytes)


# 主流程
def scui_cwf_json_parser():
    # 参数列表: 相对路径
    if len(sys.argv) != 3:
        print('argv list not match')
        return
    src_path = sys.argv[1]
    dst_path = sys.argv[2]
    img_path = os.path.join(src_path, 'image_array')
    # 获得文件处理相对路径
    if not os.path.exists(src_path):
        print('src path is not exist')
        return
    if not os.path.exists(dst_path):
        print('dst path is not exist')
        return
    if not os.path.exists(img_path):
        print('img path is not exist')
        return
    # 确保统一临时目录存在
    os.makedirs(scui_cwf_json_parser_tmp_dir(), exist_ok=True)
    # 生成或更新json对应c头文件
    scui_cwf_json_parser_proto()
    # 读取本文件夹下所有的json文件并获得文件名
    json_file_list = [file_name for file_name in os.listdir(src_path) if file_name.find('.json') != -1]
    if len(json_file_list) != 1:
        raise ValueError('unexpected json file number')
    json_name = json_file_list[0].rsplit('.', 1)[0]
    # json文件整理后转入临时文件
    c_file_list = scui_cwf_json_parser_image_collect(img_path)
    json_bytes = scui_cwf_json_parser_json(src_path, dst_path, c_file_list, json_name)
    # image info 和 image data整理后转入临时文件
    c_data_offset = 28 + json_bytes + len(c_file_list) * 22
    scui_cwf_json_parser_image_info(img_path, c_file_list, c_data_offset)
    scui_cwf_json_parser_image_data(img_path, c_file_list)
    # 打包三个文件变成一个
    with open(scui_cwf_json_parser_tmp_bin()[0], mode='rb') as file:
        json_bytes = file.read()
    with open(scui_cwf_json_parser_tmp_bin()[1], mode='rb') as file:
        image_info_bytes = file.read()
    with open(scui_cwf_json_parser_tmp_bin()[2], mode='rb') as file:
        image_data_bytes = file.read()
    # 打包三个文件变成一个
    with open(os.path.join(dst_path, json_name + '.bin'), mode='wb') as file:
        cwf_ver = scui_cwf_json_parser_ver()
        bin_ofs1 = 28
        bin_ofs2 = 28 + len(json_bytes)
        bin_ofs3 = 28 + len(json_bytes) + len(image_info_bytes)
        file.write(cwf_ver.to_bytes(byteorder='little', length=4))
        file.write(bin_ofs1.to_bytes(byteorder='little', length=4))
        file.write(len(json_bytes).to_bytes(byteorder='little', length=4))
        file.write(bin_ofs2.to_bytes(byteorder='little', length=4))
        file.write(len(image_info_bytes).to_bytes(byteorder='little', length=4))
        file.write(bin_ofs3.to_bytes(byteorder='little', length=4))
        file.write(len(image_data_bytes).to_bytes(byteorder='little', length=4))
        
        file.write(json_bytes)
        file.write(image_info_bytes)
        file.write(image_data_bytes)
    # 打印一下总固件大小
    with open(os.path.join(dst_path, json_name + '.bin'), mode='rb') as file:
        print('parser bin bytes:%d' % len(file.read()))


if __name__ == '__main__':
    try:
        scui_cwf_json_parser()
    except Exception as e:
        print(e)
    
    # 中间文件一律保留(与打包器共用同一套清除时机: 下次打包的 step0 清空)
    # 位置: scui/tools/__pack_tmp__, 已被 tools/.gitignore 忽略
    
    print('scui cwf parser finish')
    input('请按任意键退出...')
