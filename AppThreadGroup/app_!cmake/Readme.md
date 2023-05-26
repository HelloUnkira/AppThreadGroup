

# Linux下(前提:安装cmake和sdl):  
工程编译与执行:  
1>  cd 本目录  
2>  cmake ..  
3>  make  
4>  cd ../app_main/app_exe  
5>  ./main  

# Windows下(前提:安装Visual Studio并且安装cmake插件)  
工程编译与执行:  
1>  打开Visual Studio,进入终端(Ctrl + `)  
1>  cd 本目录  
3>  cmake ..  
本目录下生成解决方案  
双击运行解决方案...  
设置app_main为启动项目  
编译结束后app_!cmake/app_main下生成可执行文件  
直接执行的是调试模式的可执行文件  
