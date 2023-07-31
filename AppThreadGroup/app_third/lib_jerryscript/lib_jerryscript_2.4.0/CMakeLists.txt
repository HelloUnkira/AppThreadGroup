
# 本地Python脚本执行,更新源
set(PYTHON "python.exe")
execute_process(COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/js_convert.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# 解压发行版本
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript-2.4.0.tar.gz
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
message("jerryscript-2.4.0.tar.gz")
# 额外补充,删除jerryscript-2.4.0/jerry-math
# 备注:删除jerryscript-2.4.0/jerry-math/include/math.h与标准库冲突
# 且其中的除零宏导致编译错误,因为我们不使用库则直接删掉算了
file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript-2.4.0/jerry-math)
# 配置第三方库
# 备注:JERRY_PORT不应该使用,要自适配 js_hook
set(JERRY_CMDLINE           OFF CACHE BOOL "")
set(JERRY_EXT               ON  CACHE BOOL "")
set(JERRY_MATH              OFF CACHE BOOL "")
set(JERRY_PORT_DEFAULT      OFF CACHE BOOL "")
# 配置第三方库
set(JERRY_DEBUGGER          ON  CACHE BOOL "")
set(JERRY_ERROR_MESSAGES    ON  CACHE BOOL "")
set(JERRY_LOGGING           ON  CACHE BOOL "")
# 载入第三方库
# See jerryscript-2.4.0: CMakeLists.txt
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/jerryscript-2.4.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/js_hook)
# 配置额外的链接关系...
# 此处为外部强行链接,启用相关权限CMP0079
cmake_policy(SET CMP0079 NEW)
target_link_libraries(jerry-core jerry-port)
