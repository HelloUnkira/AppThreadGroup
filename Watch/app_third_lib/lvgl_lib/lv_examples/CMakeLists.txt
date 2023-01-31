file(GLOB_RECURSE SOURCES src/*.c)
add_library(lv_examples STATIC ${SOURCES})
target_link_libraries(lv_examples PUBLIC lvgl)