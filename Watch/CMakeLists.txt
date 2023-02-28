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
# 重命名
set_target_properties(app_main PROPERTIES OUTPUT_NAME main PREFIX "")
# 指定exe输出到cmake_exe
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/cmake_exe)
# 指定so输出到cmake_lib
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app_!cmake/cmake_lib)
