# 载入第三方库
# ...

# git 根目录执行命令添加子仓库:
# git submodule add -f --branch (BranchName) (URL) (Path)
#
#

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB_RECURSE SOURCES *.c)
target_sources(app_third PRIVATE ${SOURCES})
