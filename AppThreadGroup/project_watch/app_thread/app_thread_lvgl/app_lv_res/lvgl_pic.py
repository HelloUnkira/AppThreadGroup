import os
import PIL
import PIL.Image
# -- coding: utf-8 --**


# 启用集成化资源转换
def encode_lvgl_pic(lvgl_pic_path):
    # 获取所有文件
    for root, dirs, files in os.walk(lvgl_pic_path):
        for file in files:
            if os.path.splitext(file)[1] == '.bmp':
                img = PIL.Image.open(os.path.join(root, file))
                img.save(os.path.join(root, file.split('.')[0] + '.png'))
                os.remove(os.path.join(root, file))


if __name__ == '__main__':
    encode_lvgl_pic(r'lvgl_pic')
    print('lvgl pic generation completed')

