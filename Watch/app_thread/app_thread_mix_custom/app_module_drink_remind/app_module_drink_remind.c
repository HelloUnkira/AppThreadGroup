/*实现目标:
 *    喝水提醒相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_ext_src.h"
#include "app_module_clock.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_drink_remind.h"

static app_mutex_t app_module_drink_remind_mutex = {0};
static app_module_drink_remind_t app_module_drink_remind = {0};

/*@brief 喝水提醒默认设置
 */
void app_module_drink_remind_reset(void)
{
    /* 9:00~11:30;13:00~18:00;1mins,workday */
    app_mutex_take(&app_module_drink_remind_mutex);
    app_module_drink_remind.am_time_s[0] = 9;
    app_module_drink_remind.am_time_s[1] = 0;
    app_module_drink_remind.am_time_e[0] = 11;
    app_module_drink_remind.am_time_e[1] = 30;
    app_module_drink_remind.pm_time_s[0] = 13;
    app_module_drink_remind.pm_time_s[1] = 0;
    app_module_drink_remind.pm_time_e[0] = 18;
    app_module_drink_remind.pm_time_e[1] = 30;
    app_module_drink_remind.am_valid = true;
    app_module_drink_remind.pm_valid = true;
    app_module_drink_remind.interval = 15;
    app_module_drink_remind.week = 0b0111110;
    app_mutex_give(&app_module_drink_remind_mutex);
}

/*@brief     喝水提醒设置
 *@param[in] drink_remind 喝水提醒参数
 */
void app_module_drink_remind_set(app_module_drink_remind_t *drink_remind)
{
    app_mutex_take(&app_module_drink_remind_mutex);
    app_module_drink_remind = *drink_remind;
    app_mutex_give(&app_module_drink_remind_mutex);
}

/*@brief     喝水提醒获取
 *@param[in] drink_remind 喝水提醒参数
 */
void app_module_drink_remind_get(app_module_drink_remind_t *drink_remind)
{
    app_mutex_take(&app_module_drink_remind_mutex);
    *drink_remind = app_module_drink_remind;
    app_mutex_give(&app_module_drink_remind_mutex);
}

/*@brief 喝水提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_drink_remind_xmin_update(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    APP_SYS_LOG_WARN("");
    app_mutex_take(&app_module_drink_remind_mutex);
    /* 今天需要提醒 */
    if ((app_module_drink_remind.week & (1 << clock.week)) != 0) {
        /* 上午需要提醒 */
        if (app_module_drink_remind.am_valid) {
            uint16_t c_mins   = clock.hour * 60 + clock.minute;
            uint16_t t_mins_s = app_module_drink_remind.am_time_s[0] * 60 +
                                app_module_drink_remind.am_time_s[1];
            uint16_t t_mins_e = app_module_drink_remind.am_time_e[0] * 60 +
                                app_module_drink_remind.am_time_e[1];
            /* 落在提醒范围内 */
            if (t_mins_s <= c_mins && c_mins <= t_mins_e) {
                /* 到达提醒间隔点 */
                if (((c_mins - t_mins_s) % app_module_drink_remind.interval) == 0) {
                    /* 发送提醒事件 */
                    app_package_t package = {
                        .thread = app_thread_id_mix_custom,
                        .module = app_thread_mix_custom_drink_remind,
                        .event  = app_thread_mix_custom_drink_remind_interval,
                    };
                    app_package_notify(&package);
                }
            }
        }
        /* 下午需要提醒 */
        if (app_module_drink_remind.pm_valid) {
            uint16_t c_mins   = clock.hour * 60 + clock.minute;
            uint16_t t_mins_s = app_module_drink_remind.pm_time_s[0] * 60 +
                                app_module_drink_remind.pm_time_s[1];
            uint16_t t_mins_e = app_module_drink_remind.pm_time_e[0] * 60 +
                                app_module_drink_remind.pm_time_e[1];
            /* 落在提醒范围内 */
            if (t_mins_s <= c_mins && c_mins <= t_mins_e) {
                /* 到达提醒间隔点 */
                if (((c_mins - t_mins_s) % app_module_drink_remind.interval) == 0) {
                    /* 发送提醒事件 */
                    app_package_t package = {
                        .thread = app_thread_id_mix_custom,
                        .module = app_thread_mix_custom_drink_remind,
                        .event  = app_thread_mix_custom_drink_remind_interval,
                    };
                    app_package_notify(&package);
                }
            }
        }
    }
    app_mutex_give(&app_module_drink_remind_mutex);
}

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_drink_remind_update(app_module_clock_t clock[1])
{
    app_package_t package = {
        .thread = app_thread_id_mix_custom,
        .module = app_thread_mix_custom_drink_remind,
        .event  = app_thread_mix_custom_drink_remind_update,
    };
    app_package_notify(&package);
}

/*@brief 喝水提醒转储到外存
 */
void app_module_drink_remind_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_drink_remind_t remind;
            uint32_t crc32;
        } data;
    } remind_data = {.data.remind = app_module_drink_remind};
    
    remind_data.data.crc32 = app_sys_crc32(remind_data.buffer, sizeof(app_module_drink_remind_t));
    app_sys_ext_src_write("mix_chunk_small", "drink remind", remind_data.buffer, sizeof(remind_data));
}

/*@brief 喝水提醒加载到内存
 */
void app_module_drink_remind_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_drink_remind_t remind;
            uint32_t crc32;
        } data;
    } remind_data = {};
    
    app_sys_ext_src_read("mix_chunk_small", "drink remind", remind_data.buffer, sizeof(remind_data));
    uint32_t crc32 = app_sys_crc32(remind_data.buffer, sizeof(app_module_drink_remind_t));
    if (crc32 == remind_data.data.crc32) {
        app_mutex_take(&app_module_drink_remind_mutex);
        app_module_drink_remind = remind_data.data.remind;
        app_mutex_give(&app_module_drink_remind_mutex);
    }
    if (crc32 != remind_data.data.crc32) {
        app_module_drink_remind_reset();
        APP_SYS_LOG_WARN("load drink remind fail");
    }
}

/*@brief 喝水提醒模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_drink_remind_ready(void)
{
    app_mutex_process(&app_module_drink_remind_mutex);
}
