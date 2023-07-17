/*实现目标:
 *    电池管理相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_thread_group.h"
#include "app_module_battery.h"

static app_mutex_t app_module_battery_mutex = {0};
static app_sys_timer_t app_module_battery_charge_xms_timer  = {0};
static app_sys_timer_t app_module_battery_voltage_xms_timer = {0};
static app_module_battery_t app_module_battery = {0};

/*@brief 电池模组电压及电量百分比更新算法
 */
static void app_module_battery_voltage_algorithm(app_module_battery_t *battery)
{
    //电池电量百分比转换及过滤算法写在此处
    //使用battery->voltage_sample以及battery->percent_sample
    //生成battery->voltage_filter以及battery->percent_filter
}

/*@brief  电池模组充电状态检查
 *@retval 0:未充电; 1:充电中;
 */
uint8_t app_module_battery_charge_status(void)
{
    app_mutex_process(&app_module_battery_mutex, app_mutex_take);
    uint8_t charge_status = app_arch_battery_charge_status(&app_arch_battery);
    app_mutex_process(&app_module_battery_mutex, app_mutex_give);
    return charge_status;
}

/*@brief  电池模组充电满检查
 *@retval 0:未充满; 1:已充满; -1:不支持;
 */
uint8_t app_module_battery_charge_full(void)
{
    app_mutex_process(&app_module_battery_mutex, app_mutex_take);
    uint8_t charge_full = app_arch_battery_charge_full(&app_arch_battery);
    app_mutex_process(&app_module_battery_mutex, app_mutex_give);
    return charge_full;
}

/*@brief  电池模组电压获取
 *@retval 电压(mV)
 */
uint32_t app_module_battery_voltage(void)
{
    app_mutex_process(&app_module_battery_mutex, app_mutex_take);
    uint8_t voltage = app_module_battery.voltage_filter;
    app_mutex_process(&app_module_battery_mutex, app_mutex_give);
    return voltage;
}

/*@brief  电池模组电量获取
 *@retval 电量百分比[0, 100]
 */
uint32_t app_module_battery_percent(void)
{
    app_mutex_process(&app_module_battery_mutex, app_mutex_take);
    uint8_t percent = app_module_battery.percent_filter;
    app_mutex_process(&app_module_battery_mutex, app_mutex_give);
    return percent;
}

/*@brief 电池模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_charge_check(void)
{
    app_sys_timer_start(&app_module_battery_charge_xms_timer);
}

/*@brief 电池模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_voltage_check(void)
{
    app_sys_timer_start(&app_module_battery_voltage_xms_timer);
}

/*@brief 电池模组事件处理
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_charge_xms_update(void)
{
    uint8_t charge_status = app_module_battery_charge_status();
    uint8_t charge_full   = app_module_battery_charge_full();
    app_mutex_process(&app_module_battery_mutex, app_mutex_take);
    app_module_battery_t *battery = &app_module_battery;
    /* 充放电状态未切换 */
    if (battery->charge_status == charge_status) {
        /* 当前状态是充电状态 */
        if (charge_status == 1) {
            /* 检测到充电,持续不断发送充电事件 */
            if (battery->charge_status == 1) {
                app_thread_package_t package = {
                    .thread = app_thread_id_mix_irq,
                    .module = app_thread_mix_irq_battery,
                    .event  = app_thread_mix_irq_battery_charge,
                };
                app_thread_package_notify(&package);
            }
        }
        /* 当前状态是放电状态 */
        if (charge_status == 0) {
            /* 检测到未充电,持续不断发送放电事件 */
            if (battery->charge_status == 1) {
                app_thread_package_t package = {
                    .thread = app_thread_id_mix_irq,
                    .module = app_thread_mix_irq_battery,
                    .event  = app_thread_mix_irq_battery_discharge,
                };
                app_thread_package_notify(&package);
            }
        }
    }
    /* 充放电状态切换 */
    if (battery->charge_status != charge_status) {
        /* 当前状态是充电状态 */
        if (charge_status == 1) {
            /* 更新充放电数据 */
            battery->charge_status = charge_status;
            battery->charge_count  = 0;
            battery->charge_full   = charge_full;
        }
        /* 当前状态是放电状态 */
        if (charge_status == 0) {
            /* 软件消抖 */
            if (battery->charge_count++ > 2) {
                /* 更新充放电数据 */
                battery->charge_status = charge_status;
                battery->charge_count  = 0;
                battery->charge_full   = charge_full;
                /* 更新充放电定时器检测周期 */
                app_sys_timer_stop(&app_module_battery_charge_xms_timer);
            }
        }
    }
    app_mutex_process(&app_module_battery_mutex, app_mutex_give);
}

/*@brief 电池模组电压及电量百分比更新
 *       内部使用: 被mix irq线程使用
 */
void app_module_battery_voltage_xms_update(void)
{
    uint8_t charge_status = app_module_battery_charge_status();
    uint8_t charge_full   = app_module_battery_charge_full();
    app_mutex_process(&app_module_battery_mutex, app_mutex_take);
    app_module_battery_t *battery = &app_module_battery;
    /* 获得采样值 */
    uint32_t voltage_sample = app_arch_battery_voltage_measure(&app_arch_battery);
    battery->voltage_sample = voltage_sample;
    /* 读取充放电曲线表 */
    APP_ARCH_BATTERY_CURVE_DATA *table = NULL; uint32_t table_item = 0;
    if (battery->charge_status == 1)
        app_arch_battery_curve_charge_table(&app_arch_battery, &table, &table_item);
    if (battery->charge_status == 0)
        app_arch_battery_curve_discharge_table(&app_arch_battery, &table, &table_item);
    APP_SYS_ASSERT(table[0].voltage > 0);
    /* 充放电曲线表找到区间计算百分比 */
    for (uint32_t idx = 1; table[idx].voltage > 0; idx++)
        if (table[idx].voltage >= voltage_sample) {
            uint32_t percent = 0;
            uint32_t p0  = table[idx - 1].percent;
            uint32_t v0  = table[idx - 1].voltage;
            uint32_t p1  = table[idx - 0].percent;
            uint32_t v1  = table[idx - 0].voltage;
            uint32_t p10 = p1 - p0;
            uint32_t v10 = v1 - v0;
            uint32_t vx0 = voltage_sample - v0;
            percent = (vx0 * p10 + v10 / 2) / v10  + p0;
            percent = (percent < 0) ? 0 :(percent > 100) ? 100 : percent;
            battery->percent_sample = percent;
            break;
        }
    battery->voltage_filter = battery->voltage_sample;
    battery->percent_filter = battery->percent_sample;
    app_module_battery_voltage_algorithm(battery);
    app_mutex_process(&app_module_battery_mutex, app_mutex_give);
    /* 检测次数到时关闭 */
    if (battery->voltage_count++ > 10) {
        battery->voltage_count = 0;
        app_sys_timer_stop(&app_module_battery_voltage_xms_timer);
    }
}

/*@brief 电池模组软件定时器回调
 */
static void app_module_battery_charge_xms_timer_handler(void *timer)
{
    /* 发送电池模组更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_battery,
        .event  = app_thread_mix_irq_battery_charge_xms_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 电池模组软件定时器回调
 */
static void app_module_battery_voltage_xms_timer_handler(void *timer)
{
    /* 发送电池模组更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_battery,
        .event  = app_thread_mix_irq_battery_voltage_xms_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 电池模组充电事件中断回调
 */
static void app_module_battery_charge_irq_cb(void)
{
    /* 发送电池模组更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_battery,
        .event  = app_thread_mix_irq_battery_charge_check,
    };
    app_thread_package_notify(&package);
}

/*@brief 电池模组初始化
 */
void app_module_battery_ready(void)
{
    app_mutex_process(&app_module_battery_mutex, app_mutex_static);
    app_arch_battery_charge_irq_cb_reg(&app_arch_battery, app_module_battery_charge_irq_cb);
    app_module_battery_charge_xms_timer.expired  = app_module_battery_charge_xms_timer_handler;
    app_module_battery_charge_xms_timer.peroid   = APP_MODULE_BATTERY_CHARGE_PEROID;
    app_module_battery_charge_xms_timer.reload   = true;
    app_module_battery_voltage_xms_timer.expired = app_module_battery_voltage_xms_timer_handler;
    app_module_battery_voltage_xms_timer.peroid  = APP_MODULE_BATTERY_VOLTAGE_PEROID;
    app_module_battery_voltage_xms_timer.reload  = true;
}
