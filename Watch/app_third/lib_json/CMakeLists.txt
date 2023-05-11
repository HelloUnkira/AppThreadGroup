# cJSON 库:

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES cJSON.c cJSON_Utils.c)
add_library(app_json STATIC ${SOURCES})
set_target_properties(app_json PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_json PUBLIC app_ext)
