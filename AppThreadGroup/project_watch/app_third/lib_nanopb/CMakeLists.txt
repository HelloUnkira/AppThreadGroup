# nanopb 库:

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb/pb_common.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb/pb_decode.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb/pb_encode.c)
target_sources(app_third PRIVATE ${SOURCES})

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nanopb_out/*.c)
target_sources(app_third PRIVATE ${SOURCES})
# 本地Python脚本执行,更新源(新增文件的生成需要手动更新)
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/nanopb_build.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
