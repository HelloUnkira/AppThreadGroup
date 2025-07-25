# 文件迁移到目标
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/../../app_sys/app_sys_ext_mem/*.py")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem)
# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem/app_sys_ext_mem_table.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem)
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem/app_sys_ext_mem_src_table.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_sys_ext_mem)
# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
target_sources(app_sys PRIVATE ${SOURCES})
