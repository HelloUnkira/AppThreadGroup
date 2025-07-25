/* 实现目标:
 *     模拟看门狗
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

#if APP_ARCH_IS_PC

/* 设备watchdog抽象操作参数 */
typedef struct {
    void *args;
} app_dev_wdt_cfg_t;

/*@brief watchdog设备打开
 *@param driver 设备实例
 */
static void app_dev_wdt_hal_ready(app_dev_t *driver)
{
    app_dev_wdt_cfg_t *cfg = driver->cfg;
    app_dev_wdt_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief watchdog设备关闭
 *@param driver 设备实例
 */
static void app_dev_wdt_hal_feed(app_dev_t *driver)
{
    app_dev_wdt_cfg_t *cfg = driver->cfg;
    app_dev_wdt_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/* 静态配置的设备操作参数 */
static app_dev_wdt_cfg_t app_dev_wdt_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_wdt_api_t app_dev_wdt_api = {
    .ready  = app_dev_wdt_hal_ready,
    .feed   = app_dev_wdt_hal_feed,
};

/* 动态的设备操作数据 */
static app_dev_wdt_data_t app_dev_wdt_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_wdt = {
    .name = "app_dev_wdt",
    .cfg  = &app_dev_wdt_cfg,
    .api  = &app_dev_wdt_api,
    .data = &app_dev_wdt_data,
};

#endif
