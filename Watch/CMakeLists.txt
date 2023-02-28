cmake_minimum_required (VERSION 3.10.1)
project(app_main C)
# 导入文件搜索路径
include(${CMAKE_CURRENT_SOURCE_DIR}/header.cmake)
# 添加子目录
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_main       ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_main)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_adaptor    ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_adaptor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_sys        ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_sys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_third      ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_third_lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app_thread     ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/app_thread)

