import os
import os.path
# -- coding: utf-8 --**


# 编写集成化头文件
def encode_app_lv_pic_h(file, path):
    file.write('#ifndef APP_LV_PIC_H\n')
    file.write('#define APP_LV_PIC_H\n\n')
    # 写点简要的说明
    file.write('/*本地静态的图片路径宏表\n')
    file.write(' *通过app_lv_pic.py生成\n')
    file.write(' *脚本会递归遍历..\lvgl_pic下所有文件\n')
    file.write(' *并读取相对路径与名字合成宏\n */\n\n')
    # file.write('typedef enum {\n')
    # 编写递归化文件遍历流程
    def encode_app_lv_pic_h_rec(path_curr, path_root):
        for name_curr in os.listdir(path_curr):
            path_file_abs = path_curr + '\\' + name_curr
            if os.path.isfile(path_file_abs):
                path_file_part = path_file_abs.replace(path_root, '')
                path_file_part = path_file_part.replace('\\', r'/')
                path_file_name = path_file_part
                path_file_part = '\"S:lvgl_pic' + path_file_part + '\"'
                path_file_name = path_file_name.replace(r'/', r'_')
                path_file_name = path_file_name.replace(r'.', r'_')
                path_file_name = 'APP_LV_PIC' + path_file_name
                path_file_name = path_file_name.upper()
                path_file_name = '{0:<50}'.format(path_file_name)
                # print('#define' + path_file_name + ' ' + path_file_part + '\n')
                file.write('#define ' + path_file_name + ' ' + path_file_part + '\n')
            if os.path.isdir(path_file_abs):
                encode_app_lv_pic_h_rec(path_file_abs, path_root)
    encode_app_lv_pic_h_rec(path, path)
    # file.write('} app_lv_pic_t;\n\n')
    file.write('\n')
    file.write('#endif\n')


# 启用集成化资源总表生成
def encode_app_lv_pic(app_lv_pic_path):
    file_encode = 'utf-8'
    # 开启三个文件
    app_lv_pic_h = open('app_lv_pic.h', mode='w', encoding=file_encode)
    # 解析
    encode_app_lv_pic_h(app_lv_pic_h, app_lv_pic_path)
    # 关闭三个文件
    app_lv_pic_h.close()


if __name__ == '__main__':
    encode_app_lv_pic(r'..\lvgl_pic')
    print('app lv pic generation completed')

