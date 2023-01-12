add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lvgl)
file(GLOB_RECURSE SOURCES lv_drv/*.c)
target_sources(app_lvgl PUBLIC ${SOURCES})
