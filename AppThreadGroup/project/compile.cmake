# 多核编译加速:
# if (WIN32)
    # if(MSVC)
        # option(USE_MP "use multiple" ON)
        # option(ProjectConfig_Global_COMPILE_FLAGS_WITH_MP 
            # "Set The Global Option COMPILE_FLAGS /MP to target." ON)
        # if(ProjectConfig_Global_COMPILE_FLAGS_WITH_MP OR USE_MP)
            # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
            # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        # endif()
        # set(VS_STARTUP_PROJECT ${PROJECT_NAME})
    # endif(MSVC)
# endif()
# add_compile_options(-j8)
add_compile_options(/MP)

# 编译器禁用警告
# Visual Studio一大堆警告,甚至库里面都有,屏蔽掉不必要关心的警告
# /* 屏蔽警告4005:宏重定义 */
# /* 屏蔽警告4018:表达式左右值类型不匹配 */
# /* 屏蔽警告4047:间接类型不匹配 */
# /* 屏蔽警告4090:限定符类型不对 */
# /* 屏蔽警告4100:未引用的形参 */
# /* 屏蔽警告4127:条件表达式是常量 */
# /* 屏蔽警告4146:无符号类型运用负运算符 */
# /* 屏蔽警告4189:局部变量初始化但不使用 */
# /* 屏蔽警告4200:0大小数组 */
# /* 屏蔽警告4201:匿名结构/联合体 */
# /* 屏蔽警告4214:使用的非标准扩展名:除int以外的位字段类型 */
# /* 屏蔽警告4244:类型强转可能丢失数据 */
# /* 屏蔽警告4245:类型强转可能丢失数据 */
# /* 屏蔽警告4267:类型强转可能丢失数据 */
# /* 屏蔽警告4334:出现隐式转换 */
# /* 屏蔽警告4389:限定符类型不对 */
# /* 屏蔽警告4456:内部变量屏蔽外部变量 */
# /* 屏蔽警告4457:内部变量屏蔽外部变量 */
# /* 屏蔽警告4477:格式字符串参数不对 */
# /* 屏蔽警告4553:表达式结果未使用 */
# /* 屏蔽警告4701:使用未初始化全局变量 */
# /* 屏蔽警告4702:无法执行到的代码 */
# /* 屏蔽警告4706:表达式内赋值 */
# /* 屏蔽警告4715:函数存在可能的路径缺少返回值 */
# /* 屏蔽警告4716:函数缺少返回值(库里面的) */
# /* 屏蔽警告4819:非 Unicode文件,包含不能表示的字符 */
# /* 屏蔽警告6387:参数可能为0 */
add_compile_options(/W4)
add_compile_options(/wd4005)
add_compile_options(/wd4018)
add_compile_options(/wd4047)
add_compile_options(/wd4057)
add_compile_options(/wd4090)
add_compile_options(/wd4100)
add_compile_options(/wd4127)
add_compile_options(/wd4146)
add_compile_options(/wd4133)
add_compile_options(/wd4189)
add_compile_options(/wd4200)
add_compile_options(/wd4201)
add_compile_options(/wd4210)
add_compile_options(/wd4214)
add_compile_options(/wd4244)
add_compile_options(/wd4245)
add_compile_options(/wd4267)
add_compile_options(/wd4305)
add_compile_options(/wd4334)
add_compile_options(/wd4389)
add_compile_options(/wd4456)
add_compile_options(/wd4457)
add_compile_options(/wd4477)
add_compile_options(/wd4553)
add_compile_options(/wd4701)
add_compile_options(/wd4702)
add_compile_options(/wd4706)
add_compile_options(/wd4715)
add_compile_options(/wd4716)
add_compile_options(/wd4819)
add_compile_options(/wd4996)
add_compile_options(/wd6387)
