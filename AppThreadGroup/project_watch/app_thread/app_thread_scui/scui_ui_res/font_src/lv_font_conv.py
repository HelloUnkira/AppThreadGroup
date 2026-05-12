

import os
import sys
import shutil


# {woff/ttf name} {-r range} {size} {bpp} {bin name}
# 这个脚本就是lv_font_conv.txt里面的例子合并起来后的结果
parse_cmd_ord = 'lv_font_conv --font {} {} --size {} --format bin --bpp {} -o ../font_bin/{}.bin --force-fast-kern-format'
parse_cmd_diy = 'lv_font_conv --font {} {} --size {} --format bin --bpp {} -o ../font_bin/{}.bin --no-kerning'
# kern参数:打包会很慢,仅用于艺术字

# LVGL自定义符号表映射区间
range_symbol = '61441,61448,61451,61452,61452,61453,61457,61459,61461,61465,61468,61473,61478,61479,61480,61502,61507,61512,61515,61516,61517,61521,61522,61523,61524,61543,61544,61550,61552,61553,61556,61559,61560,61561,61563,61587,61589,61636,61637,61639,61641,61664,61671,61674,61683,61724,61732,61787,61931,62016,62017,62018,62019,62020,62087,62099,62212,62189,62810,63426,63650'


# 集成解析函数
def parse_cmd_func(parse_cmd_f):
    print('parse start:\n' + parse_cmd_f)
    os.system(parse_cmd_f)
    print('parse over\n\n')


if __name__ == '__main__':
    print('lvgl_font_vonv start \n\n')
    try:
        # ascii + symbol
        for size in [8,12,16,20,24,32,40,48,56,64,72,80,88]:
            parse_cmd_func(parse_cmd_diy.format('font_zh_en.ttf', '-r 0x20-0x7f', size, 4, 'font_ascii_%d' % size))
            parse_cmd_func(parse_cmd_diy.format('font_symbol.woff', '-r ' + range_symbol, size, 4, 'font_symbol_%d' % size))
        
        # lang_en
        parse_cmd_func(parse_cmd_ord.format('font_en_2.ttf', '-r 0x00-0x7f', 32, 8, 'font_en_%d' % 32))
        parse_cmd_func(parse_cmd_ord.format('font_en_2.ttf', '-r 0x00-0x7f', 36, 8, 'font_en_%d' % 36))
        
        # lang_eu
        range_eu = '-r 0x00-0x7F -r 0x80-0xFF -r 0x100-0x17F -r 0x180-0x24F -r 0x2B0-0x2FF'
        parse_cmd_func(parse_cmd_diy.format('font_zh_2.ttf', range_eu, 32, 4, 'font_eu_%d' % 32))
        parse_cmd_func(parse_cmd_diy.format('font_zh_2.ttf', range_eu, 36, 4, 'font_eu_%d' % 36))
        
        # lang_cjk(偷懒没有裁剪范围:这里有俩万多个字的范围)
        range_cjk = '-r 0x00-0x7F -r 0x3000-0x303F -r 0x3040-0x309F -r 0x30A0-0x30FF -r 0x4E00-0x9FFF'
        parse_cmd_func(parse_cmd_diy.format('font_zh_1.ttf', range_cjk, 32, 2, 'font_cjk_%d' % 32))
        parse_cmd_func(parse_cmd_diy.format('font_zh_1.ttf', range_cjk, 36, 2, 'font_cjk_%d' % 36))
        
        # ttf
        ttf_font_name = 'font_tinyTTF.ttf'
        shutil.copy(ttf_font_name, '../font_bin/%s' % ttf_font_name)
    
    except Exception as e:
        print(e)
    print('lvgl_font_vonv finish')
    input('请按任意键退出...')
