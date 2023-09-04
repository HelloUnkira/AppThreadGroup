# 构建主依赖项
# 收集源,生成指定目标,指定目标的链接语言,添加库依赖,重命名
file(GLOB_RECURSE SOURCES *.c)
add_executable(app_main ${SOURCES})
set_target_properties(app_main PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(app_main PUBLIC app_thread)
set_target_properties(app_main PROPERTIES OUTPUT_NAME main PREFIX "")
# 文件迁移到调试
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/app_source/*")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/app_project_main)
# 注意:  
# app_main/app_source/*下是一些需要的资源文件  
# 它与可执行文件应该在同一文件夹下面  
