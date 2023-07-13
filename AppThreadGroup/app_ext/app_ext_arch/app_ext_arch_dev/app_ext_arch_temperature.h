#ifndef APP_EXT_ARCH_TEMPERATURE_H
#define APP_EXT_ARCH_TEMPERATURE_H

/* 设备temperature抽象操作接口 */
typedef struct {
    float (*get_value)(app_arch_dev_t *driver);
} app_arch_temperature_api_t;

/* 设备temperature抽象操作数据 */
typedef struct {
    void *data;
} app_arch_temperature_data_t;

/*@brief     temperature设备获得当前温度值
 *@param[in] driver 设备实例
 *@retval    当前温度值
 */
static inline float app_arch_temperature_get_value(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temperature_api_t *api = driver->api;
        return api->get_value(driver);
    }
    return 0.0;
}

#endif
