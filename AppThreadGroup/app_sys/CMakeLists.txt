# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem/app_sys_ext_mem_table.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem)
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_src/app_sys_ext_src_table.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_src)
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_idx_str/app_sys_idx_str.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_idx_str)
# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
add_library(app_sys STATIC ${SOURCES})
set_target_properties(app_sys PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_sys PUBLIC app_ext)
