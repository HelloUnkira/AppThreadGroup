/* 实现目标:
 *     模拟LOG输出
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备log抽象操作参数 */
typedef struct {
    void *args;
} app_arch_log_cfg_t;

/*@brief     log设备就绪
 *@param[in] driver 设备实例
 */
static void app_arch_log_hal_ready(app_arch_dev_t *driver)
{
    const app_arch_log_cfg_t *cfg = driver->cfg;
    app_arch_log_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     log设备输出
 *@param[in] driver 设备实例
 *@param[in] format 输出变参格式化字符串
 *@param[in] list   输出变参列表
 */
static void app_arch_log_hal_msg(app_arch_dev_t *driver, const char *format, va_list list)
{
    const app_arch_log_cfg_t *cfg = driver->cfg;
    app_arch_log_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    vprintf(format, list);
}

/* 静态配置的设备操作参数 */
static const app_arch_log_cfg_t app_arch_log_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_arch_log_api_t app_arch_log_api = {
    .ready  = app_arch_log_hal_ready,
    .msg    = app_arch_log_hal_msg,
};

/* 动态的设备操作数据 */
static app_arch_log_data_t app_arch_log_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_arch_dev_t app_arch_log = {
    .name = "app_arch_log",
    .cfg  = &app_arch_log_cfg,
    .api  = &app_arch_log_api,
    .data = &app_arch_log_data,
};

#endif
