/*实现目标:
 *    走动提醒相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 时间表检查函数:app_module_daily_time_remind_sedentary_f */
APP_MODULE_DAILY_TIME_F(app_module_daily_time_remind_sedentary_t, app_module_daily_time_remind_sedentary_f, 2);

/*@brief 走动提醒设置
 *@param remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_set(app_module_remind_sedentary_t *remind_sedentary)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.remind_sedentary, remind_sedentary, sizeof(app_module_remind_sedentary_t));
    app_module_data_center_give();
}

/*@brief 走动提醒获取
 *@param remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_get(app_module_remind_sedentary_t *remind_sedentary)
{
    /* 提取数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(remind_sedentary, &data_center_src->module_source.remind_sedentary, sizeof(app_module_remind_sedentary_t));
    app_module_data_center_give();
}

/*@brief 走动提醒默认设置
 *@param remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_reset(app_module_remind_sedentary_t *remind_sedentary)
{
    /* 9:00~11:30;13:00~18:00;15mins,workday */
    remind_sedentary->daily_time.table[0].time_s[0] = 9;
    remind_sedentary->daily_time.table[0].time_s[1] = 0;
    remind_sedentary->daily_time.table[0].time_s[2] = 0;
    remind_sedentary->daily_time.table[0].time_e[0] = 11;
    remind_sedentary->daily_time.table[0].time_e[1] = 30;
    remind_sedentary->daily_time.table[0].time_e[2] = 0;
    remind_sedentary->daily_time.table[1].time_s[0] = 13;
    remind_sedentary->daily_time.table[1].time_s[1] = 0;
    remind_sedentary->daily_time.table[1].time_s[2] = 0;
    remind_sedentary->daily_time.table[1].time_e[0] = 18;
    remind_sedentary->daily_time.table[1].time_e[1] = 0;
    remind_sedentary->daily_time.table[1].time_e[2] = 0;
    remind_sedentary->daily_time.week = 0b0111110;
    remind_sedentary->daily_time.bits = 0b11;
    remind_sedentary->interval = 1 * 60;
    remind_sedentary->utc = 0;
}

/*@brief 走到提醒时长刷新
 *       如果产生了活动或走动,应该刷新它
 */
void app_module_remind_sedentary_reflush(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    app_module_remind_sedentary_t remind_sedentary = {0};
    app_module_remind_sedentary_get(&remind_sedentary);
    remind_sedentary.utc = clock.utc;
    app_module_remind_sedentary_set(&remind_sedentary);
}

/*@brief 走动提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_sedentary_xmin_update(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    app_module_remind_sedentary_t remind_sedentary = {0};
    app_module_remind_sedentary_get(&remind_sedentary);
    
    /* 时间片靠前,先校正 */
    if (remind_sedentary.utc > clock.utc) {
        remind_sedentary.utc = clock.utc;
        return;
    }
    /* 离上次提醒时间未超过间隔,此次提醒忽略 */
    if (remind_sedentary.utc + remind_sedentary.interval > clock.utc)
        return;
    
    APP_SYS_LOG_INFO("");
    
    if (app_module_daily_time_remind_sedentary_f(&remind_sedentary.daily_time)) {
        remind_sedentary.utc = clock.utc;
        app_module_remind_sedentary_set(&remind_sedentary);
        /* 发送提醒事件 */
        app_thread_package_t package = {
            .thread = app_thread_id_mix_custom,
            .module = app_thread_mix_custom_remind_misc,
            .event  = app_thread_mix_custom_remind_sedentary_interval,
        };
        app_thread_package_notify(&package);
    }
}

/*@brief 系统状态控制更新
 *@param clock 时钟实例
 */
void app_module_remind_sedentary_update(app_module_clock_t clock[1])
{
    app_thread_package_t package = {
        .thread = app_thread_id_mix_custom,
        .module = app_thread_mix_custom_remind_misc,
        .event  = app_thread_mix_custom_remind_sedentary_update,
    };
    app_thread_package_notify(&package);
}
