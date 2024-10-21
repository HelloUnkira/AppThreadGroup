# 载入第三方库
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_jerryscript    ${CMAKE_CURRENT_SOURCE_DIR}/../app_!cmake/project_third_jerryscript)
# 载入第三方库
# ...

# git 根目录执行命令添加子仓库:
# git submodule add -f --branch (BranchName) (URL) (Path)
#
# JerryScript库(JS):
# master https://github.com/jerryscript-project/jerryscript.git
# AppThreadGroup/project_study/app_third/lib_jerryscript/lib_jerryscript
#
