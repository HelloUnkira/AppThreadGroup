cmake_minimum_required (VERSION 3.10.1)
#
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
# 添加任意策略
# target_link_libraries
cmake_policy(SET CMP0079 NEW)
# 创建项目
project(app_thread_group C CXX)
# 设置C和C++的编译选项
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
# 禁用预编译头
set(CMAKE_DISABLE_PRECOMPILE_HEADERS OFF)
# 链接平台的系统库(数学库, linux:(多线程库, SDL2库))
link_libraries(-lm -lpthread -lSDL2)
# 编译参数设置,导入文件搜索路径
include(${CMAKE_CURRENT_SOURCE_DIR}/compile.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/directory.cmake)
# 编译器添加文件搜索路径
FIND_TARGET_H_PATH(app_ext_path     ${CMAKE_CURRENT_SOURCE_DIR}/../app_ext)
FIND_TARGET_H_PATH(app_sys_path     ${CMAKE_CURRENT_SOURCE_DIR}/../app_sys)
FIND_TARGET_H_PATH(app_thread_path  ${CMAKE_CURRENT_SOURCE_DIR}/../app_thread)
include_directories(${app_ext_path})
include_directories(${app_sys_path})
include_directories(${app_thread_path})
# 添加生成库
add_library(app_ext     STATIC)
add_library(app_sys     STATIC)
add_library(app_dev     STATIC)
add_library(app_third   STATIC)
add_library(app_thread  STATIC)
# 生成库重构依赖项
set_target_properties(app_ext    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_sys    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_dev    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_third  PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_thread PROPERTIES LINKER_LANGUAGE C)
# 定义GUI宏
set(app_thread_group_use_lvgl   FALSE)
set(app_thread_group_use_scui   TRUE)
# 这里存在库的交叉依赖,因为日志模组希望做成全局化
target_link_libraries(app_sys    PUBLIC app_ext)
target_link_libraries(app_dev    PUBLIC app_sys)
if (app_thread_group_use_lvgl)
target_link_libraries(app_dev    PUBLIC lvgl)
target_link_libraries(app_dev    PUBLIC lvgl_demos)
target_link_libraries(app_dev    PUBLIC lvgl_examples)
endif()
target_link_libraries(app_third  PUBLIC app_dev)
target_link_libraries(app_thread PUBLIC app_third)
# 编译器添加文件搜索路径
FIND_TARGET_H_PATH(app_ext_path     ${CMAKE_CURRENT_SOURCE_DIR}/app_ext)
FIND_TARGET_H_PATH(app_sys_path     ${CMAKE_CURRENT_SOURCE_DIR}/app_sys)
FIND_TARGET_H_PATH(app_dev_path     ${CMAKE_CURRENT_SOURCE_DIR}/app_dev)
FIND_TARGET_H_PATH(app_third_path   ${CMAKE_CURRENT_SOURCE_DIR}/app_third)
FIND_TARGET_H_PATH(app_thread_path  ${CMAKE_CURRENT_SOURCE_DIR}/app_thread)
FIND_TARGET_H_PATH(app_main_path    ${CMAKE_CURRENT_SOURCE_DIR}/app_main)
include_directories(${app_ext_path})
include_directories(${app_sys_path})
include_directories(${app_dev_path})
include_directories(${app_third_path})
include_directories(${app_thread_path})
include_directories(${app_main_path})
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../app_sys     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../app_thread  ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_ext        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/project_ext)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/project_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_dev        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/project_dev)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_third      ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/project_third)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/project_thread)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_main       ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/project_main)
# 指定exe输出到app_exe
set(EXECUTABLE_OUTPUT_PATH                   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
