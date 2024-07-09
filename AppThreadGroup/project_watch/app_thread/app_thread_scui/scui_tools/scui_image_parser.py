# 图片打包文件
import re
import os
import sys
import PIL.Image
import lz4.block
import lz4.frame

# 设备像素格式
scui_pixel_cf_0 = r'palette4'
scui_pixel_cf_1 = r'bmp565'
scui_pixel_cf_2 = r'bmp8565'
# 可支持转化的像素格式表
scui_pixel_cf_list_0 = ['palette4', 'palette8']     # 纯色图,只有透明度(位深<=8)
scui_pixel_cf_list_1 = ['bmp565', 'bmp888']         # 色彩图,无透明度
scui_pixel_cf_list_2 = ['bmp8565', 'bmp8888']       # 色彩图,有透明度
# 句柄表偏移:图片描述
scui_image_offset_name = 'SCUI_HANDLE_OFFSET_IMAGE'
scui_image_offset_value = '0x2000 - 1'


# 生成pixel p4(palette)
def scui_image_pixel_p4(r8_0, g8_0, b8_0, r8_1, g8_1, b8_1) -> int:
    rgb_0 = (r8_0 + g8_0 + b8_0) / 3
    rgb_1 = (r8_1 + g8_1 + b8_1) / 3
    return int(rgb_0 / 16) + (int(rgb_1 / 16) << 4)


# 生成pixel bmp565
def scui_image_pixel_bmp565(r8, g8, b8) -> tuple:
    r5 = (r8 >> 3) & 0x1F
    g6 = (g8 >> 2) & 0x3F
    b5 = (b8 >> 3) & 0x1F
    # rgb
    rgb = (r5 << 11) | (g6 << 5) | (b5 << 0)
    # return int(rgb / 256), int(rgb % 256)   # rgb raw
    return int(rgb % 256), int(rgb / 256)    # rgb swap
    # bgr
    # bgr = (b5 << 11) | (g6 << 5) | (r5 << 0)
    # return int(bgr / 256), int(bgr % 256)   # rgb raw
    # return int(bgr % 256), int(bgr / 256)   # bgr swap


# lz4压缩
def scui_image_lz4_compress(pixel_bytes_in) -> bytearray:
    # lz4hc = ctypes.cdll.LoadLibrary(r'.\lz4hc.dll')
    # lz4hc.LZ4_compress_HC(pixel_bytes_in, pixel_bytes_out, pixel_bytes_size, pixel_bytes_size, 12)
    pixel_bytes_out = lz4.frame.compress(pixel_bytes_in, compression_level=12)
    # pixel_bytes_out = lz4.block.compress(pixel_bytes_in, mode='high_compression', compression=12, return_bytearray=True)
    return pixel_bytes_out


# lz4解压缩
def scui_image_lz4_decompress(pixel_bytes_in) -> bytearray:
    pixel_bytes_out = lz4.frame.decompress(pixel_bytes_in)
    # pixel_bytes_out = lz4.block.decompress(pixel_bytes_in, return_bytearray=True)
    return pixel_bytes_out


# 流式处理所有image文件
# 头文件保存目标数据描述表,源文件保存数据源,二进制文件保存源文件的流式版本
def scui_image_parse(file_path_list, scui_image_parser_list, project_name):
    scui_image_parser_h = scui_image_parser_list[0]
    scui_image_parser_c = scui_image_parser_list[1]
    scui_image_parser_bin = scui_image_parser_list[2]
    scui_image_parser_raw = scui_image_parser_list[3]
    scui_image_parser_mem = scui_image_parser_list[4]
    # 头文件添加前缀, 源文件添加前缀
    scui_image_parser_h.write('#ifndef SCUI_IMAGE_PARSER_H\n')
    scui_image_parser_h.write('#define SCUI_IMAGE_PARSER_H\n\n')
    scui_image_parser_h.write('/*一个通过scui_image_parser脚本生成的image表\n */\n\n')
    scui_image_parser_c.write('/*一个通过scui_image_parser脚本生成的image表\n */\n\n')
    scui_image_parser_c.write('#include \"scui.h\"\n\n')
    # offset
    pixel_ofs_raw = 0
    pixel_ofs_mem = 0
    # 填充数据表
    offset_name = scui_image_offset_name
    offset_value = scui_image_offset_value
    scui_image_parser_h.write('typedef enum {\n')
    scui_image_parser_h.write('\t%s = %s,\n' % (offset_name, offset_value))
    for file in file_path_list:
        scui_image_tag = (project_name + file).replace('.', '').replace('\\', '_')
        scui_image_parser_h.write('\tscui_image_%s,\n' % scui_image_tag)
    scui_image_parser_h.write('} scui_image_parser_handle_t;\n\n')
    scui_image_parser_h.write('extern const void * scui_image_parser_table[%d];\n\n' % len(file_path_list))
    scui_image_parser_h.write('//<%10s,%10s,%10s,%10s,%12s> handle\n' % ('w', 'h', 'size_raw', 'size_mem', 'lz4_pct'))
    # 对目标图片集合进行流式处理,提取数据内容
    for file in file_path_list:
        # 取出图片转为RGBA并获得各个通道的数据值并且转化为二维像素矩阵
        try:
            image_raw = PIL.Image.open(file)
            image_std = PIL.Image.open(file).convert('RGBA')
        except Exception as e:
            print('image parse fail :', e)
            continue
        # print(image_raw.size)       # 图片尺寸
        # print(image_raw.mode)       # 图片模式
        # print(image_raw.getbands())
        # 图片宽度应该是偶数
        if (image_raw.size[0] % 2) != 0:
            print('image %s width is odd:' % file)
            return
        # ...
        # 现在我们将所有图片都转为RGBA格式的了:image_std
        # 我们需要根据原格式提取目标数据存储到缓存中
        # 这一步骤不进行本地持久化的操作
        pixel_matrix = image_std.load()
        pixel_stream = []
        # 迭代每一个像素点
        if image_raw.mode == 'P':
            if scui_pixel_cf_0 == r'palette4':
                for j in range(image_std.size[1]):
                    for i in range(0, image_std.size[0], 2):
                        r8_0, r8_1 = pixel_matrix[i + 0, j][0], pixel_matrix[i + 1, j][0]
                        g8_0, g8_1 = pixel_matrix[i + 0, j][1], pixel_matrix[i + 1, j][1]
                        b8_0, b8_1 = pixel_matrix[i + 0, j][2], pixel_matrix[i + 1, j][2]
                        rgb8 = scui_image_pixel_p4(r8_0, g8_0, b8_0, r8_1, g8_1, b8_1)
                        pixel_stream.append(rgb8)
                # for line in pixel_stream:
                #     print(line)
        if image_raw.mode == 'RGB':
            if scui_pixel_cf_1 == r'bmp565':
                for j in range(image_std.size[1]):
                    for i in range(image_std.size[0]):
                        r8 = pixel_matrix[i, j][0]
                        g8 = pixel_matrix[i, j][1]
                        b8 = pixel_matrix[i, j][2]
                        rgb16 = scui_image_pixel_bmp565(r8, g8, b8)
                        pixel_stream.append(rgb16[0])
                        pixel_stream.append(rgb16[1])
                # for line in pixel_stream:
                #     print(line)
        if image_raw.mode == 'RGBA':
            if scui_pixel_cf_2 == r'bmp8565':
                for j in range(image_std.size[1]):
                    for i in range(image_std.size[0]):
                        r8 = pixel_matrix[i, j][0]
                        g8 = pixel_matrix[i, j][1]
                        b8 = pixel_matrix[i, j][2]
                        a8 = pixel_matrix[i, j][3]
                        rgb16 = scui_image_pixel_bmp565(r8, g8, b8)
                        pixel_stream.append(rgb16[0])
                        pixel_stream.append(rgb16[1])
                        pixel_stream.append(a8)
                # for line in pixel_stream:
                #     print(line)
        # 不可解析的数据流
        if not pixel_stream:
            print('can\'t parse data stream')
            continue
        # 计算本帧数据长度
        pixel_mem_len = len(pixel_stream)
        # lz4压缩, 生成 pixel_raw_len
        scui_image_tag = (project_name + file).replace('.', '').replace('\\', '_')
        print('lz4:' + scui_image_tag)
        pixel_bytes = bytearray(pixel_stream)
        pixel_bytes_lz4_com = scui_image_lz4_compress(pixel_bytes)
        pixel_bytes_lz4_decom = scui_image_lz4_decompress(pixel_bytes_lz4_com)
        if pixel_bytes != pixel_bytes_lz4_decom or len(pixel_bytes) != len(pixel_bytes_lz4_decom):
            print('lz4 compress decompress fail')
            continue
        scui_image_parser_bin.write(pixel_bytes_lz4_com)
        pixel_raw_len = len(pixel_bytes_lz4_com)
        # 俩个调试源文件
        scui_image_parser_mem.write(pixel_bytes)
        scui_image_parser_raw.write(pixel_bytes_lz4_com)
        # 变为C数组的字节流是不可行的,文件太大了以至于不能内联编译
        # for byte in pixel_bytes:
        #     scui_image_parser_mem.write('0x{:02x},'.format(byte).encode())
        # for byte in pixel_bytes_lz4_com:
        #     scui_image_parser_raw.write('0x{:02x},'.format(byte).encode())
        # 写入结构, lz4压缩, 更新pixel_offset
        scui_image_parser_c.write('static const scui_image_t %s = {\n' % scui_image_tag)
        scui_image_parser_c.write('\t.pixel.width\t = %s,\n' % hex(image_std.size[0]))
        scui_image_parser_c.write('\t.pixel.height\t = %s,\n' % hex(image_std.size[1]))
        scui_image_parser_c.write('\t.pixel.data_raw\t = %s,\n' % hex(pixel_ofs_raw))
        scui_image_parser_c.write('\t.pixel.data_mem\t = %s,\n' % hex(pixel_ofs_mem))
        scui_image_parser_c.write('\t.pixel.size_raw\t = %s,\n' % hex(pixel_raw_len))
        scui_image_parser_c.write('\t.pixel.size_mem\t = %s,\n' % hex(pixel_mem_len))
        scui_image_parser_c.write('\t.status\t\t\t = %s,\n' % 'scui_image_status_lz4')
        if image_raw.mode == 'P':
            if scui_pixel_cf_0 == r'palette4':
                scui_image_parser_c.write('\t.format\t\t\t = %s,\n' % 'scui_image_cf_palette4')
        if image_raw.mode == 'RGB':
            if scui_pixel_cf_1 == r'bmp565':
                scui_image_parser_c.write('\t.format\t\t\t = %s,\n' % 'scui_image_cf_bmp565')
        if image_raw.mode == 'RGBA':
            if scui_pixel_cf_2 == r'bmp8565':
                scui_image_parser_c.write('\t.format\t\t\t = %s,\n' % 'scui_image_cf_bmp8565')
        scui_image_parser_c.write('};\n\n')
        # 信息记录
        scui_image_parser_h.write('//<%10s,%10s,%10s,%10s,%12.2f> scui_image_%s\n' %
                                   (hex(image_std.size[0]), hex(image_std.size[1]), pixel_raw_len,
                                    hex(pixel_mem_len), float(pixel_raw_len) / float(pixel_mem_len),
                                    scui_image_tag))
        pixel_ofs_raw += pixel_raw_len
        pixel_ofs_mem += pixel_mem_len
    scui_image_parser_h.write('\n#endif\n')
    # 填充数据表
    scui_image_parser_c.write('const void * scui_image_parser_table[%d] = {\n' % len(file_path_list))
    for file in file_path_list:
        scui_image_tag = (project_name + file).replace('.', '').replace('\\', '_')
        scui_image_parser_c.write('\t(void *)&%s,\n' % scui_image_tag)
    scui_image_parser_c.write('};\n')


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
            if not re.findall(r'#', os.path.join(path, item)):
                scui_image_collect(file_path_list, file_ext_list, os.path.join(path, item))


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
def scui_image_parser():
    # 解析支持检查
    if scui_pixel_cf_0 != r'palette4':
        print('unsupport pixel format yet:', scui_pixel_cf_0)
        return
    if scui_pixel_cf_1 != r'bmp565':
        print('unsupport pixel format yet:', scui_pixel_cf_1)
        return
    if scui_pixel_cf_2 != r'bmp8565':
        print('unsupport pixel format yet:', scui_pixel_cf_2)
        return
    # 参数列表:src相对路径 dst相对路径 项目名称
    if len(sys.argv) != 4:
        print('argv list not match')
        return
    src_path = sys.argv[1]
    dst_path = sys.argv[2]
    project_name = sys.argv[3]
    # 获得文件处理src相对路径
    if not os.path.exists(src_path):
        print('src path is not exist')
        return
    # 获得文件处理dst相对路径
    if not os.path.exists(dst_path):
        print('dst path is not exist')
        return
    # print重定向
    sys.stdout = ScuiRedirectPrint(sys.stdout, file=os.path.join(dst_path, 'scui_image_parser.out'))   # redirect print
    sys.stderr = ScuiRedirectPrint(sys.stderr, file=os.path.join(dst_path, 'scui_image_parser.err'))  # redirect print
    print('path:', src_path)
    print('path:', dst_path)
    # 遍历整个文件夹,获取指定扩展名的文件
    file_ext_list = ['.bmp', '.png']
    file_path_list = []
    scui_image_collect(file_path_list, file_ext_list, src_path)
    # check:
    # for item in file_path_list:
    #     print(item)
    # 核查文件支持
    scui_image_parser_h = open(os.path.join(dst_path, 'scui_image_parser.h'), mode='w', encoding='utf-8')
    scui_image_parser_c = open(os.path.join(dst_path, 'scui_image_parser.c'), mode='w', encoding='utf-8')
    scui_image_parser_bin = open(os.path.join(dst_path, 'scui_image_parser.bin'), mode='wb')
    scui_image_parser_raw = open(os.path.join(dst_path, 'scui_image_parser.raw'), mode='wb')
    scui_image_parser_mem = open(os.path.join(dst_path, 'scui_image_parser.mem'), mode='wb')
    scui_image_parser_list = [
        scui_image_parser_h,
        scui_image_parser_c,
        scui_image_parser_bin,
        scui_image_parser_raw,
        scui_image_parser_mem]
    scui_image_parse(file_path_list, scui_image_parser_list, project_name)
    scui_image_parser_h.close()
    scui_image_parser_c.close()
    scui_image_parser_bin.close()
    scui_image_parser_raw.close()
    scui_image_parser_mem.close()


if __name__ == '__main__':
    try:
        scui_image_parser()
    except Exception as e:
        print(e)
    print('scui image parser finish')
    input('请按任意键退出...')
