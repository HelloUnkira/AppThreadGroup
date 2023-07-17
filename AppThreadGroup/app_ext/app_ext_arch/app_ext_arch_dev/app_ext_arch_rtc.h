#ifndef APP_EXT_ARCH_RTC_H
#define APP_EXT_ARCH_RTC_H

/* 设备rtc抽象操作接口 */
typedef struct {
    void (*ready)(app_arch_dev_t *driver);
    void (*irq_cb_reg)(app_arch_dev_t *driver, void(*cb)(void));
    void (*execute)(app_arch_dev_t *driver);
    void (*get_utc)(app_arch_dev_t *driver, uint64_t *utc);
    void (*set_utc)(app_arch_dev_t *driver, uint64_t *utc);
} app_arch_rtc_api_t;

/* 设备rtc抽象操作数据 */
typedef struct {
    uint64_t utc;
    /* RTC事件通报回调 */
    void (*irq_cb)(void);
} app_arch_rtc_data_t;

/*@brief     rtc设备就绪
 *@param[in] driver 设备实例
 */
static inline void app_arch_rtc_ready(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_rtc_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief     rtc设备事件通报回调注册
 *@param[in] driver 设备实例
 *@param[in] cb     充电事件通报回调
 */
static inline void app_arch_rtc_irq_cb_reg(app_arch_dev_t *driver, void(*cb)(void))
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_rtc_api_t *api = driver->api;
        api->irq_cb_reg(driver, cb);
    }
}

/*@brief     rtc设备运行
 *@param[in] driver 设备实例
 */
static inline void app_arch_rtc_execute(app_arch_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_rtc_api_t *api = driver->api;
        api->execute(driver);
    }
}

/*@brief     rtc设备获取utc
 *@param[in] driver 设备实例
 */
static inline void app_arch_rtc_get_utc(app_arch_dev_t *driver, uint64_t *utc)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_rtc_api_t *api = driver->api;
        api->get_utc(driver, utc);
    }
}

/*@brief     rtc设备设置utc
 *@param[in] driver 设备实例
 */
static inline void app_arch_rtc_set_utc(app_arch_dev_t *driver, uint64_t *utc)
{
    if (driver != NULL && driver->api != NULL) {
        const app_arch_rtc_api_t *api = driver->api;
        api->set_utc(driver, utc);
    }
}

#endif
