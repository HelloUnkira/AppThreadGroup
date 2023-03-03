# LVGL 库:
# https://github.com/lvgl/lvgl/tree/release/v8.0
# https://github.com/lvgl/lvgl/tree/release/v8.1
# https://github.com/lvgl/lvgl/tree/release/v8.2
# 下载后本目录下修改名字为lib_lvgl(内部文件无需调整)

# 载入lvgl第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_self)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl/lib_lvgl)
