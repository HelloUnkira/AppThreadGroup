@echo off
:: 云表盘cwf批量打包
:: 配置: 下方WATCHFACE_LIST追加表盘目录名, 注释掉某行即禁用该表盘
:: 流程: 解压image.7z、图片打包、cwf打包、清理临时文件

setlocal

set WATCHFACE_LIST=
set WATCHFACE_LIST=%WATCHFACE_LIST% D10450001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10597001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10598001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10599001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10600001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10601001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10602001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10603001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10604001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10705001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10706001
set WATCHFACE_LIST=%WATCHFACE_LIST% D10707001

echo.
echo ==============================================
echo [cwf_json] 云表盘 cwf 批量打包
echo ==============================================
echo.
echo [cwf_json] watchface list: %WATCHFACE_LIST%
echo.

for %%W in (%WATCHFACE_LIST%) do call :parse_watchface %%W

echo.
echo ==============================================
echo [cwf_json] all watchface parser finish
echo ==============================================
pause
exit /b

:: 单表盘打包流程
:parse_watchface
set WATCHFACE=%1
echo.
echo ============ [%WATCHFACE%] parser start ============
echo.

pushd "%~dp0%WATCHFACE%"
if errorlevel 1 (
    echo [cwf_json] watchface dir not exist: %WATCHFACE%
    exit /b 1
)

echo --- [%WATCHFACE%] unzip image.7z ---
rmdir /s /q image 2>nul
python.exe -m py7zr x image.7z
if errorlevel 1 (
    echo [cwf_json] unzip fail: %WATCHFACE%
    popd
    exit /b 1
)
echo.

echo --- [%WATCHFACE%] step1 image parser ---
echo.|python.exe ..\..\..\scui\tools\scui_image_parser.py .\image . cwf
if errorlevel 1 (
    echo [cwf_json] image parser fail: %WATCHFACE%
    popd
    exit /b 1
)
echo.

echo --- [%WATCHFACE%] step2 cwf json parser ---
echo.|python.exe ..\..\..\scui_plugs\scui_cwf_json_parser.py . ..\bin
if errorlevel 1 (
    echo [cwf_json] cwf parser fail: %WATCHFACE%
    popd
    exit /b 1
)
echo.

echo --- [%WATCHFACE%] cleanup ---
rmdir /s /q image 2>nul
rmdir /s /q image_array 2>nul
del /q scui_image_parser.h scui_image_parser.c scui_image_parser.bin scui_image_parser.out scui_image_parser.err 2>nul
del /q ..\bin\%WATCHFACE%_json.prog 2>nul

popd
echo ============ [%WATCHFACE%] parser finish ============
echo.
exit /b 0
