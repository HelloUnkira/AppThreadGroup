# 图片打包文件
import re
import os
import sys
import PIL.Image
import lz4.frame

# 设备像素格式
scui_pixel_format_0 = r'a4'
scui_pixel_format_1 = r'bmp565'
scui_pixel_format_2 = r'png8565'
# 可支持转化的像素格式表
scui_pixel_format_list_0 = ['a4', 'a8']  # 纯色图,只有透明度(位深<=8)
scui_pixel_format_list_1 = ['rgb565', 'rgb888']  # 色彩图,无透明度()
scui_pixel_format_list_2 = ['argb8565', 'argb8888']  # 色彩图,有透明度


# 生成pixel a4
def scui_image_pixel_a4(r8_0, g8_0, b8_0, r8_1, g8_1, b8_1) -> int:
    rgb_0 = (r8_0 + g8_0 + b8_0) / 3
    rgb_1 = (r8_1 + g8_1 + b8_1) / 3
    return int(rgb_0 / 16) + (int(rgb_1 / 16) << 4)


# 生成pixel bmp565
def scui_image_pixel_bmp565(r8, g8, b8) -> tuple:
    # rgb
    rgb = (r8 << 8) | (g8 << 3) | (b8 >> 3)
    return int(rgb / 256), int(rgb % 256)   # rgb raw
    # return int(rgb % 256, int(rgb / 256)    # rgb swap
    # bgr
    # bgr = (b8 << 8) | (g8 << 3) | (r8 >> 3)
    # return int(bgr / 256), int(bgr % 256)   # rgb raw
    # return int(bgr % 256), int(bgr / 256)   # bgr swap


# lz4压缩解压缩
def scui_image_lz4_compress(pixel_bytes_in) -> bytearray:
    pixel_bytes_out = lz4.frame.compress(pixel_bytes_in, compression_level=12)
    # lz4hc = ctypes.cdll.LoadLibrary(r".\lz4hc.dll")
    # lz4hc.LZ4_compress_HC(pixel_bytes_in, pixel_bytes_out, pixel_bytes_size, pixel_bytes_size, 12)
    return pixel_bytes_out


def scui_image_lz4_decompress(pixel_bytes_in) -> bytearray:
    pixel_bytes_out = lz4.frame.decompress(pixel_bytes_in)
    return pixel_bytes_out


# 流式处理所有image文件
# 头文件保存目标数据描述表,源文件保存数据源,二进制文件保存源文件的流式版本
def scui_image_parse(file_path_list, scui_image_combine_list, project_name):
    scui_image_combine_h = scui_image_combine_list[0]
    scui_image_combine_c = scui_image_combine_list[1]
    scui_image_combine_bin = scui_image_combine_list[2]
    # 头文件添加前缀, 源文件添加前缀
    scui_image_combine_h.write('#ifndef SCUI_IMAGE_COMBINE_H\n')
    scui_image_combine_h.write('#define SCUI_IMAGE_COMBINE_H\n\n')
    scui_image_combine_h.write('/*一个通过scui_image_combine脚本生成的image表\n */\n\n')
    scui_image_combine_c.write('/*一个通过scui_image_combine脚本生成的image表\n */\n\n')
    scui_image_combine_c.write('#include \"app_ext_lib.h\"\n')
    scui_image_combine_c.write('#include \"app_sys_lib.h\"\n')
    scui_image_combine_c.write('#include \"scui.h\"\n\n')
    # offset
    pixel_offset = 0
    # 填充数据表
    scui_image_combine_h.write('typedef enum {\n')
    for file in file_path_list:
        scui_image_tag = project_name + file.replace('.', '_').replace('\\', '_')
        scui_image_combine_h.write('\tscui_image_%s,\n' % scui_image_tag)
    scui_image_combine_h.write('} scui_image_combine_handle_t;\n\n')
    scui_image_combine_h.write('extern const void * scui_image_combine_table[%d];\n\n' % len(file_path_list))
    scui_image_combine_h.write('//<%10s,%10s,%10s,%10s,%12s> handle\n' % ('w', 'h', 'size_raw', 'size_mem', 'lz4_pct'))
    # 对目标图片集合进行流式处理,提取数据内容
    for file in file_path_list:
        # 取出图片转为RGBA并获得各个通道的数据值并且转化为二维像素矩阵
        image_raw = PIL.Image.open(file)
        image_std = PIL.Image.open(file).convert('RGBA')
        # print(image_raw.size)       # 图片尺寸
        # print(image_raw.mode)       # 图片模式
        # print(image_raw.getbands())
        # 图片宽度应该是偶数
        if (image_raw.size[0] % 2) != 0:
            print('image %s width is odd:' % file)
            return
        # 写入结构
        scui_image_tag = project_name + file.replace('.', '_').replace('\\', '_')
        scui_image_combine_c.write('static const scui_image_t %s = {\n' % scui_image_tag)
        # 现在我们将所有图片都转为RGBA格式的了:image_std
        # 我们需要根据原格式提取目标数据存储到缓存中
        # 这一步骤不进行本地持久化的操作
        pixel_matrix = image_std.load()
        pixel_stream = []
        # 迭代每一个像素点
        if image_raw.mode == 'P':
            if scui_pixel_format_0 == r'a4':
                scui_image_combine_c.write('\t.format\t\t\t = %s,\n' % 'scui_image_format_a4')
                for i in range(image_std.size[0]):
                    for j in range(0, image_std.size[1], 2):
                        r8_0, r8_1 = pixel_matrix[i, j + 0][0], pixel_matrix[i, j + 1][0]
                        g8_0, g8_1 = pixel_matrix[i, j + 0][1], pixel_matrix[i, j + 1][1]
                        b8_0, b8_1 = pixel_matrix[i, j + 0][2], pixel_matrix[i, j + 1][2]
                        rgb8 = scui_image_pixel_a4(r8_0, g8_0, b8_0, r8_1, g8_1, b8_1)
                        pixel_stream.append(rgb8)
                # for line in pixel_stream:
                #     print(line)
        if image_raw.mode == 'RGB':
            if scui_pixel_format_1 == r'bmp565':
                scui_image_combine_c.write('\t.format\t\t\t = %s,\n' % 'scui_image_format_bmp565')
                for i in range(image_std.size[0]):
                    for j in range(0, image_std.size[1]):
                        r8 = pixel_matrix[i, j][0]
                        g8 = pixel_matrix[i, j][1]
                        b8 = pixel_matrix[i, j][2]
                        bmp16 = scui_image_pixel_bmp565(r8, g8, b8)
                        pixel_stream.append(bmp16[0])
                        pixel_stream.append(bmp16[1])
                # for line in pixel_stream:
                #     print(line)
        if image_raw.mode == 'RGBA':
            if scui_pixel_format_2 == r'png8565':
                scui_image_combine_c.write('\t.format\t\t\t = %s,\n' % 'scui_image_format_png8565')
                for i in range(image_std.size[0]):
                    for j in range(0, image_std.size[1]):
                        r8 = pixel_matrix[i, j][0]
                        g8 = pixel_matrix[i, j][1]
                        b8 = pixel_matrix[i, j][2]
                        a8 = pixel_matrix[i, j][3]
                        bmp16 = scui_image_pixel_bmp565(r8, g8, b8)
                        pixel_stream.append(bmp16[0])
                        pixel_stream.append(bmp16[1])
                        pixel_stream.append(a8)
                # for line in pixel_stream:
                #     print(line)
        # 不可解析的数据流
        if not pixel_stream:
            print('can\'t parse data stream')
            continue
        # 计算本帧数据长度
        pixel_mem_len = len(pixel_stream)
        # ......
        # 写入结构
        scui_image_combine_c.write('\t.pixel.data\t\t = %s,\n' % hex(pixel_offset))
        scui_image_combine_c.write('\t.pixel.width\t = %s,\n' % hex(image_std.size[0]))
        scui_image_combine_c.write('\t.pixel.height\t = %s,\n' % hex(image_std.size[1]))
        scui_image_combine_c.write('\t.pixel.size_mem\t = %s,\n' % hex(pixel_mem_len))
        print(' lz4:' + scui_image_tag)
        pixel_bytes = bytearray(pixel_stream)
        pixel_bytes_lz4_com = scui_image_lz4_compress(pixel_bytes)
        pixel_bytes_lz4_decom = scui_image_lz4_decompress(pixel_bytes_lz4_com)
        if pixel_bytes != pixel_bytes_lz4_decom or len(pixel_bytes) != len(pixel_bytes_lz4_decom):
            print('lz4 compress decompress fail')
            continue
        scui_image_combine_bin.write(pixel_bytes_lz4_com)
        pixel_raw_len = len(pixel_bytes_lz4_com)
        # lz4压缩, 更新pixel_length
        scui_image_combine_c.write('\t.pixel.size_raw\t = %s,\n' % hex(pixel_raw_len))
        scui_image_combine_c.write('\t.status\t\t\t = %s,\n' % 'scui_image_status_lz4')
        scui_image_combine_c.write('\t.from\t\t\t = %s,\n' % 'scui_image_from_ext')
        scui_image_combine_c.write('};\n\n')
        # 信息记录
        scui_image_tag = project_name + file.replace('.', '_').replace('\\', '_')
        scui_image_combine_h.write('//<%10s,%10s,%10s,%10s,%12.2f> scui_image_%s\n' %
                                   (hex(image_std.size[0]), hex(image_std.size[1]), pixel_raw_len,
                                    hex(pixel_mem_len), float(pixel_raw_len) / float(pixel_mem_len),
                                    scui_image_tag))
        pixel_offset += pixel_raw_len
    scui_image_combine_h.write('\n#endif\n')
    # 填充数据表
    scui_image_combine_c.write('const void * scui_image_combine_table[%d] = {\n' % len(file_path_list))
    for file in file_path_list:
        scui_image_tag = project_name + file.replace('.', '_').replace('\\', '_')
        scui_image_combine_c.write('\t(void *)&%s,\n' % scui_image_tag)
    scui_image_combine_c.write('};\n')
    pass


# 遍历整个文件夹,提取目标文件
def scui_image_collect(file_path_list, file_ext_list, path):
    if os.path.isfile(path):
        # 获取指定扩展名的文件
        if os.path.splitext(path)[1] in file_ext_list:
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', path):
                file_path_list.append(path)
    if os.path.isdir(path):
        for item in os.listdir(path):
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', path + '\\' + item):
                scui_image_collect(file_path_list, file_ext_list, path + '\\' + item)


def scui_image_combine():
    # 解析支持检查
    if scui_pixel_format_0 != r'a4':
        print('unsupport pixel format yet:', scui_pixel_format_0)
        return
    if scui_pixel_format_1 != r'bmp565':
        print('unsupport pixel format yet:', scui_pixel_format_1)
        return
    if scui_pixel_format_2 != r'png8565':
        print('unsupport pixel format yet:', scui_pixel_format_2)
        return
    # 参数列表:文件根目录, 设备pixel格式(不带alpha通道, 带alpha通道), 项目名称
    if len(sys.argv) != 3:
        print('argv list not match')
        return
    root_path = sys.argv[1]
    project_name = sys.argv[2]
    # 获得文件处理根路径
    if not os.path.exists(root_path):
        print('path is not exist')
        return
    print('path:', root_path)
    # 遍历整个文件夹,获取指定扩展名的文件
    file_ext_list = ['.bmp', '.png']
    file_path_list = []
    scui_image_collect(file_path_list, file_ext_list, root_path)
    # check:
    # for item in file_path_list:
    #     print(item)
    # 核查文件支持
    scui_image_combine_h = open('scui_image_combine.h', mode='w', encoding='utf-8')
    scui_image_combine_c = open('scui_image_combine.c', mode='w', encoding='utf-8')
    scui_image_combine_bin = open('scui_image_combine.bin', mode='wb')
    scui_image_combine_list = [scui_image_combine_h, scui_image_combine_c, scui_image_combine_bin]
    scui_image_parse(file_path_list, scui_image_combine_list, project_name)
    scui_image_combine_h.close()
    scui_image_combine_c.close()
    scui_image_combine_bin.close()


if __name__ == '__main__':
    try:
        scui_image_combine()
    except Exception as e:
        print(e)
    print('scui image combine finish')
    input('请按任意键退出...')
