# FatFS 库:

# 配置文件覆盖库
file(GLOB   ConfigureFile "${CMAKE_CURRENT_SOURCE_DIR}/ffconf.h")
file(COPY ${ConfigureFile} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib_fatfs/source)

# 收集源,生成库,指定库的链接语言,添加库依赖
set (SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib_fatfs/source/ff.c
             ${CMAKE_CURRENT_SOURCE_DIR}/lib_fatfs/source/ffunicode.c
             ${CMAKE_CURRENT_SOURCE_DIR}/diskio.c
             ${CMAKE_CURRENT_SOURCE_DIR}/ffsystem.c
             ${CMAKE_CURRENT_SOURCE_DIR}/app_third_fatfs.c)
target_sources(app_third PRIVATE ${SOURCES})
