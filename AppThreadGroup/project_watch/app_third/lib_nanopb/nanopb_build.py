# encoding=utf-8
# nanopb 自动化递归编译脚本
# 编译器源: .\nanopb_x86\generator-bin\protoc.exe
# 执行命令: .\nanopb_x86\generator-bin\protoc.exe --nanopb_out=.  编译源
# 编译目标: .\nanopb_src


import os
import os.path
import zipfile

compile = r'.\nanopb_x86\generator-bin\protoc.exe '
execute = r'--proto_path=.\nanopb_src --nanopb_out=.\nanopb_out '


def nanopb_recuse_build(path) -> None:
    for item in os.listdir(path):
        if os.path.isfile(path + '\\' + item):
           if item.split('.')[-1] == 'proto':
           #if item.split('.')[-1] == 'proto' and item.split('.')[0] != 'descriptor' and item.split('.')[0] != 'nanopb':
                # print(compile + execute + path + '\\' + item)
                print(item)
                os.system(compile + execute + path + '\\' + item)
        if os.path.isdir(path + '\\' + item):
            nanopb_recuse_build(path + '\\' + item)


def nanopb_to_crlf(out_dir) -> None:
    """将生成目录下的 .pb.h/.pb.c 统一转为 CRLF 行尾(P0 强规则,禁止 LF-only)"""
    for root, _dirs, files in os.walk(out_dir):
        for item in files:
            if not (item.endswith('.pb.h') or item.endswith('.pb.c')):
                continue
            path = os.path.join(root, item)
            with open(path, 'rb') as fp:
                data = fp.read()
            data = data.replace(b'\r\n', b'\n').replace(b'\n', b'\r\n')
            with open(path, 'wb') as fp:
                fp.write(data)


if __name__ == '__main__':
    if not os.path.exists(r'.\nanopb_x86'):
        with zipfile.ZipFile('nanopb_x86.zip', 'r') as zip_src:
            zip_src.extractall(r'.')
    if not os.path.exists(r'.\nanopb_out'):
        os.makedirs(r'.\nanopb_out')
    nanopb_recuse_build(r'.\nanopb_src')
    nanopb_to_crlf(r'.\nanopb_out')
    print('nanopb protoc recuse build finish')
