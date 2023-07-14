#ifndef APP_EXT_ARCH_TEMPERATURE_H
#define APP_EXT_ARCH_TEMPERATURE_H

/* 设备temperature抽象操作接口 */
typedef struct {
    float (*get_value)(app_arch_dev_t *driver);
    void  (*dlps_enter)(app_arch_dev_t *driver);
    void  (*dlps_exit)(app_arch_dev_t *driver);
    void  (*shutdown_enter)(app_arch_dev_t *driver);
    void  (*shutdown_exit)(app_arch_dev_t *driver);
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

/*@brief     temperature设备进入dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_temperature_dlps_enter(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temperature_api_t *api = driver->api;
        api->dlps_enter(driver);
    }
}

/*@brief     temperature设备退出dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_temperature_dlps_exit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temperature_api_t *api = driver->api;
        api->dlps_exit(driver);
    }
}

/*@brief     temperature设备进入shutdown模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_temperature_shutdown_enter(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temperature_api_t *api = driver->api;
        api->shutdown_enter(driver);
    }
}

/*@brief     temperature设备退出shutdown模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_temperature_shutdown_exit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_temperature_api_t *api = driver->api;
        api->shutdown_exit(driver);
    }
}

#endif
