/*实现目标:
 *    一些和时钟运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_sys_clk.h"
#include "app_sys_crc.h"
#include "app_sys_ext_src.h"
#include "app_thread_group.h"
#include "app_module_rtc.h"
#include "app_module_clock.h"
#include "app_module_clock_cb.h"

/*@brief     闰年判断
 *@param[in] clock 时钟实例{.year,}
 *@retval    是否为闰年
 */
bool app_module_clock_is_leap_year(app_module_clock_t *clock)
{
    return app_sys_clk_is_leap_year((app_sys_clk_t *)clock);
}

/*@brief     下一闰年
 *@param[in] clock 时钟实例{.year,}
 *@param[in] leap_clock 下一闰年时钟实例{.year,}
 */
void app_module_clock_next_leap_year(app_module_clock_t *clock, app_module_clock_t *leap_clock)
{
    app_sys_clk_next_leap_year((app_sys_clk_t *)clock, (app_sys_clk_t *)leap_clock);
}

/*@brief     一个月有多少天
 *@param[in] clock 时钟实例{.year,.month,}
 *@retval    天数
 */
uint32_t app_module_clock_month_days(app_module_clock_t *clock)
{
    return app_sys_clk_month_days((app_sys_clk_t *)clock);
}

/*@brief     一年过了多少天
 *@param[in] clock 时钟实例{.year,.month,.day,}
 *@retval    天数
 */
uint32_t app_module_clock_year_days(app_module_clock_t *clock)
{
    return app_sys_clk_year_days((app_sys_clk_t *)clock);
}

/*@brief     俩个日期间的天数
 *@param[in] clock1 时钟实例{.year,.month,.day,}
 *@param[in] clock2 时钟实例{.year,.month,.day,}
 *@retval    天数
 */
uint32_t app_module_clock_how_many_days(app_module_clock_t *clock1, app_module_clock_t *clock2)
{
    return app_sys_clk_how_many_days((app_sys_clk_t *)clock1, (app_sys_clk_t *)clock2);
}

/*@brief      星期转化(蔡勒公式)
 *@param[in]  clock 时钟实例{.year,.month,.day,}
 *@param[out] clock 时钟实例{.week,}
 */
void app_module_clock_to_week(app_module_clock_t *clock)
{
    app_sys_clk_to_week((app_sys_clk_t *)clock);
}

/*@brief      日期转化为utc
 *@param[in]  clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 *@param[out] clock 时钟实例{.utc,}
 */
void app_module_clock_to_utc(app_module_clock_t *clock)
{
    app_sys_clk_to_utc((app_sys_clk_t *)clock);
}

/*@brief      utc转化为日期
 *@param[in]  clock 时钟实例{.utc,}
 *@param[out] clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 */
void app_module_clock_to_dtime(app_module_clock_t *clock)
{
    app_sys_clk_to_dtime((app_sys_clk_t *)clock);
}

/* 分界线:<上面是模组通用接口, 下面是模组功能定制接口> */
static uint64_t app_module_clock_sec_tick = {0};
static app_mutex_t app_module_clock_mutex = {0};
static app_sys_timer_t app_module_clock_timer = {0};
static app_module_clock_t app_module_clock[2] = {0};

/*@brief     获得系统开机时间(中断环境下不可调用)
 *@param[in] clock 时钟实例
 */
uint64_t app_module_clock_get_sec_tick(void)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    uint64_t retval = app_module_clock_sec_tick;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
    return retval;
}

/*@brief     设置时区偏移(中断环境下不可调用)
 *@param[in] zone_sec 时区偏移(秒)
 */
void app_module_clock_set_system_clock_zone(int32_t zone_sec)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    app_module_clock[0].zone_sec = zone_sec;
    app_module_clock[1].zone_sec = zone_sec;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
}

/*@brief     设置时区偏移(中断环境下不可调用)
 *@param[in] is_24 时区偏移(秒)
 */
void app_module_clock_set_system_clock_mode(bool is_24)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    app_module_clock[0].is_24 = is_24;
    app_module_clock[1].is_24 = is_24;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);

}

/*@brief     获得系统时间(中断环境下不可调用)
 *@param[in] clock 时钟实例
 */
void app_module_clock_get_system_clock(app_module_clock_t *clock)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    *clock = app_module_clock[1];
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
    /* 扩展补充 */
    if (clock->zone_sec != 0) {
        clock->utc += clock->zone_sec;
        app_module_clock_to_dtime(clock);
        app_module_clock_to_week(clock);
    }
    if (!clock->is_24) {
        if (clock->hour >= 12) {
            clock->is_am = false;
            clock->is_pm = true;
        } else {
            clock->is_am = true;
            clock->is_pm = false;
        }
        clock->hour = clock->hour == 0 ? clock->hour > 12 ? clock->hour - 12 : clock->hour : clock->hour;
    }
}

/*@brief     设置系统时间(中断环境下不可调用)
 *@param[in] clock 时钟实例
 */
void app_module_clock_set_system_clock(app_module_clock_t *clock)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    app_module_clock[0] = app_module_clock[1];
    app_module_clock[1] = *clock;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
    /* 向线程发送时钟更新事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_clock,
        .event  = app_thread_mix_irq_clock_local_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 系统时钟更新事件
 *       内部使用: 被mix irq线程使用
 */
void app_module_clock_local_update(void)
{
    /* 获得时钟更新 */
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    app_module_clock_t clock[2] = {0};
    clock[0] = app_module_clock[0];
    clock[1] = app_module_clock[1];
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
    /* 它只有一个调用者,所以无需保护 */
    static bool clock_is_sync = false;
    /* 检查是否第一次更新时钟 */
    uint32_t update_event = clock_is_sync ?
             app_module_clock_update_default :
             app_module_clock_update_first_set;
    if (!clock_is_sync)
         clock_is_sync = true;
    /* 执行更新事件 */
    for (uint32_t idx = 0; idx < app_module_clock_update_cb_size; idx++)
        app_module_clock_update_cb[idx](clock, update_event);
}

/*@brief     系统时间戳更新回调
 *           内部使用: 被mix irq线程使用
 *@param[in] utc_new 硬件定时器派发给mix_irq包裹中携带的新utc信息
 */
void app_module_clock_timestamp_update(uint64_t utc_new)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    /* 获得时钟更新 */
    app_module_clock_t clock_old = app_module_clock[1];
    if (utc_new != 0)
        app_module_clock[1].utc = utc_new;
    if (utc_new == 0)
        app_module_clock[1].utc++;
    app_module_clock_to_dtime(&app_module_clock[1]);
    app_module_clock_to_week(&app_module_clock[1]);
    app_module_clock_t clock_new = app_module_clock[1];
    app_module_clock_sec_tick++;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
    /* 秒回调集更新 */
    if (clock_old.second != clock_new.second)
        for (uint32_t idx = 0; idx < app_module_clock_second_cb_size; idx++)
            app_module_clock_second_cb[idx](&clock_new);
    /* 分回调集更新 */
    if (clock_old.minute != clock_new.minute)
        for (uint32_t idx = 0; idx < app_module_clock_minute_cb_size; idx++)
            app_module_clock_minute_cb[idx](&clock_new);
    /* 时回调集更新 */
    if (clock_old.hour != clock_new.hour)
        for (uint32_t idx = 0; idx < app_module_clock_hour_cb_size; idx++)
            app_module_clock_hour_cb[idx](&clock_new);
    /* 天回调集更新 */
    if (clock_old.day != clock_new.day)
        for (uint32_t idx = 0; idx < app_module_clock_day_cb_size; idx++)
            app_module_clock_day_cb[idx](&clock_new);
    /*  */
    APP_SYS_LOG_INFO("clock_old: utc=%lu,%u, %u-%u-%u, %u:%u:%u",
                      clock_old.utc,clock_old.week,
                      clock_old.year,clock_old.month,clock_old.day,
                      clock_old.hour,clock_old.minute,clock_old.second);
    APP_SYS_LOG_INFO("clock_new: utc=%lu,%u, %u-%u-%u, %u:%u:%u",
                      clock_new.utc,clock_new.week,
                      clock_new.year,clock_new.month,clock_new.day,
                      clock_new.hour,clock_new.minute,clock_new.second);
}

/*@brief 系统时钟复位清除
 */
void app_module_clock_clean(void)
{
    app_module_clock_t clock = {.year = 2020, .month = 1, .day = 1,};
    app_module_clock_to_week(&clock);
    app_module_clock_to_utc(&clock);
    app_module_clock_set_system_clock(&clock);
}

/*@brief 系统时钟转储到外存
 */
void app_module_clock_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_clock_t clock;
            uint32_t crc32;
        };
    } clock_data;
    
    app_module_clock_get_system_clock(&clock_data.clock);
    clock_data.crc32 = app_sys_crc32(clock_data.buffer, sizeof(app_module_clock_t));
    app_sys_ext_src_write("mix_chunk_small", "system clock", clock_data.buffer, sizeof(clock_data));
}

/*@brief 系统时钟加载到内存
 */
void app_module_clock_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_clock_t clock;
            uint32_t crc32;
        };
    } clock_data;
    
    app_sys_ext_src_read("mix_chunk_small", "system clock", clock_data.buffer, sizeof(clock_data));
    uint32_t crc32 = app_sys_crc32(clock_data.buffer, sizeof(app_module_clock_t));
    if (crc32 == clock_data.crc32)
        app_module_clock_set_system_clock(&clock_data.clock);
    if (crc32 != clock_data.crc32) {
        app_module_clock_clean();
        APP_SYS_LOG_WARN("load system clock fail");
    }
}

/*@brief 系统时钟软件定时器模组回调
 */
static void app_module_clock_timer_handler(void *timer)
{
    static app_module_rtc_t rtc = {0};
    app_module_rtc_get(&rtc);
    app_thread_package_t package = {
        .thread   = app_thread_id_mix_irq,
        .module   = app_thread_mix_irq_clock,
        .event    = app_thread_mix_irq_clock_timestamp_update,
        .priority = app_thread_package_priority_highest,
        .dynamic  = false,
        .size     = sizeof(uint64_t),
        .data     = &rtc.utc,
    };
    app_thread_package_notify(&package);
}

/*@brief 系统时钟模组初始化
 *       内部使用: 被mix irq线程使用
 */
void app_module_clock_ready(void)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_static);
    app_module_clock_timer.expired = app_module_clock_timer_handler;
    app_module_clock_timer.peroid  = 1000;
    app_module_clock_timer.reload  = true;
    app_sys_timer_start(&app_module_clock_timer);
}
