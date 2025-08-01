#ifndef APP_DEV_BATTERY_H
#define APP_DEV_BATTERY_H

/* 设备battery抽象操作接口 */
typedef struct {
    void     (*ready)(app_dev_t *driver);
    uint8_t  (*charge_full)(app_dev_t *driver);
    uint8_t  (*charge_status)(app_dev_t *driver);
    void     (*charge_irq_cb_reg)(app_dev_t *driver, void(*cb)(void));
    void     (*charge_irq_switch)(app_dev_t *driver, bool flag);
    uint32_t (*voltage_measure)(app_dev_t *driver);
    void     (*curve_charge_table)(app_dev_t *driver, void **table, uint32_t *item);
    void     (*curve_discharge_table)(app_dev_t *driver, void **table, uint32_t *item);
} app_dev_battery_api_t;

/* 设备battery抽象操作数据 */
typedef struct {
#define APP_DEV_BATTERY_CURVE_DATA          \
        struct {                            \
            uint32_t voltage;               \
            uint8_t  percent;               \
        }
    const void    *curve_charge;
    const uint32_t curve_charge_item;
    const void    *curve_discharge;
    const uint32_t curve_discharge_item;
    /* 充电事件通报回调 */
    void (*charge_irq_cb)(void);
} app_dev_battery_data_t;

/*@brief battery设备初始化
 *@param driver 设备实例
 *@retval 当前温度值
 */
static inline void app_dev_battery_ready(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        api->ready(driver);
    }
}

/*@brief battery设备充电满检查
 *@param driver 设备实例
 *@retval 0:未充满; 1:已充满; -1:不支持;
 */
static inline uint8_t app_dev_battery_charge_full(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        return api->charge_full(driver);
    }
    return -1;
}

/*@brief battery设备充电状态检查
 *@param driver 设备实例
 *@retval 0:未充电; 1:充电中;
 */
static inline uint8_t app_dev_battery_charge_status(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        return api->charge_status(driver);
    }
    return 0;
}

/*@brief battery设备充电事件通报回调注册
 *@param driver 设备实例
 *@param cb     充电事件通报回调
 */
static inline void app_dev_battery_charge_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        api->charge_irq_cb_reg(driver, cb);
    }
}

/*@brief battery设备事件通报开关
 *@param driver 设备实例
 *@param flag   true:开启事件通报;false:关闭事件通报
 */
static inline void app_dev_battery_charge_irq_switch(app_dev_t *driver, bool flag)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        api->charge_irq_switch(driver, flag);
    }
}

/*@brief battery设备电池电压(mV)
 *@param driver 设备实例
 *@retval 电池电压
 */
static inline uint32_t app_dev_battery_voltage_measure(app_dev_t *driver)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        return api->voltage_measure(driver);
    }
    return 0;
}

/*@brief battery设备充电曲线表
 *@param driver 设备实例
 *@param table  曲线表(APP_DEV_BATTERY_CURVE_DATA类型)
 *@param item   元素个数
 */
static inline void app_dev_battery_curve_charge_table(app_dev_t *driver, void **table, uint32_t *item)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        api->curve_charge_table(driver, table, item);
    }
}

/*@brief battery设备放电曲线表
 *@param driver 设备实例
 *@param table  曲线表(APP_DEV_BATTERY_CURVE_DATA类型)
 *@param item   元素个数
 */
static inline void app_dev_battery_curve_discharge_table(app_dev_t *driver, void **table, uint32_t *item)
{
    if (driver != NULL && driver->api != NULL) {
        const app_dev_battery_api_t *api = driver->api;
        api->curve_discharge_table(driver, table, item);
    }
}

#endif
