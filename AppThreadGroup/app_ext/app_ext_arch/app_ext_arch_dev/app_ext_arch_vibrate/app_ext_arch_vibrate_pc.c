/* 实现目标:
 *     模拟震动
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备vibrate抽象操作参数 */
typedef struct {
    void *args;
} app_arch_vibrate_cfg_t;

/*@brief     vibrate设备打开
 *@param[in] driver 设备实例
 */
static void app_arch_vibrate_hal_open(app_arch_dev_t *driver)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     vibrate设备关闭
 *@param[in] driver 设备实例
 */
static void app_arch_vibrate_hal_close(app_arch_dev_t *driver)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     vibrate设备更新占空比
 *@param[in] driver 设备实例
 *@param[in] duty_ratio 百分比振幅[0,100]
 */
static void app_arch_vibrate_hal_update(app_arch_dev_t *driver, uint8_t duty_ratio)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     vibrate设备进入dlps模式
 *@param[in] driver 设备实例
 */
static void app_arch_vibrate_hal_dlps_enter(app_arch_dev_t *driver)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     vibrate设备退出dlps模式
 *@param[in] driver 设备实例
 */
static void app_arch_vibrate_hal_dlps_exit(app_arch_dev_t *driver)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     vibrate设备进入shutdown模式
 *@param[in] driver 设备实例
 */
static void app_arch_vibrate_hal_shutdown_enter(app_arch_dev_t *driver)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     vibrate设备退出shutdown模式
 *@param[in] driver 设备实例
 */
static void app_arch_vibrate_hal_shutdown_exit(app_arch_dev_t *driver)
{
    const app_arch_vibrate_cfg_t *cfg = driver->cfg;
    app_arch_vibrate_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/* 静态配置的设备操作参数 */
static const app_arch_vibrate_cfg_t app_arch_vibrate_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_arch_vibrate_api_t app_arch_vibrate_api = {
    .open               = app_arch_vibrate_hal_open,
    .close              = app_arch_vibrate_hal_close,
    .update             = app_arch_vibrate_hal_update,
    .dlps_enter         = app_arch_vibrate_hal_dlps_enter,
    .dlps_exit          = app_arch_vibrate_hal_dlps_exit,
    .shutdown_enter     = app_arch_vibrate_hal_shutdown_enter,
    .shutdown_exit      = app_arch_vibrate_hal_shutdown_exit,
};

/* 动态的设备操作数据 */
static app_arch_vibrate_data_t app_arch_vibrate_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_arch_dev_t app_arch_vibrate = {
    .name = "app_arch_vibrate",
    .cfg  = &app_arch_vibrate_cfg,
    .api  = &app_arch_vibrate_api,
    .data = &app_arch_vibrate_data,
};

#endif
