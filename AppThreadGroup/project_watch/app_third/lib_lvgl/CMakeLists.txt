
# 载入第三方库(lvgl release:8.2)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl)

FIND_TARGET_H_PATH(target_path ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(app_third SYSTEM PUBLIC ${target_path})

# 手动载入lvgl,不使用它的Cmake构建
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib_lvgl/src/*.c)
target_sources(app_third PRIVATE ${SOURCES})

target_link_libraries(app_dev PUBLIC app_third)
