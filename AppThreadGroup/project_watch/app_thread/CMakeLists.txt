# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/app_lv_lang/app_lv_lang.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app_thread_lvgl/app_lv_res/app_lv_lang)
# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
target_sources(app_thread PRIVATE ${SOURCES})
