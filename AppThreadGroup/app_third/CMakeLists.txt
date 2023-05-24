# git 根目录执行命令添加子仓库:

# 自己的库
# git submodule add -f --branch master https://github.com/HelloUnkira/StudyRecord.git AppThreadGroup/app_third/lib_self/StudyRecord
# LVGL库:
# git submodule add -f --branch release/v8.0 https://github.com/lvgl/lvgl.git AppThreadGroup/app_third/lib_lvgl/lib_lvgl
# git submodule add -f --branch release/v8.1 https://github.com/lvgl/lvgl.git AppThreadGroup/app_third/lib_lvgl/lib_lvgl
# git submodule add -f --branch release/v8.2 https://github.com/lvgl/lvgl.git AppThreadGroup/app_third/lib_lvgl/lib_lvgl
# 或者直接官网下载后放置于./lib_lvgl下修改名字为lib_lvgl(内部文件无需调整)
# cJSON库:
# git submodule add -f --branch master https://github.com/DaveGamble/cJSON.git AppThreadGroup/app_third/lib_json/lib_json
# nanopb库
# git submodule add -f --branch master https://github.com/nanopb/nanopb.git AppThreadGroup/app_third/lib_nanopb/lib_nanopb
# nanopb伴随编译器
# https://jpa.kapsi.fi/nanopb/download

# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_self)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_json)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl)
