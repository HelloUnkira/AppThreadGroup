#ifndef APP_EXT_DEV_VIBRATE_H
#define APP_EXT_DEV_VIBRATE_H

/* 设备vibrate抽象操作接口 */
typedef struct {
    void (*open)(app_dev_t *driver);
    void (*close)(app_dev_t *driver);
    void (*update)(app_dev_t *driver, uint8_t duty_ratio);
    void (*dlps_enter)(app_dev_t *driver);
    void (*dlps_exit)(app_dev_t *driver);
} app_dev_vibrate_api_t;

/* 设备vibrate抽象操作数据 */
typedef struct {
    void *data;
} app_dev_vibrate_data_t;

/*@brief     vibrate设备打开
 *@param[in] driver 设备实例
 */
static inline void app_dev_vibrate_open(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->open(driver);
    }
}

/*@brief     vibrate设备关闭
 *@param[in] driver 设备实例
 */
static inline void app_dev_vibrate_close(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->close(driver);
    }
}

/*@brief     vibrate设备更新占空比
 *@param[in] driver 设备实例
 *@param[in] duty_ratio 百分比振幅[0,100]
 */
static inline void app_dev_vibrate_update(app_dev_t *driver, uint8_t duty_ratio)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->update(driver, duty_ratio);
    }
}

/*@brief     vibrate设备进入dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_dev_vibrate_dlps_enter(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->dlps_enter(driver);
    }
}

/*@brief     vibrate设备退出dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_dev_vibrate_dlps_exit(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_vibrate_api_t *api = driver->api;
        api->dlps_exit(driver);
    }
}

#endif
