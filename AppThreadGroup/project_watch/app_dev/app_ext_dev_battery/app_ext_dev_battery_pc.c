/* 实现目标:
 *     模拟电池管理
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_ARCH_IS_PC

/* 设备battery抽象操作参数 */
typedef struct {
    void *args;
} app_dev_battery_cfg_t;

/*@brief battery设备初始化
 *@param driver 设备实例
 *@retval 当前温度值
 */
static void app_dev_battery_hal_ready(app_dev_t *driver)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
}

/*@brief battery设备充电满检查
 *@param driver 设备实例
 *@retval 0:未充满; 1:已充满; -1:不支持;
 */
static uint8_t app_dev_battery_hal_charge_full(app_dev_t *driver)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    return -1;
}

/*@brief battery设备充电状态检查
 *@param driver 设备实例
 *@retval 0:未充电; 1:充电中;
 */
static uint8_t app_dev_battery_hal_charge_status(app_dev_t *driver)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    return 0;
}

/*@brief battery设备充电事件通报回调注册
 *@param driver 设备实例
 *@param cb     充电事件通报回调
 */
static void app_dev_battery_hal_charge_irq_cb_reg(app_dev_t *driver, void(*cb)(void))
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    data->charge_irq_cb = cb;
}

/*@brief battery设备事件通报开关
 *@param driver 设备实例
 *@param flag   true:开启事件通报;false:关闭事件通报
 */
static void app_dev_battery_hal_charge_irq_switch(app_dev_t *driver, bool flag)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    APP_SYS_LOG_WARN("irq_switch:%d", flag);
}

/*@brief battery设备电池电压(mV)
 *@param driver 设备实例
 *@retval 电池电压
 */
static uint32_t app_dev_battery_hal_voltage_measure(app_dev_t *driver)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    return 3850;
}

/*@brief battery设备充电曲线表
 *@param driver 设备实例
 *@param table  曲线表
 *@param item   元素个数
 */
static void app_dev_battery_hal_curve_charge_table(app_dev_t *driver, void **table, uint32_t *item)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    *table = data->curve_charge;
    *item  = data->curve_charge_item;
}

/*@brief battery设备放电曲线表
 *@param driver 设备实例
 *@param table  曲线表
 *@param item   元素个数
 */
static void app_dev_battery_hal_curve_discharge_table(app_dev_t *driver, void **table, uint32_t *item)
{
    const app_dev_battery_cfg_t *cfg = driver->cfg;
    app_dev_battery_data_t *data = driver->data;
    /* 填充目标平台下的动作 */
    *table = data->curve_discharge;
    *item  = data->curve_discharge_item;
}

/* 充电曲线表 */
static const APP_DEV_BATTERY_CURVE_DATA app_dev_battery_hal_curve_charge[] = {
    {3600,   0},
    {4200, 100},
    {0, 0},
};

/* 放电曲线表 */
static const APP_DEV_BATTERY_CURVE_DATA app_dev_battery_hal_curve_discharge[] = {
    {3600,   0},
    {4200, 100},
    {0, 0},
};

/* 静态配置的设备操作参数 */
static const app_dev_battery_cfg_t app_dev_battery_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_battery_api_t app_dev_battery_api = {
    .ready                  = app_dev_battery_hal_ready,
    .charge_full            = app_dev_battery_hal_charge_full,
    .charge_status          = app_dev_battery_hal_charge_status,
    .charge_irq_cb_reg      = app_dev_battery_hal_charge_irq_cb_reg,
    .charge_irq_switch      = app_dev_battery_hal_charge_irq_switch,
    .voltage_measure        = app_dev_battery_hal_voltage_measure,
    .curve_charge_table     = app_dev_battery_hal_curve_charge_table,
    .curve_discharge_table  = app_dev_battery_hal_curve_discharge_table,
};

/* 动态的设备操作数据 */
static app_dev_battery_data_t app_dev_battery_data = {
    .curve_charge           = app_dev_battery_hal_curve_charge,
    .curve_discharge        = app_dev_battery_hal_curve_discharge,
    .curve_charge_item      = sizeof(app_dev_battery_hal_curve_charge)    / sizeof(app_dev_battery_hal_curve_charge[0]),
    .curve_discharge_item   = sizeof(app_dev_battery_hal_curve_discharge) / sizeof(app_dev_battery_hal_curve_discharge[0]),
    
    .charge_irq_cb          = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_battery = {
    .name = "app_dev_battery",
    .cfg  = &app_dev_battery_cfg,
    .api  = &app_dev_battery_api,
    .data = &app_dev_battery_data,
};

#endif
