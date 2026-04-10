# thorvg 库:
set(CMAKE_MESSAGE_LOG_LEVEL NONE)

project(thorvg LANGUAGES C CXX)
add_library(thorvg STATIC)
set_target_properties(thorvg PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB_RECURSE SOURCES *.cpp)
target_sources(thorvg PRIVATE ${SOURCES})

# 这个库搞得我很多日心态奔溃的一点莫过于
# 你要带库就自己改个名字自己用啊, 干嘛跟系统的标准头名字搞一样
# target_include_directories(thorvg SYSTEM PRIVATE ${CMAKE_CXX_STANDARD_INCLUDE_DIRS})
# target_include_directories(thorvg BEFORE SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# 路径下的rapidjson子文件夹根本没用到, 为什么带过来折腾人呢?
# FIND_TARGET_H_PATH(scui_draw_tvg_path_h ${CMAKE_CURRENT_SOURCE_DIR}/lib_thorvg)
# target_include_directories(thorvg SYSTEM PRIVATE ${scui_draw_tvg_path_h})

# 我们为其专门导入外界的一个特殊的配置头文件来控制此静态库的启停
target_include_directories(thorvg SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib_thorvg)
target_include_directories(thorvg SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../app_thread/app_thread_scui/scui/draw/scui_draw_tvg")

# 将本库中有必要的头文件暴露到目标库
target_include_directories(app_thread PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib_thorvg")
# 这里会相互依赖, 所以需要循环链接
target_link_libraries(app_thread PRIVATE thorvg)
target_link_libraries(thorvg PRIVATE app_thread)

# 豆包:给thorvg设置动态运行时库（MD/MDd）
if(MSVC)
    target_compile_options(thorvg PRIVATE
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
    )
endif()

set(CMAKE_MESSAGE_LOG_LEVEL WARNING)
