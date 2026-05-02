# 图片打包文件
import re
import os
import sys
import json


# 句柄表偏移:字库描述
scui_font_offset_name = 'SCUI_HANDLE_OFFSET_FONT'
scui_font_offset_value = '0x3000 - 1'


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
    scui_font_package_h.write('/*一个通过scui_font_package脚本生成的font表\n */\n\n')
    scui_font_package_c.write('/*一个通过scui_font_package脚本生成的font表\n */\n\n')
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
    scui_font_package_h.write('\nextern const void * scui_font_package_table[%d];\n\n' % scui_font_num)
    scui_font_package_h.write('/*@brief 字库语言字号匹配\n')
    scui_font_package_h.write(' *@param lang 字库语言类型\n')
    scui_font_package_h.write(' *@param size 字库字号\n')
    scui_font_package_h.write(' *@retval 字库句柄\n */\n')
    scui_font_package_h.write('scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size);\n')
    scui_font_package_h.write('\n#endif\n')
    # 填充数据表
    scui_font_package_c.write('const void * scui_font_package_table[%d] = {\n' % scui_font_num)
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
        if font_item['lang'] == 'ascii' or font_item['lang'] == 'multi':
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
    # print重定向
    sys.stdout = ScuiRedirectPrint(sys.stdout, file=os.path.join(dst_path, 'scui_font_package.out'))   # redirect print
    sys.stderr = ScuiRedirectPrint(sys.stderr, file=os.path.join(dst_path, 'scui_font_package.err'))   # redirect print
    print('src path:', src_path)
    print('dst path:', dst_path)
    # json转Python字符串并转标准字典
    parser_path = os.path.join(dst_path, 'scui_font_package.json')
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
    scui_font_package_h = open(os.path.join(dst_path, 'scui_font_package.h'), mode='w', encoding='utf-8')
    scui_font_package_c = open(os.path.join(dst_path, 'scui_font_package.c'), mode='w', encoding='utf-8')
    scui_font_package_bin = open(os.path.join(dst_path, 'scui_font_package.bin'), mode='wb')
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


if __name__ == '__main__':
    try:
        scui_font_package()
    except Exception as e:
        print(e)
    print('scui font parser finish')
    input('请按任意键退出...')
