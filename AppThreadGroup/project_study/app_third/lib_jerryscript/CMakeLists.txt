
# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/js_convert.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# 配置第三方库(jerry-core jerry-ext jerry-math)
set(JERRY_CMDLINE   OFF CACHE BOOL ".\\CMakeLists.txt")
set(JERRY_EXT       ON  CACHE BOOL ".\\CMakeLists.txt")
set(JERRY_MATH      ON  CACHE BOOL ".\\CMakeLists.txt")
# 配置第三方库(jerry-port不使用,自适配)
set(JERRY_PORT      OFF CACHE BOOL ".\\CMakeLists.txt")
# 配置第三方库(jerry-core)
# set(JERRY_DEBUGGER          ON  CACHE BOOL ".\\jerry-core\\CMakeLists.txt")
# set(JERRY_ERROR_MESSAGES    ON  CACHE BOOL ".\\jerry-core\\CMakeLists.txt")
# set(JERRY_LOGGING           ON  CACHE BOOL ".\\jerry-core\\CMakeLists.txt")
# 配置第三方库(jerry-core)
set(JERRY_GLOBAL_HEAP_SIZE  "(512)"  CACHE STRING ".\\jerry-core\\CMakeLists.txt")
set(DEFINES_JERRY ${DEFINES_JERRY}   JERRY_NDEBUG)
# 载入第三方库
set(CMAKE_MESSAGE_LOG_LEVEL WARNING)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript_hook)
set(CMAKE_MESSAGE_LOG_LEVEL NONE)
# 配置额外的链接关系...
# 此处为外部强行链接,启用相关权限CMP0079
cmake_policy(SET CMP0079 NEW)
target_link_libraries(jerry-core jerry-port)

