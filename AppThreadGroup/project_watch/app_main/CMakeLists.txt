# 文件迁移到调试
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/app_source/*")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_main)

# 构建主依赖项
# 收集源,导入库,重命名
file(GLOB_RECURSE SOURCES *.c)
add_executable(app_main ${SOURCES})
# set_target_properties(app_main PROPERTIES LINKER_LANGUAGE C)
FIND_TARGET_H_PATH(target_path ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(app_ext SYSTEM PUBLIC ${target_path})
target_include_directories(app_main SYSTEM PUBLIC ${target_path})

set_target_properties(app_main PROPERTIES OUTPUT_NAME main PREFIX "")
target_link_libraries(app_main PUBLIC app_thread)
