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
# JerryScript库(JS):
# master https://github.com/jerryscript-project/jerryscript.git
# AppThreadGroup/app_third/lib_jerryscript/lib_jerryscript
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
# TinyEKF库(Extend Kalman Filter):
# master https://github.com/simondlevy/TinyEKF.git
# AppThreadGroup/app_third/lib_TinyEKF/lib_TinyEKF
#

# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_self)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_fatfs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_json)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl)
