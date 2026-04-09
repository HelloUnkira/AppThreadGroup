# thorvg 库:

project(thorvg LANGUAGES C CXX)
add_library(thorvg STATIC)
set_target_properties(thorvg PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB_RECURSE SOURCES *.cpp)
target_sources(thorvg PRIVATE ${SOURCES})

FIND_TARGET_H_PATH(scui_thorvg_path_h ${CMAKE_CURRENT_SOURCE_DIR}/lib_thorvg)
target_include_directories(thorvg SYSTEM PRIVATE ${scui_thorvg_path_h})

target_include_directories(app_thread PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib_thorvg/thorvg_scui.h")
target_include_directories(app_thread PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib_thorvg/thorvg_capi.h")
target_link_libraries(app_thread PRIVATE thorvg)
