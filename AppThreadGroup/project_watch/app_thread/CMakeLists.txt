# 收集源,导入库
file(GLOB_RECURSE SOURCES app_thread_group/*.c)
target_sources(app_thread PRIVATE ${SOURCES})
file(GLOB_RECURSE SOURCES app_thread_mix_irq/*.c)
target_sources(app_thread PRIVATE ${SOURCES})
file(GLOB_RECURSE SOURCES app_thread_mix_custom/*.c)
target_sources(app_thread PRIVATE ${SOURCES})
file(GLOB_RECURSE SOURCES app_thread_manage/*.c)
target_sources(app_thread PRIVATE ${SOURCES})

if (app_thread_group_use_lvgl)
# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/app_lv_lang/app_lv_lang.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/app_lv_lang)
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/app_lv_pic/app_lv_pic.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/app_lv_pic)

# 注意:  
# 依赖资源文件:  
# app_main/app_source  
# 程序打包到依赖资源文件中的文件:  
# app_thread/app_thread_lvgl/app_lv_res/lvgl_font  
# app_thread/app_thread_lvgl/app_lv_res/lvgl_pic  
# 上面的资源文件与可执行文件应该在同一文件夹下面  

# 文件迁移到调试
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/lvgl_font/*")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_main/lvgl_font)
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/lvgl_pic/*")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_main/lvgl_pic)

file(GLOB_RECURSE SOURCES app_thread_lvgl/*.c)
target_sources(app_thread PRIVATE ${SOURCES})
endif()

if (app_thread_group_use_scui)

if (FALSE)
add_library(scui_thorvg STATIC)
set_target_properties(scui_thorvg PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(app_thread PUBLIC scui_thorvg)
file(GLOB_RECURSE SOURCES app_thread_scui/scui/linker/app_thrid/thorvg/*.cpp)
target_sources(scui_thorvg PRIVATE ${SOURCES})
FIND_TARGET_H_PATH(scui_thorvg_path_h ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_scui/scui/linker/app_thrid/thorvg)
FIND_TARGET_HPP_PATH(scui_thorvg_path_hpp ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_scui/scui/linker/app_thrid/thorvg)
target_include_directories(scui_thorvg SYSTEM PUBLIC scui_thorvg_path_h scui_thorvg_path_hpp)
endif()

# 文件迁移到调试
file(GLOB   SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/app_thread_scui/scui_ui_res/font_out/*"
                        "${CMAKE_CURRENT_SOURCE_DIR}/app_thread_scui/scui_ui_res/image_out/*.mem"
                        "${CMAKE_CURRENT_SOURCE_DIR}/app_thread_scui/scui_ui_res/image_out/*.raw"
                        "${CMAKE_CURRENT_SOURCE_DIR}/app_thread_scui/scui_ui_res/image_out/*.bin")
file(COPY ${SourceFiles} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_main)

file(GLOB_RECURSE SOURCES app_thread_scui/*.c)
target_sources(app_thread PRIVATE ${SOURCES})
endif()
