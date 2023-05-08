# LVGL 库:
# git 根目录执行命令添加子仓库
# git submodule add -f --branch release/v8.0 https://github.com/lvgl/lvgl.git Watch/app_third/lib_lvgl/lib_lvgl
# git submodule add -f --branch release/v8.1 https://github.com/lvgl/lvgl.git Watch/app_third/lib_lvgl/lib_lvgl
# git submodule add -f --branch release/v8.2 https://github.com/lvgl/lvgl.git Watch/app_third/lib_lvgl/lib_lvgl
# 下载后本目录下修改名字为lib_lvgl(内部文件无需调整)

# 载入lvgl第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_self)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl/lib_lvgl)
