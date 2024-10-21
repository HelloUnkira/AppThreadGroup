# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_self       ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_self)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_fatfs      ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_fatfs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb     ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_nanopb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_json       ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_json)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lz4        ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_lz4)

# 载入第三方库(lvgl库)
if (app_thread_group_use_lvgl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl       ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_lvgl)
endif()

# git 根目录执行命令添加子仓库:
# git submodule add -f --branch (BranchName) (URL) (Path)

# Self库:
# master https://github.com/HelloUnkira/StudyRecord.git
# AppThreadGroup/project_watch/app_third/lib_self/StudyRecord
#
# FatFS库:
# master https://github.com/abbrev/fatfs.git
# AppThreadGroup/project_watch/app_third/lib_fatfs/lib_fatfs
#
# LVGL库:
# release/v8.2 https://github.com/lvgl/lvgl.git
# AppThreadGroup/project_watch/app_third/lib_lvgl/lib_lvgl_8_2
#
# LVGL库(字库制作工具):
# master https://github.com/lvgl/lv_font_conv.git
# AppThreadGroup/project_watch/app_third/lib_lvgl/lib_lvgl_font_conv
#
# cJSON库:
# master https://github.com/DaveGamble/cJSON.git
# AppThreadGroup/project_watch/app_third/lib_json/lib_json
#
# nanopb库
# master https://github.com/nanopb/nanopb.git
# AppThreadGroup/project_watch/app_third/lib_nanopb/lib_nanopb
# nanopb库(编译器):https://jpa.kapsi.fi/nanopb/download
#
# tinycrypt库:
# master https://github.com/intel/tinycrypt.git 
# AppThreadGroup/project_watch/app_third/lib_tinycrypt/lib_tinycrypt
#
# mbedtls库:
# master https://github.com/Mbed-TLS/mbedtls.git
# AppThreadGroup/project_watch/app_third/lib_mbedtls/lib_mbedtls
#
# LZ4库:
# dev https://github.com/lz4/lz4.git
# AppThreadGroup/project_watch/app_third/lib_lz4/lib_lz4
#
# TinyEKF库(Extend Kalman Filter):
# master https://github.com/simondlevy/TinyEKF.git
# AppThreadGroup/project_watch/app_third/lib_TinyEKF/lib_TinyEKF
#
#
#
# MinGW-W64:
# Windows下的GCC编译器
# 可用于制作C的dll文件以便Python进行使用
# https://github.com/nixman/mingw-builds-binaries
# gcc -shared -o lz4hc.dll lz4hc.c lz4.c
# gcc -shared -o lz4.dll lz4.c
#
#
#
