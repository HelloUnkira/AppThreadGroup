# git 根目录执行命令添加子仓库:
# git submodule add -f --branch (BranchName) (URL) (Path)

# Self库:
# master https://github.com/HelloUnkira/StudyRecord.git
# AppThreadGroup/app_third/lib_self/StudyRecord
#
# FatFS库:
# master https://github.com/abbrev/fatfs.git
# AppThreadGroup/app_third/lib_fatfs/lib_fatfs
#
# LVGL库:
# release/v8.3 https://github.com/lvgl/lvgl.git
# AppThreadGroup/app_third/lib_lvgl/lib_lvgl_8_3
# release/v8.2 https://github.com/lvgl/lvgl.git
# AppThreadGroup/app_third/lib_lvgl/lib_lvgl_8_2
# LVGL库(字库制作工具):
# master https://github.com/lvgl/lv_font_conv.git
# AppThreadGroup/app_third/lib_lvgl/lib_lvgl_font_conv
#
# cJSON库:
# master https://github.com/DaveGamble/cJSON.git
# AppThreadGroup/app_third/lib_json/lib_json
#
# nanopb库
# master https://github.com/nanopb/nanopb.git
# AppThreadGroup/app_third/lib_nanopb/lib_nanopb
# nanopb库(编译器):https://jpa.kapsi.fi/nanopb/download
#
# tinycrypt库:
# master https://github.com/intel/tinycrypt.git 
# AppThreadGroup/app_third/lib_tinycrypt/lib_tinycrypt
#
# mbedtls库:
# master https://github.com/Mbed-TLS/mbedtls.git
# AppThreadGroup/app_third/lib_mbedtls/lib_mbedtls
#
# LZ4库:
# dev https://github.com/lz4/lz4.git
# AppThreadGroup/app_third/lib_lz4/lib_lz4
#
# TinyEKF库(Extend Kalman Filter):
# master https://github.com/simondlevy/TinyEKF.git
# AppThreadGroup/app_third/lib_TinyEKF/lib_TinyEKF
#
# JerryScript库(JS):
# master https://github.com/jerryscript-project/jerryscript.git
# AppThreadGroup/app_third/lib_jerryscript/lib_jerryscript
#

#
# MinGW-W64:
# Windows下的GCC编译器
# 可用于制作C的dll文件以便Python进行使用
# https://github.com/nixman/mingw-builds-binaries
# gcc -shared -o lz4hc.dll lz4hc.c lz4.c
# gcc -shared -o lz4.dll lz4.c
#

# 本地不额外载入第三方库
# 该动作针对于每一个需要的目标进行
