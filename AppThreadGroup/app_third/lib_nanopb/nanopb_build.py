# encoding=utf-8
# nanopb 自动化递归编译脚本
# 编译器源: .\nanopb_x86\generator-bin\protoc.exe
# 执行命令: .\nanopb_x86\generator-bin\protoc.exe --nanopb_out=.  编译源
# 编译目标: .\nanopb_src


import os
import os.path

compile = r'.\nanopb_x86\generator-bin\protoc.exe '
execute = r'--proto_path=.\nanopb_src --nanopb_out=.\nanopb_out '


def nanopb_recuse_build(path) -> None:
    for item in os.listdir(path):
        if os.path.isfile(path + '\\' + item):
            if item.split('.')[-1] == 'proto':
                print(compile + execute + path + '\\' + item)
                os.system(compile + execute + path + '\\' + item)
        if os.path.isdir(path + '\\' + item):
            nanopb_recuse_build(path + '\\' + item)


if __name__ == '__main__':
    nanopb_recuse_build(r'.\nanopb_src')
    print('nanopb protoc recuse build finish')
