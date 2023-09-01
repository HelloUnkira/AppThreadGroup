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
add_library(app_third   STATIC)
add_library(app_thread  STATIC)
# 生成库重构依赖项
set_target_properties(app_ext    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_sys    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_third  PROPERTIES LINKER_LANGUAGE C)
set_target_properties(app_thread PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_sys    PUBLIC app_ext)
target_link_libraries(app_third  PUBLIC app_sys)
target_link_libraries(app_thread PUBLIC app_third)
target_link_libraries(app_thread PUBLIC jerry-core)
target_link_libraries(app_thread PUBLIC lvgl)
target_link_libraries(app_thread PUBLIC lvgl_examples)
target_link_libraries(app_thread PUBLIC lvgl_demos)
# 文件迁移到目标
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/../app_sys/app_sys_ext_mem/*.py")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/app_sys_ext_mem)
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/../app_sys/app_sys_idx_str/*.py")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/app_sys/app_sys_idx_str)
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_main       ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_project_main)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_ext        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_project_ext)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../app_ext     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_ext)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../app_sys     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../app_third   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_third)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../app_thread  ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_project_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_third      ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_project_third)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_project_thread)
# 指定exe输出到app_exe
set(EXECUTABLE_OUTPUT_PATH                   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
