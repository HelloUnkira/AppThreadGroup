/*实现目标:
 *    喝水提醒相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_module_clock.h"
#include "app_thread_group.h"
#include "app_module_data_center.h"
#include "app_module_remind_drink.h"

static app_mutex_t app_module_remind_drink_mutex = {0};
static app_module_remind_drink_t app_module_remind_drink = {0};

/*@brief     喝水提醒设置
 *@param[in] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_set(app_module_remind_drink_t *remind_drink)
{
    app_mutex_process(&app_module_remind_drink_mutex, app_mutex_take);
    app_module_remind_drink = *remind_drink;
    app_mutex_process(&app_module_remind_drink_mutex, app_mutex_give);
}

/*@brief     喝水提醒获取
 *@param[in] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_get(app_module_remind_drink_t *remind_drink)
{
    app_mutex_process(&app_module_remind_drink_mutex, app_mutex_take);
    *remind_drink = app_module_remind_drink;
    app_mutex_process(&app_module_remind_drink_mutex, app_mutex_give);
}

/*@brief 喝水提醒默认设置
 */
void app_module_remind_drink_reset(void)
{
    /* 9:00~11:30;13:00~18:00;1mins,workday */
    app_module_remind_drink_t remind_drink = {0};
    remind_drink.am_time_s[0] = 9;
    remind_drink.am_time_s[1] = 0;
    remind_drink.am_time_e[0] = 11;
    remind_drink.am_time_e[1] = 30;
    remind_drink.pm_time_s[0] = 13;
    remind_drink.pm_time_s[1] = 0;
    remind_drink.pm_time_e[0] = 18;
    remind_drink.pm_time_e[1] = 30;
    remind_drink.am_valid = true;
    remind_drink.pm_valid = true;
    remind_drink.interval = 15;
    remind_drink.week = 0b0111110;
    app_module_remind_drink_set(&remind_drink);
}

/*@brief 喝水提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_drink_xmin_update(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    APP_SYS_LOG_INFO("");
    app_module_remind_drink_t remind_drink = {0};
    app_module_remind_drink_get(&remind_drink);
    /* 今天需要提醒 */
    if ((remind_drink.week & (1 << clock.week)) != 0) {
        /* 上午需要提醒 */
        if (remind_drink.am_valid) {
            uint16_t c_mins   = clock.hour * 60 + clock.minute;
            uint16_t t_mins_s = remind_drink.am_time_s[0] * 60 + remind_drink.am_time_s[1];
            uint16_t t_mins_e = remind_drink.am_time_e[0] * 60 + remind_drink.am_time_e[1];
            /* 落在提醒范围内 */
            if (t_mins_s <= c_mins && c_mins <= t_mins_e) {
                /* 到达提醒间隔点 */
                if (((c_mins - t_mins_s) % remind_drink.interval) == 0) {
                    /* 发送提醒事件 */
                    app_thread_package_t package = {
                        .thread = app_thread_id_mix_custom,
                        .module = app_thread_mix_custom_remind_misc,
                        .event  = app_thread_mix_custom_remind_drink_interval,
                    };
                    app_thread_package_notify(&package);
                }
            }
        }
        /* 下午需要提醒 */
        if (remind_drink.pm_valid) {
            uint16_t c_mins   = clock.hour * 60 + clock.minute;
            uint16_t t_mins_s = remind_drink.pm_time_s[0] * 60 + remind_drink.pm_time_s[1];
            uint16_t t_mins_e = remind_drink.pm_time_e[0] * 60 + remind_drink.pm_time_e[1];
            /* 落在提醒范围内 */
            if (t_mins_s <= c_mins && c_mins <= t_mins_e) {
                /* 到达提醒间隔点 */
                if (((c_mins - t_mins_s) % remind_drink.interval) == 0) {
                    /* 发送提醒事件 */
                    app_thread_package_t package = {
                        .thread = app_thread_id_mix_custom,
                        .module = app_thread_mix_custom_remind_misc,
                        .event  = app_thread_mix_custom_remind_drink_interval,
                    };
                    app_thread_package_notify(&package);
                }
            }
        }
    }
    app_module_remind_drink_set(&remind_drink);
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

/*@brief 喝水提醒转储到外存
 */
void app_module_remind_drink_dump(void)
{
    app_module_remind_drink_t remind_drink = {0};
    app_module_remind_drink_get(&remind_drink);
    
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = NULL;
    app_module_data_center_load(app_module_data_center_user_profile);
    app_module_data_center_source(&data_center);
    memcpy(&data_center->user_profile.remind_drink, &remind_drink, sizeof(app_module_remind_drink_t));
    app_module_data_center_dump();
}

/*@brief 喝水提醒加载到内存
 */
void app_module_remind_drink_load(void)
{
    app_module_remind_drink_t remind_drink = {0};
    
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = NULL;
    app_module_data_center_load(app_module_data_center_user_profile);
    bool retval = app_module_data_center_source(&data_center);
    memcpy(&remind_drink, &data_center->user_profile.remind_drink, sizeof(app_module_remind_drink_t));
    app_module_data_center_dump();
    
    if(retval)
        app_module_remind_drink_set(&remind_drink);
    else {
        app_module_remind_drink_reset();
        APP_SYS_LOG_WARN("load drink remind fail");
    }
}

/*@brief 喝水提醒模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_drink_ready(void)
{
    app_mutex_process(&app_module_remind_drink_mutex, app_mutex_static);
}
