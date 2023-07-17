/* 实现目标:
 *     模拟温度计
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备temperature抽象操作参数 */
typedef struct {
    void *args;
} app_dev_temperature_cfg_t;

/*@brief     temperature设备X毫秒定时器回调,手动调用使用
 *@param[in] driver 设备实例
 *@retval    测量是否完毕
 */
static float app_dev_temperature_hal_get_value(app_dev_t *driver)
{
    const app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 这里伪造一下数据即可,因为是模拟 */
    return 36.0 + rand() % 6000 * 1.0e-3;
}

/*@brief     temperature设备进入dlps模式
 *@param[in] driver 设备实例
 */
static void app_dev_temperature_hal_dlps_enter(app_dev_t *driver)
{
    const app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     temperature设备退出dlps模式
 *@param[in] driver 设备实例
 */
static void app_dev_temperature_hal_dlps_exit(app_dev_t *driver)
{
    const app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     temperature设备进入shutdown模式
 *@param[in] driver 设备实例
 */
static void app_dev_temperature_hal_shutdown_enter(app_dev_t *driver)
{
    const app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief     temperature设备退出shutdown模式
 *@param[in] driver 设备实例
 */
static void app_dev_temperature_hal_shutdown_exit(app_dev_t *driver)
{
    const app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/* 静态配置的设备操作参数 */
static const app_dev_temperature_cfg_t app_dev_temperature_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_temperature_api_t app_dev_temperature_api = {
    .get_value      = app_dev_temperature_hal_get_value,
    .dlps_enter     = app_dev_temperature_hal_dlps_enter,
    .dlps_exit      = app_dev_temperature_hal_dlps_exit,
    .shutdown_enter = app_dev_temperature_hal_shutdown_enter,
    .shutdown_exit  = app_dev_temperature_hal_shutdown_exit,
};

/* 动态的设备操作数据 */
static app_dev_temperature_data_t app_dev_temperature_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_temperature = {
    .name = "app_dev_temperature",
    .cfg  = &app_dev_temperature_cfg,
    .api  = &app_dev_temperature_api,
    .data = &app_dev_temperature_data,
};

#endif
