/*实现目标:
 *    喝水提醒相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 时间表检查函数:app_module_daily_time_remind_drink_f */
APP_MODULE_DAILY_TIME_F(app_module_daily_time_remind_drink_t, app_module_daily_time_remind_drink_f, 2);

/*@brief     喝水提醒设置
 *@param[in] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_set(app_module_remind_drink_t *remind_drink)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.remind_drink, remind_drink, sizeof(app_module_remind_drink_t));
    app_module_data_center_give();
}

/*@brief      喝水提醒获取
 *@param[out] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_get(app_module_remind_drink_t *remind_drink)
{
    /* 提取数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(remind_drink, &data_center_src->module_source.remind_drink, sizeof(app_module_remind_drink_t));
    app_module_data_center_give();
}

/*@brief      喝水提醒默认设置
 *@param[out] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_reset(app_module_remind_drink_t *remind_drink)
{
    /* 9:00~11:30;13:00~18:00;1mins,workday */
    remind_drink->daily_time.table[0].time_s[0] = 9;
    remind_drink->daily_time.table[0].time_s[1] = 0;
    remind_drink->daily_time.table[0].time_s[2] = 0;
    remind_drink->daily_time.table[0].time_e[0] = 11;
    remind_drink->daily_time.table[0].time_e[1] = 30;
    remind_drink->daily_time.table[0].time_e[2] = 0;
    remind_drink->daily_time.table[1].time_s[0] = 13;
    remind_drink->daily_time.table[1].time_s[1] = 0;
    remind_drink->daily_time.table[1].time_s[2] = 0;
    remind_drink->daily_time.table[1].time_e[0] = 18;
    remind_drink->daily_time.table[1].time_e[1] = 0;
    remind_drink->daily_time.table[1].time_e[2] = 0;
    remind_drink->daily_time.week = 0b0111110;
    remind_drink->daily_time.bits = 0b11;
    remind_drink->interval = 1 * 60;
    remind_drink->utc = 0;
}

/*@brief 喝水提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_drink_xmin_update(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    app_module_remind_drink_t remind_drink = {0};
    app_module_remind_drink_get(&remind_drink);
    
    /* 时间片靠前,先校正 */
    if (remind_drink.utc > clock.utc) {
        remind_drink.utc = clock.utc;
        return;
    }
    /* 离上次提醒时间未超过间隔,此次提醒忽略 */
    if (remind_drink.utc + remind_drink.interval > clock.utc)
        return;
    
    APP_SYS_LOG_INFO("");
    
    if (app_module_daily_time_remind_drink_f(&remind_drink.daily_time)) {
        remind_drink.utc = clock.utc;
        app_module_remind_drink_set(&remind_drink);
        /* 发送提醒事件 */
        app_thread_package_t package = {
            .thread = app_thread_id_mix_custom,
            .module = app_thread_mix_custom_remind_misc,
            .event  = app_thread_mix_custom_remind_drink_interval,
        };
        app_thread_package_notify(&package);
    }
}

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_remind_drink_update(app_module_clock_t clock[1])
{
    app_thread_package_t package = {
        .thread = app_thread_id_mix_custom,
        .module = app_thread_mix_custom_remind_misc,
        .event  = app_thread_mix_custom_remind_drink_update,
    };
    app_thread_package_notify(&package);
}
