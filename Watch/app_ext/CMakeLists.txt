# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
add_library(app_ext STATIC ${SOURCES})
set_target_properties(app_ext PROPERTIES LINKER_LANGUAGE C)
