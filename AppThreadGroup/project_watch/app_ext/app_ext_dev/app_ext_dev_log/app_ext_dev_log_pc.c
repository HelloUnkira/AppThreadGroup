/* 实现目标:
 *     模拟LOG输出
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备log抽象操作参数 */
typedef struct {
    void *args;
} app_dev_log_cfg_t;

/*@brief log设备就绪
 *@param driver 设备实例
 */
static void app_dev_log_hal_ready(app_dev_t *driver)
{
    app_dev_log_cfg_t *cfg = driver->cfg;
    app_dev_log_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief log设备输出
 *@param driver 设备实例
 *@param format 输出变参格式化字符串
 *@param list   输出变参列表
 */
static void app_dev_log_hal_msg(app_dev_t *driver, const char *format, va_list list)
{
    app_dev_log_cfg_t *cfg = driver->cfg;
    app_dev_log_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    vprintf(format, list);
    
    static bool not_ready = true;
    /* 将日志转存到文件中,方便回溯 */
    if (not_ready) {
        not_ready = false;
        FILE *file = fopen("app_thread_group(log).txt", "w");
        fclose(file);
    }
    
    FILE *file = fopen("app_thread_group(log).txt", "a+");
    vfprintf(file, format, list);
    fclose(file);
}

/* 静态配置的设备操作参数 */
static app_dev_log_cfg_t app_dev_log_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_log_api_t app_dev_log_api = {
    .ready  = app_dev_log_hal_ready,
    .msg    = app_dev_log_hal_msg,
};

/* 动态的设备操作数据 */
static app_dev_log_data_t app_dev_log_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_log = {
    .name = "app_dev_log",
    .cfg  = &app_dev_log_cfg,
    .api  = &app_dev_log_api,
    .data = &app_dev_log_data,
};

#endif
