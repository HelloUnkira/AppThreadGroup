#ifndef APP_DEV_WDT_H
#define APP_DEV_WDT_H

/* 设备watchdog抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*feed)(app_dev_t *driver);
} app_dev_wdt_api_t;

/* 设备watchdog抽象操作数据 */
typedef struct {
    void *data;
} app_dev_wdt_data_t;

/*@brief watchdog设备就绪
 *@param driver 设备实例
 */
static inline void app_dev_wdt_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_wdt_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief watchdog设备喂狗
 *@param driver 设备实例
 */
static inline void app_dev_wdt_feed(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_wdt_api_t *api = driver->api;
        api->feed(driver);
    }
}

#endif
