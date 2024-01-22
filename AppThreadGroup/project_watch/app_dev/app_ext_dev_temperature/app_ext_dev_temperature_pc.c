/* 实现目标:
 *     模拟温度计
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_ARCH_IS_PC

/* 设备temperature抽象操作参数 */
typedef struct {
    void *args;
} app_dev_temperature_cfg_t;

/*@brief temperature设备X毫秒定时器回调,手动调用使用
 *@param driver 设备实例
 *@retval 测量是否完毕
 */
static float app_dev_temperature_hal_get_value(app_dev_t *driver)
{
    app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 这里伪造一下数据即可,因为是模拟 */
    return 36.0 + rand() % 6000 * 1.0e-3;
}

/*@brief temperature设备进出dlps模式
 *@param driver 设备实例
 *@param dlps   true:进入dlps模式;false:退出dlps模式
 *@param mode   other:正常模式;1:低电量模式;2:关机模式
 */
static void app_dev_temperature_hal_dlps_exec(app_dev_t *driver, bool dlps, uint8_t mode)
{
    app_dev_temperature_cfg_t *cfg = driver->cfg;
    app_dev_temperature_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    APP_SYS_LOG_INFO("dlps enter exit:%d:%d", dlps, mode);
}

/* 静态配置的设备操作参数 */
static app_dev_temperature_cfg_t app_dev_temperature_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_temperature_api_t app_dev_temperature_api = {
    .get_value      = app_dev_temperature_hal_get_value,
    .dlps_exec      = app_dev_temperature_hal_dlps_exec,
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
