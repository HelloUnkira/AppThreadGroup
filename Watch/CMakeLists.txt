cmake_minimum_required (VERSION 3.10.1)
project(app_main C)
# 链接平台的系统库(数学库, linux:(多线程库, SDL2库))
link_libraries(-lm -lpthread -lSDL2)
# 导入文件搜索路径,编译参数
include(${CMAKE_CURRENT_SOURCE_DIR}/path.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/compile.cmake)
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_main       ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_main)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_ext        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_ext)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_third      ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_third)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)
# 指定exe输出到app_exe
set(EXECUTABLE_OUTPUT_PATH                   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY           ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_exe)
# 文件迁移到调试
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/app_main/app_source/*")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_main)

# 注意:  
# app_main/app_source/*下是一些需要的资源文件  
# 它与可执行文件应该在同一文件夹下面  

# Linux下(前提:安装cmake和sdl):  
# 工程编译与执行:  
# cd    Watch  
# mkdir app_\!cmake  
# cd    app_\!cmake  
# cmake ..  
# make  
# cd    ../app_main/app_exe  
# ./main  

# Windows下(前提:安装Visual Studio并且安装cmake插件)  
# 工程编译与执行:  
# 打开Visual Studio,进入终端(Ctrl + `)  
# mkdir app_\!cmake  
# cd    app_\!cmake  
# cmake ..  
# app_!cmake下生成解决方案  
# 双击运行解决方案...  
# 设置app_main为启动项目  
# 编译结束后app_!cmake/app_main下生成可执行文件  
