#ifndef APP_DEV_VIBRATE_H
#define APP_DEV_VIBRATE_H

/* 设备vibrate抽象操作接口 */
typedef struct {
    void (*open)(app_dev_t *driver);
    void (*close)(app_dev_t *driver);
    void (*update)(app_dev_t *driver, uint8_t duty_ratio);
} app_dev_vibrate_api_t;

/* 设备vibrate抽象操作数据 */
typedef struct {
    void *data;
} app_dev_vibrate_data_t;

/*@brief vibrate设备打开
 *@param driver 设备实例
 */
static inline void app_dev_vibrate_open(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->open(driver);
    }
}

/*@brief vibrate设备关闭
 *@param driver 设备实例
 */
static inline void app_dev_vibrate_close(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->close(driver);
    }
}

/*@brief vibrate设备更新占空比
 *@param driver 设备实例
 *@param duty_ratio 百分比振幅[0,100]
 */
static inline void app_dev_vibrate_update(app_dev_t *driver, uint8_t duty_ratio)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->update(driver, duty_ratio);
    }
}

#endif
