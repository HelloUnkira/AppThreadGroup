cmake_minimum_required (VERSION 3.10.1)
# 添加任意策略
# target_link_libraries
cmake_policy(SET CMP0079 NEW)
# 创建项目
project(app_thread_group C)
# 链接平台的系统库(数学库, linux:(多线程库, SDL2库))
link_libraries(-lm -lpthread -lSDL2)
# 编译参数设置,导入文件搜索路径
include(${CMAKE_CURRENT_SOURCE_DIR}/compile.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/directory.cmake)
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
