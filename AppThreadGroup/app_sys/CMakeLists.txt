# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
target_sources(app_sys PRIVATE ${SOURCES})
