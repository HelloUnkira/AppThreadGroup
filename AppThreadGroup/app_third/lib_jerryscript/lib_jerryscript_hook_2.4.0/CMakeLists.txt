# JerryScript 平台适配库:

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
add_library(jerry-port STATIC ${SOURCES})
set_target_properties(jerry-port PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(jerry-port PUBLIC app_ext)
target_link_libraries(jerry-port PUBLIC app_sys)
