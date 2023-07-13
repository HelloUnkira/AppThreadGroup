/* 实现目标:
 *     模拟温度计
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备temperature抽象操作参数 */
typedef struct {
    void *args;
} app_arch_temperature_cfg_t;

/*@brief     temperature设备X毫秒定时器回调,手动调用使用
 *@param[in] driver 设备实例
 *@retval    测量是否完毕
 */
static bool app_arch_temperature_hal_get_value(app_arch_dev_t *driver)
{
    const app_arch_temperature_cfg_t *cfg = driver->cfg;
    app_arch_temperature_data_t *data = driver->data;
    /* 这里伪造一下数据即可,因为是模拟 */
    return 36.0 + rand() % 6000 / 1.0e-3;
}

/* 静态配置的设备操作参数 */
static const app_arch_temperature_cfg_t app_arch_temperature_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_arch_temperature_api_t app_arch_temperature_api = {
    .get_value = app_arch_temperature_hal_get_value,
};

/* 动态的设备操作数据 */
static app_arch_temperature_data_t app_arch_temperature_data = {
    .data = NULL,
};

/* 静态配置的设备实例 */
const app_arch_dev_t app_arch_temperature = {
    .name = "app_arch_temperature",
    .cfg  = &app_arch_temperature_cfg,
    .api  = &app_arch_temperature_api,
    .data = &app_arch_temperature_data,
};

#endif
