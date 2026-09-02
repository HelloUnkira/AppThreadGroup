@echo off
rem scui_pack_tools.exe 构建脚本(PyInstaller, 单文件免安装)
rem 输出到本目录: scui_pack_tools.exe
setlocal
cd /d "%~dp0"

rem 前置依赖
py -3 -m pip install --upgrade pip
py -3 -m pip install pyinstaller py7zr openpyxl

rem 打单文件 exe; 各打包脚本作为模块打进(scui_pack_tools 为唯一入口)
py -3 -m PyInstaller --noconsole --onefile --name scui_pack_tools ^
    --paths "%~dp0." ^
    --hidden-import scui_pack_image ^
    --hidden-import scui_pack_font ^
    --hidden-import scui_pack_lang ^
    --hidden-import scui_pack_widget ^
    --hidden-import scui_pack_cwf ^
    --hidden-import py7zr ^
    --hidden-import openpyxl ^
    "%~dp0scui_pack_tools.py"

if exist "%~dp0dist\scui_pack_tools.exe" (
    move /y "%~dp0dist\scui_pack_tools.exe" "%~dp0scui_pack_tools.exe" >nul
    echo.
    echo [build] exe: %~dp0scui_pack_tools.exe
)

rem 清理构建产物(cache/子文件不留)
rmdir /s /q "%~dp0build" 2>nul
rmdir /s /q "%~dp0dist" 2>nul
rmdir /s /q "%~dp0__pycache__" 2>nul
del /q "%~dp0scui_pack_tools.spec" 2>nul

echo.
pause
endlocal