#ifndef APP_EXT_DEV_TEMPERATURE_H
#define APP_EXT_DEV_TEMPERATURE_H

/* 设备temperature抽象操作接口 */
typedef struct {
    float (*get_value)(app_dev_t *driver);
    void  (*dlps_exec)(app_dev_t *driver, bool dlps, uint8_t mode);
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

/*@brief     temperature设备进出dlps模式
 *@param[in] driver 设备实例
 *@param[in] dlps   true:进入dlps模式;false:退出dlps模式
 *@param[in] mode   other:正常模式;1:低电量模式;2:关机模式
 */
static inline void app_dev_temperature_dlps_exec(app_dev_t *driver, bool dlps, uint8_t mode)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_temperature_api_t *api = driver->api;
        api->dlps_exec(driver, dlps, mode);
    }
}

#endif