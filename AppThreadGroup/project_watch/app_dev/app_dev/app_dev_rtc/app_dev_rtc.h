#ifndef APP_DEV_RTC_H
#define APP_DEV_RTC_H

/* 设备rtc抽象操作接口 */
typedef struct {
    void (*ready)(app_dev_t *driver);
    void (*irq_cb_reg)(app_dev_t *driver, void(*cb)(void));
    void (*irq_switch)(app_dev_t *driver, bool flag);
    void (*get_utc)(app_dev_t *driver, uint64_t *utc, uint64_t *ms);
    void (*set_utc)(app_dev_t *driver, uint64_t *utc, uint64_t *ms);
    void (*sync)(app_dev_t *driver);
} app_dev_rtc_api_t;

/* 设备rtc抽象操作数据 */
typedef struct {
    uint64_t utc;
    uint64_t utc_ms;
    /* RTC事件通报回调 */
    void (*irq_cb)(void);
} app_dev_rtc_data_t;

/*@brief rtc设备就绪
 *@param driver 设备实例
 */
static inline void app_dev_rtc_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_rtc_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief rtc设备事件通报回调注册
 *@param driver 设备实例
 *@param cb     事件通报回调
 */
static inline void app_dev_rtc_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_rtc_api_t *api = driver->api;
        api->irq_cb_reg(driver, cb);
    }
}

/*@brief rtc设备事件通报开关
 *@param driver 设备实例
 *@param flag   true:开启事件通报;false:关闭事件通报
 */
static inline void app_dev_rtc_irq_switch(app_dev_t *driver, bool flag)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_rtc_api_t *api = driver->api;
        api->irq_switch(driver, flag);
    }
}

/*@brief rtc设备获取utc
 *@param driver 设备实例
 *@param utc    标准时
 *@param utc_ms 标准时(ms余留)
 */
static inline void app_dev_rtc_get_utc(app_dev_t *driver, uint64_t *utc, uint64_t *utc_ms)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_rtc_api_t *api = driver->api;
        api->get_utc(driver, utc, utc_ms);
    }
}

/*@brief rtc设备设置utc
 *@param driver 设备实例
 *@param utc    标准时
 *@param utc_ms 标准时(ms余留)
 */
static inline void app_dev_rtc_set_utc(app_dev_t *driver, uint64_t *utc, uint64_t *utc_ms)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_rtc_api_t *api = driver->api;
        api->set_utc(driver, utc, utc_ms);
    }
}

/*@brief rtc设备同步
 *@param driver 设备实例
 */
static inline void app_dev_rtc_sync(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_rtc_api_t *api = driver->api;
        api->sync(driver);
    }
}

#endif
