# nanopb 库:

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb/pb_common.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb/pb_decode.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/lib_nanopb/pb_encode.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/nanopb_out/*.c)
add_library(app_nanopb STATIC ${SOURCES})
set_target_properties(app_nanopb PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_nanopb PUBLIC app_ext)
# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/nanopb_build.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
