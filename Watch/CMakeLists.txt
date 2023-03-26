cmake_minimum_required (VERSION 3.10.1)
project(app_main C)
# 导入文件搜索路径
include(${CMAKE_CURRENT_SOURCE_DIR}/header.cmake)
# 链接平台的系统库(数学库, linux:(多线程库, SDL2库))
link_libraries(-lm -lpthread -lSDL2)
# 编译器多核编译加速
include(${CMAKE_CURRENT_SOURCE_DIR}/compile.cmake)
# 编译器禁用警告
include(${CMAKE_CURRENT_SOURCE_DIR}/warn.cmake)
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_main       ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_main)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor    ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_adaptor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_third      ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_third)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)

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
# 进入...\app_!cmake下()  
# mkdir app_\!cmake  
# cd    app_\!cmake  
# cmake ..  
# app_!cmake下生成解决方案  
# 双击运行解决方案...  
