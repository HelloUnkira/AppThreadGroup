#ifndef APP_EXT_DEV_BACKLIGHT_H
#define APP_EXT_DEV_BACKLIGHT_H

/* 设备backlight抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*update)(app_dev_t *driver, uint8_t duty_cycle);
} app_dev_backlight_api_t;

/* 设备backlight抽象操作数据 */
typedef struct {
    void *data;
} app_dev_backlight_data_t;

/*@brief     backlight设备初始化
 *@param[in] driver 设备实例
 */
static inline void app_dev_backlight_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_backlight_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief     backlight设备更新占空比
 *@param[in] driver 设备实例
 *@param[in] duty_cycle 百分比振幅[0,100]
 */
static inline void app_dev_backlight_update(app_dev_t *driver, uint8_t duty_cycle)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_backlight_api_t *api = driver->api;
        api->update(driver, duty_cycle);
    }
}

#endif
