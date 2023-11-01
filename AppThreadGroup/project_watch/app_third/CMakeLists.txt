# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app_third/lib_self       ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_self)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app_third/lib_fatfs      ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_fatfs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app_third/lib_nanopb     ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_nanopb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app_third/lib_json       ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_json)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app_third/lib_lvgl       ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_lvgl)
# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb)
