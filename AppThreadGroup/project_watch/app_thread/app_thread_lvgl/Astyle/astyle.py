# encoding=utf-8
# astyle  自动化递归astyle脚本
# astyle: https://astyle.sourceforge.net/
# 可执行源: .\astyle.exe
# 执行命令: 见execute
# 编译目标: .


import os
import os.path


compile = r'.\astyle.exe'
# 如果执行无效时跟换style类型后换回
execute = r'--style=allman --indent-switches --indent-preproc-cond --indent-preproc-block --min-conditional-indent=0 --convert-tabs --break-blocks --pad-oper --pad-header --pad-comma --unpad-paren --align-pointer=name --break-closing-braces --break-one-line-headers --add-braces --lineend=windows  --indent-after-parens --break-after-logical --max-code-length=140 --fill-empty-lines'
# execute = r'--style=allman -p -s4 -S -f -xW -w -xw'
# execute = r'--style=linux'

def astyle_recure(path) -> None:
    for item in os.listdir(path):
        if os.path.isfile(path + '\\' + item):
            if item.split('.')[-1] == 'c' or item.split('.')[-1] == 'h':
                print(compile + ' ' + path + '\\' + item + ' ' + execute)
                os.system(compile + ' ' + path + '\\' + item + ' ' + execute)
        if os.path.isdir(path + '\\' + item):
            astyle_recure(path + '\\' + item)


def astyle_recure_clean(path) -> None:
    for item in os.listdir(path):
        if os.path.isfile(path + '\\' + item):
            if item.split('.')[-1] == 'orig':
                os.remove(path + '\\' + item)
        if os.path.isdir(path + '\\' + item):
            astyle_recure_clean(path + '\\' + item)


if __name__ == '__main__':
    path_list = [
        r'..\lvgl_plug',
        r'..\lvgl_sched',
        r'..\lvgl_ui',
    ]
    for path in path_list:
        astyle_recure(path)
        astyle_recure_clean(path)
    input('astyle recuse and clean orig finish')

