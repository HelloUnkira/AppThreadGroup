cmake_minimum_required (VERSION 3.10.1)
project(app_main C)
# 生成文件路径
set(app_adaptor_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor/app_arch_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor/app_arch_ext_mem
    ${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor/app_os_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor/app_os_ext_mem
)
set(app_sys_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_sys
    ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/app_sys_pipe
    ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/app_sys_ring_buffer
    ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/app_sys_util
)
set(app_thread_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master/app_thread_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master/app_module_vbus
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master/app_module_system
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master/app_module_work
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master/app_module_watchdog
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_irq
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_timer
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_clock
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_alarm
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_countdown
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_stopwatch
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_world_time
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_ext_mem
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_source
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_trace
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_data/app_module_dump
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_data/app_module_load
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_data/app_module_shutdown
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_data/app_module_data/app_module_source_mix
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_protocol/app_module_protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_protocol/app_module_transfer
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_data_manage/app_module_protocol/app_module_transfer_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_drv
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui_scene
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui_scene/app_lv_ui_watch
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui_scene/app_lv_ui_test
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui_scene/app_lv_ui_group
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui_scene/app_lv_ui_scene/app_lv_ui_stopwatch
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lv_ui_scene/app_lv_ui_scene/app_lv_ui_countdown
)
set(app_third_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/app_third_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/app_third_lib/lvgl_lib
)
set(app_main_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_main
)
# 编译器添加文件搜索路径
include_directories(${app_adaptor_path})
include_directories(${app_sys_path})
include_directories(${app_third_lib})
include_directories(${app_thread_path})
include_directories(${app_main_path})
# 生成库以及指定目标
set(app_empty ${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor/app_empty.c)
add_library(app_adaptor STATIC ${app_empty})
add_library(app_sys     STATIC ${app_empty})
add_library(app_thread  STATIC ${app_empty})
add_executable(app_main ${CMAKE_CURRENT_SOURCE_DIR}/app_main/main.c)
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor    ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_adaptor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_third_lib  ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_third_lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)
# 添加链接库    /usr/lib  /usr/local/lib
find_library(linux_lib pthread)
find_library(sdl2_lib  SDL2)
# 指定库的链接语言
set_target_properties(app_adaptor   PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_sys       PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_thread    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_main      PROPERTIES LINKER_LANGUAGE C)
# 添加库依赖
target_link_libraries(app_main  PUBLIC ${linux_lib})
target_link_libraries(app_main  PUBLIC ${sdl2_lib})
target_link_libraries(app_main  PUBLIC lvgl)
target_link_libraries(app_main  PUBLIC lvgl_examples)
target_link_libraries(app_main  PUBLIC lvgl_demos)
target_link_libraries(app_main  PUBLIC app_adaptor)
target_link_libraries(app_main  PUBLIC app_sys)
target_link_libraries(app_main  PUBLIC app_thread)
# 重命名
set_target_properties(app_main PROPERTIES OUTPUT_NAME main PREFIX "")
# 指定exe输出到cmake_exe
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/cmake_exe)
# 指定so输出到cmake_lib
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/cmake_lib)
