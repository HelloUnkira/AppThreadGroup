
# 注意:  
# app_main/app_source/*下是一些需要的资源文件  
# 它与可执行文件应该在同一文件夹下面  

# Linux下(前提:安装cmake和sdl):  
# 工程编译与执行:  
# cd    Watch  
# mkdir app_\!cmake  
# cd    app_\!cmake  
# cmake ..  
# make  
# cd    ../app_main/app_exe  
# ./main  

# Windows下(前提:安装Visual Studio并且安装cmake插件)  
# 工程编译与执行:  
# 打开Visual Studio,进入终端(Ctrl + `)  
# mkdir app_\!cmake  
# cd    app_\!cmake  
# cmake ..  
# app_!cmake下生成解决方案  
# 双击运行解决方案...  
# 设置app_main为启动项目  
# 编译结束后app_!cmake/app_main下生成可执行文件  
