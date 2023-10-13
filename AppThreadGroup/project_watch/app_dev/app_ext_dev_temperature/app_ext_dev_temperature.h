#ifndef APP_EXT_DEV_TEMPERATURE_H
#define APP_EXT_DEV_TEMPERATURE_H

/* 设备temperature抽象操作接口 */
typedef struct {
    float (*get_value)(app_dev_t *driver);
    void  (*dlps_enter)(app_dev_t *driver, uint8_t mode);
    void  (*dlps_exit)(app_dev_t *driver,  uint8_t mode);
} app_dev_temperature_api_t;

/* 设备temperature抽象操作数据 */
typedef struct {
    void *data;
} app_dev_temperature_data_t;

/*@brief     temperature设备获得当前温度值
 *@param[in] driver 设备实例
 *@retval    当前温度值
 */
static inline float app_dev_temperature_get_value(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_temperature_api_t *api = driver->api;
        return api->get_value(driver);
    }
    return 0.0;
}

/*@brief     temperature设备进入dlps模式
 *@param[in] driver 设备实例
 *@param[in] mode   other:正常模式;1:低电量模式;2:关机模式
 */
static inline void app_dev_temperature_dlps_enter(app_dev_t *driver, uint8_t mode)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_temperature_api_t *api = driver->api;
        api->dlps_enter(driver, mode);
    }
}

/*@brief     temperature设备退出dlps模式
 *@param[in] driver 设备实例
 *@param[in] mode   other:正常模式;1:低电量模式;2:关机模式
 */
static inline void app_dev_temperature_dlps_exit(app_dev_t *driver, uint8_t mode)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_temperature_api_t *api = driver->api;
        api->dlps_exit(driver, mode);
    }
}

#endif
