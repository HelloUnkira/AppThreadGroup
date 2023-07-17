#ifndef APP_EXT_ARCH_BACKLIGHT_H
#define APP_EXT_ARCH_BACKLIGHT_H

/* 设备backlight抽象操作接口 */
typedef struct {
    void (*ready)(app_arch_dev_t *driver);
    void (*update)(app_arch_dev_t *driver, uint8_t duty_cycle);
    void (*dlps_enter)(app_arch_dev_t *driver);
    void (*dlps_exit)(app_arch_dev_t *driver);
    void (*shutdown_enter)(app_arch_dev_t *driver);
    void (*shutdown_exit)(app_arch_dev_t *driver);
} app_arch_backlight_api_t;

/* 设备backlight抽象操作数据 */
typedef struct {
    void *data;
} app_arch_backlight_data_t;

/*@brief     backlight设备初始化
 *@param[in] driver 设备实例
 */
static inline void app_arch_backlight_ready(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_backlight_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief     backlight设备更新占空比
 *@param[in] driver 设备实例
 *@param[in] duty_cycle 百分比振幅[0,100]
 */
static inline void app_arch_backlight_update(app_arch_dev_t *driver, uint8_t duty_cycle)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_backlight_api_t *api = driver->api;
        api->update(driver, duty_cycle);
    }
}

/*@brief     backlight设备进入dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_backlight_dlps_enter(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_backlight_api_t *api = driver->api;
        api->dlps_enter(driver);
    }
}

/*@brief     backlight设备退出dlps模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_backlight_dlps_exit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_backlight_api_t *api = driver->api;
        api->dlps_exit(driver);
    }
}

/*@brief     backlight设备进入shutdown模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_backlight_shutdown_enter(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_backlight_api_t *api = driver->api;
        api->shutdown_enter(driver);
    }
}

/*@brief     backlight设备退出shutdown模式
 *@param[in] driver 设备实例
 */
static inline void app_arch_backlight_shutdown_exit(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_backlight_api_t *api = driver->api;
        api->shutdown_exit(driver);
    }
}

#endif
