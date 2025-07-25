# 工程:
    本工程仅用于数学学习记录
    编写一些喜欢的数学逻辑
    培养一下曾经的数学思维
    编写一些喜欢的测试用例

# Windows下(安装Visual Studio并且安装Cmake插件)  
工程编译与执行:  
1>  打开Visual Studio,进入终端(Ctrl + `)  
1>  cd ./app_!cmake  
3>  cmake ..  
4>  双击运行app_thread_group.sln  
5>  找到解决方案资源管理器  
    找到解决方案app_thread_group  
    找到app_main:右键设置为启动项目  
5>  点击本地Windows调试器  
直接执行:调试模式的可执行文件  
可执行文件:app_!cmake/app_main下生成  

# Linux下(安装Cmake和SDL2)  
工程编译与执行:  
1>  cd ./app_\!cmake  
2>  cmake ..  
3>  make  
4>  cd ../app_main/app_exe  
5>  ./main  
