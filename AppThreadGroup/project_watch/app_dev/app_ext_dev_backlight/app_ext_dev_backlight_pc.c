/* 实现目标:
 *     模拟背光灯
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_ARCH_IS_PC

/* 设备backlight抽象操作参数 */
typedef struct {
    void *args;
} app_dev_backlight_cfg_t;

/*@brief     backlight设备打开
 *@param[in] driver 设备实例
 */
static void app_dev_backlight_hal_open(app_dev_t *driver)
{
    const app_dev_backlight_cfg_t *cfg = driver->cfg;
    app_dev_backlight_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    APP_SYS_LOG_INFO("open");
}

/*@brief     backlight设备关闭
 *@param[in] driver 设备实例
 */
static void app_dev_backlight_hal_close(app_dev_t *driver)
{
    const app_dev_backlight_cfg_t *cfg = driver->cfg;
    app_dev_backlight_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    APP_SYS_LOG_INFO("close");
}

/*@brief     backlight设备更新占空比
 *@param[in] driver 设备实例
 *@param[in] duty_ratio 百分比振幅[0,100]
 */
static void app_dev_backlight_hal_update(app_dev_t *driver, uint8_t duty_cycle)
{
    const app_dev_backlight_cfg_t *cfg = driver->cfg;
    app_dev_backlight_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/* 静态配置的设备操作参数 */
static const app_dev_backlight_cfg_t app_dev_backlight_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_backlight_api_t app_dev_backlight_api = {
    .open       = app_dev_backlight_hal_open,
    .close      = app_dev_backlight_hal_close,
    .update     = app_dev_backlight_hal_update,
};

/* 动态的设备操作数据 */
static app_dev_backlight_data_t app_dev_backlight_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_backlight = {
    .name = "app_dev_backlight",
    .cfg  = &app_dev_backlight_cfg,
    .api  = &app_dev_backlight_api,
    .data = &app_dev_backlight_data,
};

#endif