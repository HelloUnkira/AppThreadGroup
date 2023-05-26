# encoding=utf-8
# javascript 自动化递归打包脚本
# 打包脚本: .\js_convert\convert_c.py
# 编译目标: .\js_convert_target


import os
import os.path

execute = r'python.exe .\js_convert\convert_c.py '


def js_convert_recuse(path) -> None:
    for item in os.listdir(path):
        if os.path.isfile(path + '\\' + item):
            if item.split('.')[-1] == 'js':
                print(execute + path + '\\' + item + ' ' + path + '\\' + item.split('.')[0])
                os.system(execute + path + '\\' + item + ' ' + path + '\\' + item.split('.')[0])
        if os.path.isdir(path + '\\' + item):
            js_convert_recuse(path + '\\' + item)


def js_convert_recuse_rename(path) -> None:
    for item in os.listdir(path):
        if os.path.isfile(path + '\\' + item):
            if item.split('.')[-1] == 'c':
                if os.path.isfile(path + '\\' + item + '.txt'):
                    os.remove(path + '\\' + item + '.txt')
                os.rename(path + '\\' + item, path + '\\' + item + '.txt')
        if os.path.isdir(path + '\\' + item):
            js_convert_recuse_rename(path + '\\' + item)


if __name__ == '__main__':
    js_convert_recuse(r'.\js_convert_target')
    js_convert_recuse_rename(r'.\js_convert_target')
    input('js convert recuse package finish')
