/* 实现目标: PC 控制台输入轮次 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

#if APP_ARCH_IS_PC

#include <windows.h>

typedef struct {
    void *args;
} app_dev_shell_cfg_t;

/*@brief shell设备初始化
 *@param driver 设备实例
 */
static void app_dev_shell_hal_ready(app_dev_t *driver)
{
    app_dev_shell_data_t *data = driver->data;
    
    AllocConsole();
    data->hconin  = CreateFile("CONIN$",  GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, 0, NULL);
    data->hconout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, 0, NULL);
    data->prompt = 0;
}

/*@brief shell设备读取(输入轮次控制)
 *        prompt=0: 等待回车进入输入轮次
 *        prompt=1: 输入轮次中, 逐字回显, 回车提交
 *@param driver 设备实例
 *@param prompt 输出:当前输入轮次状态
 */
static void app_dev_shell_hal_read(app_dev_t *driver, char *buf, uint32_t size, uint32_t *len, uint32_t *prompt)
{
    *len = 0;
    app_dev_shell_data_t *data = driver->data;
    *prompt = data->prompt;
    DWORD w;
    
    DWORD count;
    if (!GetNumberOfConsoleInputEvents(data->hconin, &count) || count == 0)
        return;
    
    INPUT_RECORD ir;
    DWORD read;
    
    while (count > 0) {
        if (!ReadConsoleInput(data->hconin, &ir, 1, &read) || read == 0)
            return;
        
        if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            char ch = ir.Event.KeyEvent.uChar.AsciiChar;
            
            if (ch == '\r' || ch == '\n') {
                if (data->prompt == 0) {
                    /* 回车进入输入轮次 */
                    data->prompt = 1;
                    *prompt = 1;
                    WriteFile(data->hconout, "> ", 2, &w, NULL);
                } else {
                    /* 回车提交,退出输入轮次 */
                    WriteFile(data->hconout, "\r\n", 2, &read, NULL);
                    data->prompt = 0;
                    *prompt = 0;
                    if (data->len > 0) {
                        uint32_t copy = data->len < size ? data->len : size - 1;
                        memcpy(buf, data->buf, copy);
                        buf[copy] = '\0';
                        *len = copy;
                        data->len = 0;
                    }
                }
                return;
            }
            if (data->prompt == 1) {
                if (ch == '\b' && data->len > 0) {
                    data->len--;
                    WriteFile(data->hconout, "\b \b", 3, &read, NULL);
                } else if ((unsigned char)ch >= ' ') {
                    if (data->len < sizeof(data->buf) - 1) {
                        data->buf[data->len++] = ch;
                        WriteFile(data->hconout, &ch, 1, &w, NULL);
                    }
                }
            }
        }
        GetNumberOfConsoleInputEvents(data->hconin, &count);
    }
}

/* 静态配置的设备操作参数 */
static app_dev_shell_cfg_t app_dev_shell_cfg = {
    .args = NULL,
};

/*@brief shell设备写入(绕过 log work 开关)
 *@param driver 设备实例
 */
static void app_dev_shell_hal_write(app_dev_t *driver, const char *buf, uint32_t size)
{
    app_dev_shell_data_t *data = driver->data;
    DWORD w;
    WriteFile(data->hconout, buf, size, &w, NULL);
}

/* 静态配置的设备操作集合 */
static const app_dev_shell_api_t app_dev_shell_api = {
    .ready = app_dev_shell_hal_ready,
    .read  = app_dev_shell_hal_read,
    .write = app_dev_shell_hal_write,
};

/* 动态的设备操作数据 */
static app_dev_shell_data_t app_dev_shell_data = {
    .hconin  = NULL,
    .hconout = NULL,
    .prompt  = 0,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_shell = {
    .name = "shell",
    .cfg  = &app_dev_shell_cfg,
    .api  = &app_dev_shell_api,
    .data = &app_dev_shell_data,
};

#endif
