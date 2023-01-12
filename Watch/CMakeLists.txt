cmake_minimum_required (VERSION 3.10.1)
project(app_main C)
# 生成文件路径
set(app_arch_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_arch
    ${CMAKE_CURRENT_SOURCE_DIR}/app_arch/app_arch_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_arch/app_arch_ext_mem
)
set(app_os_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_os
    ${CMAKE_CURRENT_SOURCE_DIR}/app_os/app_os_adaptor
    ${CMAKE_CURRENT_SOURCE_DIR}/app_os/app_os_ext_mem
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
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_master/app_module_system
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_irq
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_alarm
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_clock
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_countdown
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_stopwatch
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_mix_custom/app_module_world_time
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_source_manage
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_source_manage/app_module_ext_mem
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_source_manage/app_module_source
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_source_manage/app_module_dump
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_source_manage/app_module_load
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_source_manage/app_module_source_mix
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/app_thread/app_thread_lvgl/app_lvgl/lvgl
)
set(app_lvgl_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/app_lvgl/lvgl
)
set(app_main_path
    ${CMAKE_CURRENT_SOURCE_DIR}/app_!linux
)
# 编译器添加文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${app_arch_path})
include_directories(${app_os_path})
include_directories(${app_sys_path})
include_directories(${app_thread_path})
include_directories(${app_lvgl_path})
include_directories(${app_main_path})
# 生成库以及指定目标
set(app_empty ${CMAKE_CURRENT_SOURCE_DIR}/app_empty.c)
add_library(app_arch    SHARED ${app_empty})
add_library(app_os      SHARED ${app_empty})
add_library(app_sys     SHARED ${app_empty})
add_library(app_thread  SHARED ${app_empty})
add_executable(app_main ${CMAKE_CURRENT_SOURCE_DIR}/app_!linux/main.c)
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_arch    ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_arch)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_os      ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_os)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread  ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_lvgl    ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_lvgl)
# 添加链接库    /usr/lib  /usr/local/lib
find_library(linux_lib pthread)
# 设置实时路径
set(CMAKE_USE_RELATIVE_PATHS ON)
# 设置编译规则
SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")
# 指定库的链接语言
set_target_properties(app_arch   PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_os     PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_sys    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_thread PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_main   PROPERTIES LINKER_LANGUAGE C)
# 添加库依赖
target_link_libraries(app_main   PUBLIC ${linux_lib})
target_link_libraries(app_main   PUBLIC app_thread)
target_link_libraries(app_arch   PUBLIC ${linux_lib})
target_link_libraries(app_os     PUBLIC ${linux_lib})
target_link_libraries(app_sys    PUBLIC app_arch)
target_link_libraries(app_sys    PUBLIC app_os)
target_link_libraries(app_thread PUBLIC app_arch)
target_link_libraries(app_thread PUBLIC app_os)
target_link_libraries(app_thread PUBLIC app_sys)
target_link_libraries(app_thread PUBLIC lvgl)
# 重命名
set_target_properties(app_main
                      PROPERTIES OUTPUT_NAME main
                      PREFIX "")
# 指定exe输出到cmake_exe
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/cmake_exe)
# 指定so输出到cmake_lib
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/cmake_lib)
