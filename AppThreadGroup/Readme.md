# 如何进行多产品多项目构建
app_dev:    平台驱动
app_ext:    平台驱动以及SDK和对应OS
app_sys:    跨平台组件及线程组调度组件
app_third:  第三方支持
app_thread: 线程组及其下属模组(通用模组)
本目录下这些文件夹下的是通用组件
意味着所有产品都需要使用的
实际上只会使用app_sys和app_thread
因为其他的文件夹下的资源是由项目所构建

产品构建(例:watch):
project_watch:对应watch产品
对于上述目录通常我们可以对其进一步拆分为以下三类:
project_watch_util: 跨平台组件(所有项目均使用,稳定的业务模组)
    主要放置app_third,app_thread下一些完全通用的module
project_watch_port: 平台SDK和OS(port尾缀替换不同平台)
    主要放置app_dev,app_ext,下针对特定平台的封装
project_watch_xxxx: 具体的项目(xxxx尾缀替换不同项目编号)
    不同的项目使用同一project_watch_util
    不同的项目使用不同project_watch_port
    主要放置app_sys,app_thread,app_main的专属定制内容
    例如:将其中lvgl的部分ui及res单独提出,作为本项目的专属定制内容
    此外:项目的构建只发生在本文件夹下,其他目录都只做对应的引用
