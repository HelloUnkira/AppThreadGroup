#ifndef APP_MODULE_BATTERY_H
#define APP_MODULE_BATTERY_H

/* 电池电量临界配置 */
#define APP_MODULE_BATTERY_PERCENT_LOW_POWERR   (10)    //低电报警电量
#define APP_MODULE_BATTERY_PERCENT_SHUTDOWN     (3)     //自动关机电量
#define APP_MODULE_BATTERY_PERCENT_POWEN_ON     (8)     //开机最小电量

/* 充电检测周期 */
#define APP_MODULE_BATTERY_CHARGE_PEROID            (500)
/* 电压更新周期 */
#define APP_MODULE_BATTERY_VOLTAGE_PEROID           (100)

typedef struct {
    /* 电池充放电状态检查 */
    uint8_t  charge_status;
    uint8_t  charge_count;
    uint8_t  charge_full;
    /* 电池当前电压和电量(采样值,过滤值) */
    uint32_t voltage_count;
    uint32_t voltage_sample;
    uint8_t  percent_sample;
    uint32_t voltage_filter;
    uint32_t percent_filter;
    /* 电池电压电量算法所需数据元素: */
    /* ...... */
} app_module_battery_t;

/*@brief     电池电量百分比转等级
 *@param[in] percent 电量百分比
 *@param[in] level   电量等级
 */
uint8_t app_module_battery_level(uint8_t percent, uint8_t level);

/*@brief  电池模组充电状态检查
 *@retval 0:未充电; 1:充电中;
 */
uint8_t app_module_battery_charge_status(void);

/*@brief  电池模组充电满检查
 *@retval 0:未充满; 1:已充满; -1:不支持;
 */
uint8_t app_module_battery_charge_full(void);

/*@brief  电池模组电压获取
 *@retval 电压(mV)
 */
uint32_t app_module_battery_voltage(void);

/*@brief  电池模组电量获取
 *@retval 电量百分比[0, 100]
 */
uint32_t app_module_battery_percent(void);

/*@brief 电池模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_charge_check(void);

/*@brief 电池模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_voltage_check(void);

/*@brief 电池模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_charge_xms_update(void);

/*@brief 电池模组电压及电量百分比更新
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_voltage_xms_update(void);

/*@brief 电池模组初始化
 */
void app_module_battery_ready(void);

#endif
