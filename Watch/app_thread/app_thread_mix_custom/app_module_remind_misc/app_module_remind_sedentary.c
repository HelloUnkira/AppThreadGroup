/*实现目标:
 *    走动提醒相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_ext_src.h"
#include "app_module_clock.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_remind_sedentary.h"

static app_mutex_t app_module_remind_sedentary_mutex = {0};
static app_module_remind_sedentary_t app_module_remind_sedentary = {0};

/*@brief     走动提醒设置
 *@param[in] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_set(app_module_remind_sedentary_t *remind_sedentary)
{
    /* reflush_utc是内部特殊字段,不可设置 */
    app_mutex_take(&app_module_remind_sedentary_mutex);
    app_module_remind_sedentary = *remind_sedentary;
    app_mutex_give(&app_module_remind_sedentary_mutex);
}

/*@brief     走动提醒获取
 *@param[in] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_get(app_module_remind_sedentary_t *remind_sedentary)
{
    app_mutex_take(&app_module_remind_sedentary_mutex);
    *remind_sedentary = app_module_remind_sedentary;
    app_mutex_give(&app_module_remind_sedentary_mutex);
}

/*@brief 走动提醒默认设置
 */
void app_module_remind_sedentary_reset(void)
{
    /* 9:00~11:30;13:00~18:00;1mins,workday */
    app_module_remind_sedentary_t remind_sedentary = {0};
    remind_sedentary.reflush      = 0;
    remind_sedentary.am_time_s[0] = 9;
    remind_sedentary.am_time_s[1] = 0;
    remind_sedentary.am_time_e[0] = 11;
    remind_sedentary.am_time_e[1] = 30;
    remind_sedentary.pm_time_s[0] = 13;
    remind_sedentary.pm_time_s[1] = 0;
    remind_sedentary.pm_time_e[0] = 18;
    remind_sedentary.pm_time_e[1] = 30;
    remind_sedentary.am_valid = true;
    remind_sedentary.pm_valid = true;
    remind_sedentary.interval = 15;
    remind_sedentary.week = 0b0111110;
    app_module_remind_sedentary_set(&remind_sedentary);
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
    remind_sedentary.reflush = clock.hour * 60 + clock.minute;
    app_module_remind_sedentary_set(&remind_sedentary);
}

/*@brief 走动提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_sedentary_xmin_update(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    APP_SYS_LOG_INFO("");
    app_module_remind_sedentary_t remind_sedentary = {0};
    app_module_remind_sedentary_get(&remind_sedentary);
    /* 今天需要提醒 */
    if ((remind_sedentary.week & (1 << clock.week)) != 0) {
        /* 上午需要提醒 */
        if (remind_sedentary.am_valid) {
            uint16_t c_mins   = clock.hour * 60 + clock.minute;
            uint16_t t_mins_s = remind_sedentary.am_time_s[0] * 60 + remind_sedentary.am_time_s[1];
            uint16_t t_mins_e = remind_sedentary.am_time_e[0] * 60 + remind_sedentary.am_time_e[1];
            /* 落在提醒范围内 */
            if (t_mins_s <= c_mins && c_mins <= t_mins_e) {
                /* 到达提醒间隔点 */
                if (c_mins - t_mins_s > remind_sedentary.interval &&
                   (remind_sedentary.reflush >= c_mins - remind_sedentary.interval ||
                    remind_sedentary.reflush == 0)) {
                    remind_sedentary.reflush  = c_mins;
                    /* 发送提醒事件 */
                    app_package_t package = {
                        .thread = app_thread_id_mix_custom,
                        .module = app_thread_mix_custom_remind_misc,
                        .event  = app_thread_mix_custom_remind_sedentary_interval,
                    };
                    app_package_notify(&package);
                }
            }
        }
        /* 下午需要提醒 */
        if (remind_sedentary.pm_valid) {
            uint16_t c_mins   = clock.hour * 60 + clock.minute;
            uint16_t t_mins_s = remind_sedentary.pm_time_s[0] * 60 + remind_sedentary.pm_time_s[1];
            uint16_t t_mins_e = remind_sedentary.pm_time_e[0] * 60 + remind_sedentary.pm_time_e[1];
            /* 落在提醒范围内 */
            if (t_mins_s <= c_mins && c_mins <= t_mins_e) {
                /* 到达提醒间隔点 */
                if (c_mins - t_mins_s > remind_sedentary.interval &&
                   (remind_sedentary.reflush >= c_mins - remind_sedentary.interval ||
                    remind_sedentary.reflush == 0)) {
                    remind_sedentary.reflush  = c_mins;
                    /* 发送提醒事件 */
                    app_package_t package = {
                        .thread = app_thread_id_mix_custom,
                        .module = app_thread_mix_custom_remind_misc,
                        .event  = app_thread_mix_custom_remind_sedentary_interval,
                    };
                    app_package_notify(&package);
                }
            }
        }
    }
    app_module_remind_sedentary_set(&remind_sedentary);
}

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_remind_sedentary_update(app_module_clock_t clock[1])
{
    app_package_t package = {
        .thread = app_thread_id_mix_custom,
        .module = app_thread_mix_custom_remind_misc,
        .event  = app_thread_mix_custom_remind_sedentary_update,
    };
    app_package_notify(&package);
}

/*@brief 走动提醒转储到外存
 */
void app_module_remind_sedentary_dump(void)
{
    app_module_remind_sedentary_t remind_sedentary = {0};
    app_module_remind_sedentary_get(&remind_sedentary);
    union {
        uint8_t buffer[0];
        struct {
            app_module_remind_sedentary_t remind;
            uint32_t crc32;
        } data;
    } remind_data = {.data.remind = remind_sedentary};
    
    remind_data.data.crc32 = app_sys_crc32(remind_data.buffer, sizeof(app_module_remind_sedentary_t));
    app_sys_ext_src_write("mix_chunk_small", "remind sedentary", remind_data.buffer, sizeof(remind_data));
}

/*@brief 走动提醒加载到内存
 */
void app_module_remind_sedentary_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_remind_sedentary_t remind;
            uint32_t crc32;
        } data;
    } remind_data;
    
    app_sys_ext_src_read("mix_chunk_small", "remind sedentary", remind_data.buffer, sizeof(remind_data));
    uint32_t crc32 = app_sys_crc32(remind_data.buffer, sizeof(app_module_remind_sedentary_t));
    if (crc32 == remind_data.data.crc32) {
        app_module_remind_sedentary_t remind_sedentary = {0};
        remind_sedentary = remind_data.data.remind;
        app_module_remind_sedentary_set(&remind_sedentary);
    }
    if (crc32 != remind_data.data.crc32) {
        app_module_remind_sedentary_reset();
        APP_SYS_LOG_WARN("load sedentary remind fail");
    }
}

/*@brief 走动提醒模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_sedentary_ready(void)
{
    app_mutex_process(&app_module_remind_sedentary_mutex);
}
