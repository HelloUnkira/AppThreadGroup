# 收集源,导入库
file(GLOB_RECURSE SOURCES *.c)
target_sources(app_dev PRIVATE ${SOURCES})
