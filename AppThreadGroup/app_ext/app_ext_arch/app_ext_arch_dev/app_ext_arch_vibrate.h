#ifndef APP_EXT_ARCH_VIBRATE_H
#define APP_EXT_ARCH_VIBRATE_H

/* 设备vibrate抽象操作接口 */
typedef struct {
    void (*open)(app_arch_dev_t *driver);
    void (*close)(app_arch_dev_t *driver);
    void (*update)(app_arch_dev_t *driver, uint8_t duty_ratio);
    void (*dlps_enter)(app_arch_dev_t *driver);
    void (*dlps_exit)(app_arch_dev_t *driver);
    void (*shutdown_enter)(app_arch_dev_t *driver);
    void (*shutdown_exit)(app_arch_dev_t *driver);
} app_arch_vibrate_api_t;

/* 设备vibrate抽象操作数据 */
typedef struct {
    void *data;
} app_arch_vibrate_data_t;

/*@brief     vibrate设备打开
 *@param[in] driver 设备实例
 */
static inline void app_arch_vibrate_open(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->open(driver);
    }
}

/*@brief     vibrate设备关闭
 *@param[in] driver 设备实例
 */
static inline void app_arch_vibrate_close(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->close(driver);
    }
}

/*@brief     vibrate设备更新占空比
 *@param[in] driver 设备实例
 *@param[in] duty_ratio 百分比振幅[0,100]
 */
static inline void app_arch_vibrate_update(app_arch_dev_t *driver, uint8_t duty_ratio)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->update(driver, duty_ratio);
    }
}

/*@brief     vibrate设备进入dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_vibrate_dlps_enter(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->dlps_enter(driver);
    }
}

/*@brief     vibrate设备退出dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_vibrate_dlps_exit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->dlps_exit(driver);
    }
}

/*@brief     vibrate设备进入shutdown模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_vibrate_shutdown_enter(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->shutdown_enter(driver);
    }
}

/*@brief     vibrate设备退出shutdown模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_vibrate_shutdown_exit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_vibrate_api_t *api = driver->api;
        api->shutdown_exit(driver);
    }
}

#endif