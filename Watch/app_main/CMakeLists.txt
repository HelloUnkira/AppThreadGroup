# 添加链接库 /usr/lib  /usr/local/lib
find_library(linux_lib pthread)
# 收集源
file(GLOB_RECURSE SOURCES *.c)
# 生成指定目标
add_executable(app_main ${SOURCES})
# 指定目标的链接语言
set_target_properties(app_main PROPERTIES LINKER_LANGUAGE C)
# 添加库依赖
target_link_libraries(app_main PUBLIC ${linux_lib})
target_link_libraries(app_main PUBLIC app_thread)
