# nanopb 库:

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES pb_common.c pb_decode.c pb_encode.c)
add_library(app_nanopb STATIC ${SOURCES})
set_target_properties(app_nanopb PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_nanopb PUBLIC app_ext)
