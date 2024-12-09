import os
import sys
import json
import functools
import re


# 缓存列表, 与本脚本共用路径, 流程结束后移除
scui_cwf_json_parser_tmp_bin = [
    'scui_cwf_json_parser.tmp.json.bin',
    'scui_cwf_json_parser.tmp.image_info.bin',
    'scui_cwf_json_parser.tmp.image_data.bin',
]


# 清洗协议json的annotation字段, 将其移除
def scui_cwf_json_parser_remove_annotations(json_obj):
    if isinstance(json_obj, dict):
        json_obj = {k: scui_cwf_json_parser_remove_annotations(v) for k, v in json_obj.items() if not k.endswith('annotation')}
    elif isinstance(json_obj, list):
        json_obj = [scui_cwf_json_parser_remove_annotations(item) for item in json_obj]
    return json_obj


# 对json进行预处理
def scui_cwf_json_parser_preprocess(c_file_list, json_obj):
    # 打开协议json
    parser_path = os.path.join(os.path.dirname(__file__), 'scui_cwf_json_parser.json')
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_parser = json.load(file)
    # 清洗json的annotation字段, 将其移除
    json_parser = scui_cwf_json_parser_remove_annotations(json_parser)
    json_obj = scui_cwf_json_parser_remove_annotations(json_obj)
    # print(json.dumps(json_parser, indent=4))
    # print(json.dumps(json_obj, indent=4))
    # 清洗后缀, 提出图片的前缀
    image_list = [file_name.rsplit('.', 1)[0] for file_name in c_file_list]
    # print(image_list)
    # 清洗image_src字段, 将其替换成协议c_file_list中列表下标
    # 更新image_num字段, 将其替换成实际image_src数量
    for idx, node in enumerate(json_obj['layout']):
        # 有些类型没有这个字段
        if re.match('scui_cwf_json_type_img_', node['type']) is not None:
            image_src = json_obj['layout'][idx]['image_src']
            for idx_t, image in enumerate(node['image_src']):
                json_obj['layout'][idx]['image_src'][idx_t] = image_list.index(image)
            json_obj['layout'][idx]['image_num'] = len(image_src)
    # 替换type
    for idx, node in enumerate(json_obj['layout']):
        for idx_t, dict in enumerate(json_parser['scui_cwf_json_type']):
            if dict['key'] == node['type']:
                json_obj['layout'][idx]['type'] = idx_t
                break
        if type(json_obj['layout'][idx]['type']) != int:
            print(json.dumps(json_obj, indent=4))
            raise ValueError("json type error:%d" % idx)
    # print(json.dumps(json_obj, indent=4))
    return json_obj


# 子流程:cwf版本号
def scui_cwf_json_parser_ver():
    # 打开协议json
    parser_path = os.path.join(os.path.dirname(__file__), 'scui_cwf_json_parser.json')
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_data = json.load(file)
        print('version:', json_data['version'])  # 转为一个整数
    return functools.reduce(lambda x, y: x * 256 + y, json_data['version'])


# 子流程:生成协议c头文件
def scui_cwf_json_parser_proto():
    # 打开协议json
    parser_path = os.path.join(os.path.dirname(__file__), 'scui_cwf_json_parser.json')
    with open(parser_path, mode='r', encoding='utf-8') as file:
        json_parser = json.load(file)
    # 清洗json的annotation字段, 将其移除
    json_parser = scui_cwf_json_parser_remove_annotations(json_parser)
    # 生成或更新protocol
    parser_path = os.path.join(os.path.dirname(__file__), 'scui_cwf_json_proto.h')
    with open(parser_path, mode='w', encoding='utf-8') as file:
        file.write('#ifndef SCUI_CWF_JSON_PROTO_H\n')
        file.write('#define SCUI_CWF_JSON_PROTO_H\n\n')
        file.write('#define SCUI_CWF_JSON_VER\t\t{%d, %d, %d, %d}\n\n' %
            (json_parser['version'][3], json_parser['version'][2],
             json_parser['version'][1], json_parser['version'][0]))
        
        file.write('typedef enum {\n')
        for idx, item in enumerate(json_parser['scui_cwf_json_image_cf']):
            file.write('\t%s%s,\n' % (item, ' = 0' if idx == 0 else ''))
        file.write('} scui_cwf_json_image_cf_t;\n\n')
        
        file.write('typedef enum {\n')
        for idx, dict in enumerate(json_parser['scui_cwf_json_type']):
            file.write('\t%s%s,\n' % (dict['key'], ' = 0' if idx == 0 else ''))
        file.write('} scui_cwf_json_type_t;\n\n')
        
        file.write('#endif\n')

# 收集图片文件生成列表
def scui_cwf_json_parser_image_collect(image_path):
    # 统计所有的image c文件
    c_file_list = []
    for file_name in [file_name for file_name in os.listdir(image_path)]:
        # 处理每一个文件, 如果匹配到指定结构, 该文件便是我们要找的目标
        with open(os.path.join(image_path, file_name), mode='r', encoding='utf-8') as file:
            if re.search('const scui_image_t', file.read()):
                c_file_list.append(file_name)
    print('image src file num:%d' % len(c_file_list))
    return c_file_list


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
    with open(scui_cwf_json_parser_tmp_bin[0], mode='wb') as file:
        file.write(json_bytes)
        file.write(int(0).to_bytes(byteorder='little', length=1))
    #
    return len(json_bytes) + 1


# 子流程:image info处理
def scui_cwf_json_parser_image_info(image_path, c_file_list, c_data_offset):
    image_info_bytes = bytearray()
    # 对所有的image info统计
    for file_name in c_file_list:
        scui_image_format = 0
        scui_image_type = 0
        scui_image_height = 0
        scui_image_width = 0
        scui_image_size = 0
        scui_image_data = 0
        # 对所有的image info统计
        with open(os.path.join(image_path, file_name), mode='r', encoding='utf-8') as file:
            for line in file.readlines():
                if re.search(r'\.format', line):
                    payload = line.split('=')[1].split(',')[0]
                    if re.search('scui_pixel_cf_palette4', payload):
                        scui_image_format = 1
                    if re.search('scui_pixel_cf_bmp565', payload):
                        scui_image_format = 3
                    if re.search('scui_pixel_cf_bmp8565', payload):
                        scui_image_format = 5
                if re.search(r'\.type', line):
                    payload = line.split('=')[1].split(',')[0]
                    if re.search('scui_image_type_bmp', payload):
                        scui_image_type = 0
                    if re.search('scui_image_type_lz4', payload):
                        scui_image_type = 1
                    if re.search('scui_image_type_jpg', payload):
                        scui_image_type = 2
                    if re.search('scui_image_type_png', payload):
                        scui_image_type = 3
                if re.search(r'\.pixel\.height', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_height = int(payload[2:], 16)
                if re.search(r'\.pixel\.width', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_width = int(payload[2:], 16)
                if re.search(r'\.pixel\.size_bin', line):
                    payload = line.split('=')[1].split(',')[0].strip()
                    scui_image_size = int(payload[2:], 16)
        scui_image_data = c_data_offset
        c_data_offset += scui_image_size
        # 简单过滤一下
        if scui_image_format == 0 or scui_image_height == 0 or scui_image_width == 0 or scui_image_size == 0:
            raise ValueError('incomplete image info %s' % file_name)
        # 打包成cwf约定结构 'u8u8u32u32u32u32'
        image_info_bytes.extend(scui_image_format.to_bytes(byteorder='little', length=1))
        image_info_bytes.extend(scui_image_type.to_bytes(byteorder='little', length=1))
        image_info_bytes.extend(scui_image_height.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_width.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_size.to_bytes(byteorder='little', length=4))
        image_info_bytes.extend(scui_image_data.to_bytes(byteorder='little', length=4))
    # 写入json文件到达缓存目标文件
    with open(scui_cwf_json_parser_tmp_bin[1], mode='wb') as file:
        file.write(image_info_bytes)


# 子流程:image data处理
def scui_cwf_json_parser_image_data(image_path, c_file_list):
    image_data_bytes = bytearray()
    # 对所有的image data统计
    for file_name in c_file_list:
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
    with open(scui_cwf_json_parser_tmp_bin[2], mode='wb') as file:
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
    c_data_offset = 28 + json_bytes + len(c_file_list) * 18
    scui_cwf_json_parser_image_info(img_path, c_file_list, c_data_offset)
    scui_cwf_json_parser_image_data(img_path, c_file_list)
    # 打包三个文件变成一个
    with open(scui_cwf_json_parser_tmp_bin[0], mode='rb') as file:
        json_bytes = file.read()
    with open(scui_cwf_json_parser_tmp_bin[1], mode='rb') as file:
        image_info_bytes = file.read()
    with open(scui_cwf_json_parser_tmp_bin[2], mode='rb') as file:
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
    
    # 移除临时文件
    for file_name in scui_cwf_json_parser_tmp_bin:
        if os.path.isfile(file_name):
            os.remove(file_name)
    
    print('scui cwf parser finish')
    input('请按任意键退出...')
