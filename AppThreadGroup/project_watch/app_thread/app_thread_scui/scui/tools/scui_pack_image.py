# 图片打包文件
import re
import os
import sys
import json
import PIL.Image
import lz4.block
import lz4.frame
import numpy
from concurrent.futures import ThreadPoolExecutor

# 支持的压缩目标
scui_image_pkg_use_lz4 = True
scui_image_pkg_use_jpg = True
scui_image_pkg_use_png = True
# 并行编码线程数(上限6): 各图独立编码, 再顺序装订, 保持输出字节一致
scui_image_pkg_workers = 6
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
# 图形配置: 前端统一配置(scui_pack_tools.json)键->本模块变量
_IMG_CFG_KEYS = {
    'use_lz4':    'scui_image_pkg_use_lz4',
    'use_jpg':    'scui_image_pkg_use_jpg',
    'use_png':    'scui_image_pkg_use_png',
    'workers':    'scui_image_pkg_workers',
    'alpha_bits': 'scui_image_alpha_bits',
    'index_bits': 'scui_image_index_bits',
    'endian':     'scui_image_endian',
}
def _load_image_cfg():
    """brief: 读取统一配置覆盖头部默认
    @retval 无
    """
    try:
        cfg = os.path.join(SCUI_TOOLS or os.path.dirname(__file__), 'scui_pack_tools.json')
        j = json.load(open(cfg, encoding='utf-8')) if os.path.isfile(cfg) else {}
    except Exception:
        j = {}
    for k, varname in _IMG_CFG_KEYS.items():
        if k in j.get('image_config', {}):
            globals()[varname] = j['image_config'][k]
# 像素位宽配置(全局统一,不混用)
scui_image_alpha_bits = 4       # alpha位宽(1/2/4/8)
scui_image_index_bits = 8       # index位宽(1/2/4/8)
scui_image_endian    = False    # 整体字节序(True=大端, False=小端): bmp565/8565/index共用
# 句柄表偏移:图片描述
scui_image_offset_name = 'SCUI_HANDLE_OFFSET_IMAGE'
scui_image_offset_value = '0x2000 - 1'


# 生成pixel px(grey,按位宽合并)
def scui_image_pixel_px(grey_list, bits) -> int:
    pixel_per_byte = 8 // bits
    mask = (1 << bits) - 1
    byte = 0
    for idx in range(pixel_per_byte):
        if idx < len(grey_list):
            byte |= (grey_list[idx] & mask) << (8 - bits - idx * bits)
    return byte


# 生成pixel p4(grey)
def scui_image_pixel_p4(r8_0, g8_0, b8_0, r8_1, g8_1, b8_1) -> int:
    rgb_0 = (r8_0 + g8_0 + b8_0) / 3
    rgb_1 = (r8_1 + g8_1 + b8_1) / 3
    return (int(rgb_0 / 16) << 4) + int(rgb_1 / 16)


# 索引图量化:统计rgba出现次数,取前color_num色为调色板
def scui_image_pixel_index_palette(image, color_num) -> list:
    pixel_matrix = image.load()
    width, height = image.size
    color_count = {}
    for y in range(height):
        for x in range(width):
            r8, g8, b8, a8 = pixel_matrix[x, y]
            key = (r8, g8, b8, a8)
            color_count[key] = color_count.get(key, 0) + 1
    # 按出现次数从高到低排序,取前color_num色,不足用黑色不透明补齐
    color_sorted = sorted(color_count.items(), key=lambda item: item[1], reverse=True)
    palette = [key for key, cnt in color_sorted[:color_num]]
    while len(palette) < color_num:
        palette.append((0, 0, 0, 0xFF))
    return palette


# 索引图量化:像素取最近色索引,按位宽合并为像素流
def scui_image_pixel_index(image, palette, bits, endian) -> int:
    pixel_matrix = image.load()
    width, height = image.size
    pixel_stream = []
    # 调色板表(8565,每项3字节:A+R5G6B5,字节序由endian控制)
    for pr, pg, pb, pa in palette:
        r5 = (pr >> 3) & 0x1F
        g6 = (pg >> 2) & 0x3F
        b5 = (pb >> 3) & 0x1F
        rgb8565 = (pa << 16) | (r5 << 11) | (g6 << 5) | (b5 << 0)
        if endian:
            pixel_stream.append((rgb8565 >> 16) & 0xFF)
            pixel_stream.append((rgb8565 >> 8) & 0xFF)
            pixel_stream.append(rgb8565 & 0xFF)
        else:
            pixel_stream.append(rgb8565 & 0xFF)
            pixel_stream.append((rgb8565 >> 8) & 0xFF)
            pixel_stream.append((rgb8565 >> 16) & 0xFF)
    # 最近色索引
    index_stream = []
    for y in range(height):
        for x in range(width):
            r8, g8, b8, a8 = pixel_matrix[x, y]
            best_idx = 0
            best_dist = 0x7FFFFFFF
            for idx, (pr, pg, pb, pa) in enumerate(palette):
                dr, dg, db, da = r8 - pr, g8 - pg, b8 - pb, a8 - pa
                dist = dr * dr + dg * dg + db * db + da * da
                if dist < best_dist:
                    best_dist = dist
                    best_idx = idx
            index_stream.append(best_idx)
    # 索引像素流(1/2/4/8 bpp,高位在前)
    mask = (1 << bits) - 1
    if bits == 8:
        pixel_stream.extend(index_stream)
    else:
        pixel_per_byte = 8 // bits
        for idx in range(0, len(index_stream), pixel_per_byte):
            byte = 0
            for bit in range(pixel_per_byte):
                if idx + bit < len(index_stream):
                    byte |= (index_stream[idx + bit] & mask) << (8 - bits - bit * bits)
            pixel_stream.append(byte)
    return pixel_stream


# 生成pixel bmp565(字节序由endian控制: True=大端高字节在前)
def scui_image_pixel_bmp565(r8, g8, b8, endian=False) -> tuple:
    r5 = (r8 >> 3) & 0x1F
    g6 = (g8 >> 2) & 0x3F
    b5 = (b8 >> 3) & 0x1F
    # rgb
    rgb = (r5 << 11) | (g6 << 5) | (b5 << 0)
    # 小端: 低字节在前(默认)
    if endian:
        return int(rgb / 256), int(rgb % 256)    # 大端: 高字节在前
    return int(rgb % 256), int(rgb / 256)


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


# 对常规数据流进行dither处理(未完成, 未测试)
def scui_image_pixel_dither(image, dither):
    pixel_matrix = image.load()
    if not dither:
        return pixel_matrix
    # 提取RGBA数据到NumPy数组
    width, height = image.size
    pixel_rgba = numpy.zeros((height, width, 4), dtype=numpy.float32)
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixel_matrix[x, y]
            pixel_rgba[y, x] = [r, g, b, a]
    # 对RGB通道进行dither(Alpha通道保持不变)
    pixel_a = pixel_rgba[:, :, 3].copy()
    pixel_rgb = pixel_rgba[:, :, :3].copy()
    pixel_rgb = pixel_rgb.copy().astype(numpy.float32) # 使用float存储误差
    height, width, channels = pixel_rgb.shape
    for y in range(height):
        for x in range(width):
            for c in range(channels):  # R, G, B
                # 随机抖动
                channel_old = pixel_rgb[y, x, c]
                channel_new = channel_old + numpy.random.uniform(-15, 15)
                channel_new = numpy.clip(channel_new, 0, 255)
                pixel_rgb[y, x, c] = channel_new
                # 计算误差并扩散
                spread_error = channel_old - channel_new
                if x + 1 < width: pixel_rgb[y, x + 1, c] += spread_error * 7 / 16
                if y + 1 < height: pixel_rgb[y + 1, x, c] += spread_error * 5 / 16
                if y + 1 < height and x > 0: pixel_rgb[y + 1, x - 1, c] += spread_error * 3 / 16
                if y + 1 < height and x + 1 < width: pixel_rgb[y + 1, x + 1, c] += spread_error * 1 / 16
                # 扩散后的值不溢出
                pixel_rgb[:, :, c] = numpy.clip(pixel_rgb[:, :, c], 0, 255)
    # 整理所有的通道值, 然后替换RGB通道值
    pixel_matrix = numpy.zeros((height, width, 4), dtype=numpy.uint8)
    pixel_matrix[:, :, :3] = pixel_rgb.clip(0, 255).astype(numpy.uint8)
    pixel_matrix[:, :, 3] = pixel_a.astype(numpy.uint8)
    return PIL.Image.fromarray(pixel_matrix, 'RGBA').load()


# 生成常规数据流
def scui_image_pixel_stream(image_raw, image_std, dither, tag_index=False) -> ():
    # ...
    # 现在我们将剩下的图片都转为RGBA格式的了:image_std
    # 我们需要根据原格式提取目标数据存储到缓存中
    # 这一步骤不进行本地持久化的操作
    # 针对被标记为dither的image进行处理
    pixel_matrix = scui_image_pixel_dither(image_std, dither)
    pixel_stream = []
    # 索引量化:[[颜色表][索引图]]
    if tag_index:
        image_std = image_std.convert('RGBA')
        index_bits = scui_image_index_bits
        color_num = 1 << index_bits
        palette = scui_image_pixel_index_palette(image_std, color_num)
        pixel_stream = scui_image_pixel_index(image_std, palette, index_bits, scui_image_endian)
        scui_pixel_cf = 'scui_pixel_cf_index%d' % index_bits
        return pixel_stream, scui_pixel_cf
    # 迭代每一个像素点
    if image_raw.mode == 'P':
        alpha_bits = scui_image_alpha_bits
        pixel_per_byte = 8 // alpha_bits
        grey_max = (1 << alpha_bits) - 1
        scui_pixel_cf = 'scui_pixel_cf_alpha%d' % alpha_bits
        for j in range(image_std.size[1]):
            for i in range(0, image_std.size[0], pixel_per_byte):
                grey_list = []
                for k in range(pixel_per_byte):
                    if i + k < image_std.size[0]:
                        r8 = pixel_matrix[i + k, j][0]
                        g8 = pixel_matrix[i + k, j][1]
                        b8 = pixel_matrix[i + k, j][2]
                        grey_list.append(int((r8 + g8 + b8) / 3) * grey_max // 255)
                pixel_stream.append(scui_image_pixel_px(grey_list, alpha_bits))
        # for line in pixel_stream:
        #     print(line)
    if image_raw.mode == 'RGB':
        scui_pixel_cf = 'scui_pixel_cf_bmp565'
        for j in range(image_std.size[1]):
            for i in range(image_std.size[0]):
                r8 = pixel_matrix[i, j][0]
                g8 = pixel_matrix[i, j][1]
                b8 = pixel_matrix[i, j][2]
                rgb16 = scui_image_pixel_bmp565(r8, g8, b8, scui_image_endian)
                pixel_stream.append(rgb16[0])
                pixel_stream.append(rgb16[1])
        # for line in pixel_stream:
        #     print(line)
    if image_raw.mode == 'RGBA':
        scui_pixel_cf = 'scui_pixel_cf_bmp8565'
        for j in range(image_std.size[1]):
            for i in range(image_std.size[0]):
                r8 = pixel_matrix[i, j][0]
                g8 = pixel_matrix[i, j][1]
                b8 = pixel_matrix[i, j][2]
                a8 = pixel_matrix[i, j][3]
                rgb16 = scui_image_pixel_bmp565(r8, g8, b8, scui_image_endian)
                pixel_stream.append(rgb16[0])
                pixel_stream.append(rgb16[1])
                pixel_stream.append(a8)
        # for line in pixel_stream:
        #     print(line)
    return pixel_stream, scui_pixel_cf


# 图片宽度应为偶数, 奇数宽度跳过(索引/位操作需要偶数宽度)
def scui_image_width_even(file):
    # 非图片格式(lottie.json/mp4等)无宽度概念, 直接放行
    if not file.lower().endswith(('.bmp', '.jpg', '.jpeg', '.png', '.gif')):
        return True
    try:
        import PIL.Image as Image
        image_raw = Image.open(file)
        even = (image_raw.size[0] % 2) == 0
        image_raw.close()
        return even
    except Exception:
        return False


# 单图编码(并行worker, 纯内存, 不写共享文件): 返回本帧打包数据, 失败返回None
def _scui_image_encode(file, src_path, project_name):
    # 定制尾缀激活(非子文件夹): foo.idx / foo.dit 缀于扩展名前
    scui_image_tag_dither = os.path.splitext(os.path.basename(file))[0].lower().endswith('.dit')
    scui_image_tag_index  = os.path.splitext(os.path.basename(file))[0].lower().endswith('.idx')
    scui_image_tag_frame = False
    scui_image_pkg_over = False
    # 去除src根目录段(image_src/), 保留实际资源子路径, 缩短枚举长度
    file_short = os.path.relpath(file, src_path).replace('\\', '/')
    scui_image_tag = (project_name + '_' + file_short).replace('.', '').replace('\\', '_').replace('/', '_').replace(' ', '_')
    scui_image_byte = 0
    scui_image_type = 'scui_image_type_bmp'
    scui_pixel_cf = 'scui_pixel_cf_bmp565'
    pixel_raw_len = 0
    image_raw = image_std = None
    # 通用压缩协议(vedio, gif)
    if file.endswith('.gif'):
        scui_pixel_cf = 'scui_pixel_cf_none'
        scui_image_type = 'scui_image_type_gif'
        scui_image_pkg_over = True
        scui_image_tag_frame = True
        # 直接原模原样的copy即可
        with open(file, mode='rb') as file_raw:
            scui_image_byte = file_raw.read()
            pixel_raw_len = len(scui_image_byte)
        print('gif:' + scui_image_tag)
    # 通用压缩协议(vedio, lottie)
    if file.endswith('.lottie.json'):
        scui_pixel_cf = 'scui_pixel_cf_none'
        scui_image_type = 'scui_image_type_lottie'
        scui_image_pkg_over = True
        scui_image_tag_frame = True
        # 直接原模原样的copy即可
        with open(file, mode='rb') as file_raw:
            scui_image_byte = file_raw.read()
            pixel_raw_len = len(scui_image_byte)
        print('lottie:' + scui_image_tag)
    # 通用压缩协议(vedio, mp4)
    if file.endswith('.mp4'):
        scui_pixel_cf = 'scui_pixel_cf_none'
        scui_image_type = 'scui_image_type_mp4'
        scui_image_pkg_over = True
        scui_image_tag_frame = True
        # 直接原模原样的copy即可
        with open(file, mode='rb') as file_raw:
            scui_image_byte = file_raw.read()
            pixel_raw_len = len(scui_image_byte)
        print('mp4:' + scui_image_tag)
    # 这里记录源文件的俩个关键信息,以用于后续的生成子文件
    # 取出图片转为RGBA并获得各个通道的数据值并且转化为二维像素矩阵
    if not scui_image_pkg_over:
        try:
            image_raw = PIL.Image.open(file)
            image_std = PIL.Image.open(file).convert('RGBA')
        except Exception as e:
            print('image parse fail :', e)
            return None
        # print(image_raw.size)       # 图片尺寸
        # print(image_raw.mode)       # 图片模式
        # print(image_raw.getbands())
        # 图片宽度应该是偶数
        if (image_raw.size[0] % 2) != 0:
            print('image %s width is odd:' % file)
            return None
    # 自定义打包格式(索引量化优先)
    if scui_image_tag_index:
        pixel_stream, scui_pixel_cf = scui_image_pixel_stream(
            image_raw, image_std, scui_image_tag_dither, scui_image_tag_index)
        if not pixel_stream:
            print('can\'t parse data stream')
            return None
        scui_image_pkg_over = True
        # 计算本帧数据长度
        pixel_bytes = bytearray(pixel_stream)
        # lz4压缩
        if scui_image_pkg_use_lz4:
            scui_image_type = 'scui_image_type_lz4'
            print('lz4:' + scui_image_tag)
            pixel_bytes_lz4_com = scui_image_lz4_compress(pixel_bytes)
            pixel_bytes_lz4_decom = scui_image_lz4_decompress(pixel_bytes_lz4_com)
            if pixel_bytes != pixel_bytes_lz4_decom or len(pixel_bytes) != len(pixel_bytes_lz4_decom):
                print('lz4 compress decompress fail')
                return None
            pixel_raw_len = len(pixel_bytes)
            scui_image_byte = pixel_bytes_lz4_com
        else:
            scui_image_type = 'scui_image_type_idx'
            print('raw:' + scui_image_tag)
            pixel_raw_len = len(pixel_bytes)
            scui_image_byte = pixel_bytes
    # 通用压缩协议
    if not scui_image_pkg_over:
        if scui_image_pkg_use_jpg and (file.endswith('.jpg') or file.endswith('.jpeg')):
            scui_pixel_cf = 'scui_pixel_cf_bmp565'
            scui_image_type = 'scui_image_type_jpg'
            scui_image_pkg_over = True
            # 直接原模原样的copy即可
            with open(file, mode='rb') as file_raw:
                scui_image_byte = file_raw.read()
                pixel_raw_len = image_std.size[0] * image_std.size[1] * 2
            print('jpg:' + scui_image_tag)
    # 通用压缩协议
    if not scui_image_pkg_over:
        if scui_image_pkg_use_png and file.endswith('.png'):
            scui_pixel_cf = 'scui_pixel_cf_bmp8565'
            scui_image_type = 'scui_image_type_png'
            scui_image_pkg_over = True
            # 直接原模原样的copy即可
            with open(file, mode='rb') as file_raw:
                scui_image_byte = file_raw.read()
                pixel_raw_len = image_std.size[0] * image_std.size[1] * 3
            print('png:' + scui_image_tag)
    # 自定义打包格式(dither等)
    if not scui_image_pkg_over:
        pixel_stream, scui_pixel_cf = scui_image_pixel_stream(
            image_raw, image_std, scui_image_tag_dither, scui_image_tag_index)
        # 不可解析的数据流
        if not pixel_stream:
            print('can\'t parse data stream')
            return None
        # 计算本帧数据长度
        pixel_bytes = bytearray(pixel_stream)
        # lz4压缩
        if scui_image_pkg_use_lz4:
            scui_image_type = 'scui_image_type_lz4'
            print('lz4:' + scui_image_tag)
            pixel_bytes_lz4_com = scui_image_lz4_compress(pixel_bytes)
            pixel_bytes_lz4_decom = scui_image_lz4_decompress(pixel_bytes_lz4_com)
            if pixel_bytes != pixel_bytes_lz4_decom or len(pixel_bytes) != len(pixel_bytes_lz4_decom):
                print('lz4 compress decompress fail')
                return None
            pixel_raw_len = len(pixel_bytes)
            scui_image_byte = pixel_bytes_lz4_com
        else:
            print('raw:' + scui_image_tag)
            pixel_raw_len = len(pixel_bytes)
            scui_image_byte = pixel_bytes
    # 参数计算
    scui_image_pixel_width = 0
    scui_image_pixel_height = 0
    if not scui_image_tag_frame:
        scui_image_pixel_width = image_std.size[0]
        scui_image_pixel_height = image_std.size[1]
    # 子记录内容(hex字节数据流, 不含struct/偏移, 由装订方补齐)
    if not scui_image_tag_frame:
        scui_image_width = image_std.size[0]
    else:
        scui_image_width = 100
    sub_body = ['\nconst uint8_t scui_image_array[] = {\n\t']
    scui_image_byte_cnt = 0
    for byte in scui_image_byte:
        sub_body.append('0x{:02x}, '.format(byte))
        scui_image_byte_cnt += 1
        if scui_image_byte_cnt % scui_image_width == 0:
            sub_body.append('\n\t')
    sub_body.append('\n\n};\n\n')
    if image_raw is not None:
        image_raw.close()
    if image_std is not None:
        image_std.close()
    return {
        'tag':      scui_image_tag,
        'cf':       scui_pixel_cf,
        'type':     scui_image_type,
        'width':    scui_image_pixel_width,
        'height':   scui_image_pixel_height,
        'bin':      scui_image_byte,
        'size_bin': len(scui_image_byte),
        'size_raw': pixel_raw_len,
        'sub_name': scui_image_tag.replace('/', '_'),
        'sub_body': sub_body,
    }


# 流式处理所有image文件
# 头文件保存目标数据描述表,源文件保存数据源,二进制文件保存源文件的流式版本
# 并行: 各图独立编码(解码/逐像素/lz4)再顺序装订, 偏移按顺序累加, 保持输出字节一致
def scui_image_parser_all(file_path_list, scui_image_parser_list, project_name, src_path='.'):
    scui_image_parser_h = scui_image_parser_list[0]
    scui_image_parser_c = scui_image_parser_list[1]
    scui_image_parser_bin = scui_image_parser_list[2]
    scui_image_parser_sub = scui_image_parser_list[3]
    # 检查子路径是否存在,不存在则创建它
    os.makedirs(scui_image_parser_sub, exist_ok=True)
    # 过滤宽度为奇数的图片(需要偶数宽度), 枚举/数据表/数据处理统一跳过
    file_path_list = [f for f in file_path_list if scui_image_width_even(f)]
    # 头文件添加前缀, 源文件添加前缀
    scui_image_parser_h.write('#ifndef SCUI_IMAGE_PARSER_H\n')
    scui_image_parser_h.write('#define SCUI_IMAGE_PARSER_H\n\n')
    scui_image_parser_h.write('/*一个通过scui_image_parser脚本生成的image表\n */\n\n')
    scui_image_parser_c.write('/*一个通过scui_image_parser脚本生成的image表\n */\n\n')
    scui_image_parser_c.write('#include \"scui.h\"\n\n')
    # 填充数据表
    offset_name = scui_image_offset_name
    offset_value = scui_image_offset_value
    scui_image_parser_h.write('typedef enum {\n')
    scui_image_parser_h.write('\t%s = %s,\n' % (offset_name, offset_value))
    scui_image_num = 0
    for file in file_path_list:
        scui_image_num += 1
        # 去除src根目录段(image_src/), 保留实际资源子路径, 缩短枚举长度
        file_short = os.path.relpath(file, src_path).replace('\\', '/')
        scui_image_tag = (project_name + '_' + file_short).replace('.', '').replace('\\', '_').replace('/', '_').replace(' ', '_')
        scui_image_ofs = hex(eval(offset_value) + scui_image_num)
        scui_image_parser_h.write('\tscui_image_%s, // %s\n' % (scui_image_tag, scui_image_ofs))
    scui_image_parser_h.write('} scui_image_parser_handle_t;\n\n')
    scui_image_parser_h.write('extern const void * const scui_image_parser_table[%d];\n\n' % len(file_path_list))
    scui_image_parser_h.write('//<%6s,%6s,%6s,%6s,%2s> handle\n' % ('w', 'h', 'size_raw', 'size_mem', 'com_pct'))
    # 并行编码各子部分, 保持和 file_path_list 的顺序一一对应
    workers = max(1, min(scui_image_pkg_workers, os.cpu_count() or 1))
    print('image pack threads:%d' % workers)
    results = [None] * len(file_path_list)
    if workers > 1 and len(file_path_list) > 1:
        with ThreadPoolExecutor(max_workers=workers) as executor:
            futures = [executor.submit(_scui_image_encode, f, src_path, project_name) for f in file_path_list]
            for i, fut in enumerate(futures):
                results[i] = fut.result()
    else:
        for i, f in enumerate(file_path_list):
            results[i] = _scui_image_encode(f, src_path, project_name)
    # 顺序装订(h/c/bin/sub): 偏移按顺序累加, 与单线程逐图输出一致
    pixel_bin_ofs = 0
    pixel_bin_all = 0
    pixel_raw_all = 0
    for r in results:
        if r is None:
            continue
        scui_image_struct = ''
        scui_image_struct += 'static const scui_image_t %s = {\n' % r['tag']
        scui_image_struct += '\t.format\t\t\t\t = %s,\n' % r['cf']
        scui_image_struct += '\t.type\t\t\t\t = %s,\n' % r['type']
        scui_image_struct += '\t.pixel.width\t\t = %s,\n' % hex(r['width'])
        scui_image_struct += '\t.pixel.height\t\t = %s,\n' % hex(r['height'])
        scui_image_struct += '\t.pixel.data_bin\t\t = %s,\n' % hex(pixel_bin_ofs)
        scui_image_struct += '\t.pixel.size_bin\t\t = %s,\n' % hex(r['size_bin'])
        scui_image_struct += '\t.pixel.size_mem\t\t = %s,\n' % hex(r['size_raw'])
        scui_image_struct += '};\n\n'
        scui_image_parser_c.write(scui_image_struct)
        scui_image_parser_bin.write(r['bin'])
        # 我们生成一个子记录,用于外界解析时使用(内部只使用全部bin)
        with open(os.path.join(scui_image_parser_sub, r['sub_name']), mode='w', encoding='utf-8') as file:
            file.write(''.join(r['sub_body']) + scui_image_struct + '\n')
        # 信息记录
        pixel_bin_all += r['size_bin']
        pixel_raw_all += r['size_raw']
        scui_image_parser_h.write('//<%6s,%6s,%6s,%6s,%2.2f> scui_image_%s\n' %
                                   (hex(r['width']), hex(r['height']), hex(r['size_bin']),
                                    hex(r['size_raw']), float(r['size_bin']) / float(r['size_raw']),
                                    r['tag']))
        pixel_bin_ofs += r['size_bin']
    scui_image_parser_h.write('\n//static pct:%2.2f\n' % (float(pixel_bin_all) / float(pixel_raw_all)))
    scui_image_parser_h.write('\n#endif\n')
    # 填充数据表
    scui_image_parser_c.write('const void * const scui_image_parser_table[%d] = {\n' % len(file_path_list))
    for file in file_path_list:
        # 去除src根目录段(image_src/), 保留实际资源子路径, 缩短枚举长度
        file_short = os.path.relpath(file, src_path).replace('\\', '/')
        scui_image_tag = (project_name + '_' + file_short).replace('.', '').replace('\\', '_').replace('/', '_').replace(' ', '_')
        scui_image_parser_c.write('\t(void *)&%s,\n' % scui_image_tag)
    scui_image_parser_c.write('};\n')


# 遍历整个文件夹,提取目标文件
def scui_image_collect(file_path_list, file_ext_list, path):
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
    # 图形配置: 前端统一配置 json 覆盖头部默认
    _load_image_cfg()
    # 句柄偏移: 前端"设置句柄"json 覆盖头部默认
    global scui_image_offset_name, scui_image_offset_value
    scui_image_offset_name, scui_image_offset_value = \
        _handle_offset('image', scui_image_offset_name, scui_image_offset_value)
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
    print('src path:', _rel_ui(src_path))
    print('dst path:', _rel_ui(dst_path))
    # 遍历整个文件夹,获取指定扩展名的文件
    file_ext_list = ['.bmp', '.jpg', '.jpeg', '.png', '.gif', '.lottie.json', '.mp4']
    file_path_list = []
    scui_image_collect(file_path_list, file_ext_list, src_path)
    # check:
    # for item in file_path_list:
    #     print(item)
    # 核查文件支持
    scui_image_parser_h = open(os.path.join(dst_path, 'scui_image_parser.h'), mode='w', encoding='utf-8')
    scui_image_parser_c = open(os.path.join(dst_path, 'scui_image_parser.c'), mode='w', encoding='utf-8')
    scui_image_parser_bin = open(os.path.join(dst_path, 'scui_image_parser.bin'), mode='wb')
    scui_image_parser_sub = os.path.join(dst_path, "image_array")   # 子文件单独生成到子路径
    scui_image_parser_list = [
        scui_image_parser_h,
        scui_image_parser_c,
        scui_image_parser_bin,
        scui_image_parser_sub,
    ]
    scui_image_parser_all(file_path_list, scui_image_parser_list, project_name, src_path)
    scui_image_parser_h.close()
    scui_image_parser_c.close()
    scui_image_parser_bin.close()


if __name__ == '__main__':
    try:
        scui_image_parser()
    except Exception as e:
        print(e)
    print('scui image parser finish')
    input('请按任意键退出...')
