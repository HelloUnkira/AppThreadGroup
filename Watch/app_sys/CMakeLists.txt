# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
add_library(app_sys STATIC ${SOURCES})
set_target_properties(app_sys PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_sys PUBLIC app_ext)
