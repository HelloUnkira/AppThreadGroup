
# 配置第三方库
set(JERRY_CMDLINE   OFF CACHE BOOL "See Next Level: CMakeLists.txt")
set(JERRY_EXT       ON  CACHE BOOL "See Next Level: CMakeLists.txt")
set(JERRY_MATH      ON  CACHE BOOL "See Next Level: CMakeLists.txt")
# 备注:JERRY_PORT不应该使用,要自适配
set(JERRY_PORT      OFF CACHE BOOL "See Next Level: CMakeLists.txt")
# 配置第三方库
set(JERRY_GLOBAL_HEAP_SIZE  "(64)"  CACHE STRING "See Next Level: CMakeLists.txt")
# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript_hook)
# 配置第三方库版本
set(PYTHON      "python.exe")
set(VERSION_PY ${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript/tools/version.py)
execute_process(COMMAND
    ${PYTHON}
    ${VERSION_PY}
    OUTPUT_VARIABLE JERRY_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
set(JERRY_VERSION "" CACHE STRING "See Next Level: CMakeLists.txt")
# 配置额外的链接关系...
# 此处为外部强行链接,启用相关权限CMP0079
cmake_policy(SET CMP0079 NEW)
target_link_libraries(jerry-core jerry-port)
# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/js_convert.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
