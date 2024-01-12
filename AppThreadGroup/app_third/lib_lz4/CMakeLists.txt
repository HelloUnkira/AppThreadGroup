# cJSON 库:

# 收集源,生成库,指定库的链接语言,添加库依赖
set (SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib_lz4/lib/lz4.c
             ${CMAKE_CURRENT_SOURCE_DIR}/lib_lz4/lib/lz4file.c
             ${CMAKE_CURRENT_SOURCE_DIR}/lib_lz4/lib/lz4frame.c
             ${CMAKE_CURRENT_SOURCE_DIR}/lib_lz4/lib/lz4hc.c)
target_sources(app_third PRIVATE ${SOURCES})
