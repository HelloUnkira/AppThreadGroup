/*实现目标:
 *    一些和时钟运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#define   APP_MODULE_CLOCK_CB_H
#include "app_module_clock_cb.h"

/*@brief 闰年判断
 *@param clock 时钟实例{.year,}
 *@retval 是否为闰年
 */
bool app_module_clock_is_leap_year(app_module_clock_t *clock)
{
    return (clock->year % 4 == 0 && clock->year % 100 != 0) || (clock->year % 400 == 0);
}

/*@brief 下一闰年
 *@param clock 时钟实例{.year,}
 *@param leap_clock 下一闰年时钟实例{.year,}
 */
void app_module_clock_next_leap_year(app_module_clock_t *clock, app_module_clock_t *leap_clock)
{
    *leap_clock = *clock;
    // 根据闰年定义，可知8年内必有一个闰年
    for(uint32_t idx = 0; idx < 8; idx++) {
        leap_clock->year++;
        if(app_module_clock_is_leap_year(leap_clock))
            break;
    }
}

/*@brief 一个月有多少天
 *@param clock 时钟实例{.year,.month,}
 *@retval 天数
 */
uint32_t app_module_clock_month_days(app_module_clock_t *clock)
{
    // const month = {31, (28, 29), 31, 30, 31, 30, 31
    //                31,  30     , 31, 30, 31,}
    return clock->month == 2 ? app_module_clock_is_leap_year(clock) ? 29 : 28 :
           clock->month == 7 ? 31 : clock->month % 7 % 2 == 0 ? 30 : 31;
}

/*@brief 一年过了多少天
 *@param clock 时钟实例{.year,.month,.day,}
 *@retval 天数
 */
uint32_t app_module_clock_year_days(app_module_clock_t *clock)
{
    uint32_t days = clock->day;
    for (uint32_t month = 1; month < clock->month; month++) {
        app_module_clock_t clock1 = {.year = clock->year,.month = month,};
        days += app_module_clock_month_days(&clock1);
    }
    /* 已经过去的天数,今天不能算进去 */
    return days - 1;
}

/*@brief 俩个日期间的天数
 *@param clock1 时钟实例{.year,.month,.day,}
 *@param clock2 时钟实例{.year,.month,.day,}
 *@retval 天数
 */
uint32_t app_module_clock_how_many_days(app_module_clock_t *clock1, app_module_clock_t *clock2)
{
    if (!(clock1->year  <= clock2->year  ||
          clock1->month <= clock2->month ||
          clock1->day   <= clock2->day))
          return -1;
    
    uint32_t days = 0;
    app_module_clock_t clock = {0};
    for (clock.year = clock1->year + 1; clock.year <= clock2->year - 1; clock.year++)
        days += app_module_clock_is_leap_year(&clock) ? 366 : 365;
    if (clock1->year != clock2->year)
        days += app_module_clock_is_leap_year(clock1) ? 366 : 365;
        days += app_module_clock_year_days(clock2);
        days -= app_module_clock_year_days(clock1);
    return days;
}

/*@brief 星期转化(蔡勒公式)
 *@param clock 时钟实例{.year,.month,.day,}
 *@param clock 时钟实例{.week,}
 */
void app_module_clock_to_week(app_module_clock_t *clock)
{
    uint32_t y = clock->year % 100;
    uint32_t c = clock->year / 100;
    uint32_t m = clock->month;
    uint32_t d = clock->day;
    if (m == 1 || m == 2) {
        m += 12;
        y--;
    }
    /* 蔡勒公式 */
    clock->week = (y + y / 4 + c / 4 - 2 * c + 13 * (m + 1) / 5 + d - 1) % 7;
}

/*@brief 日期转化为utc
 *@param clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 *@param clock 时钟实例{.utc,}
 */
void app_module_clock_to_utc(app_module_clock_t *clock)
{
    app_module_clock_t bclock = {
        .year = 1970, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0,
    };
    
    clock->utc  = app_module_clock_how_many_days(&bclock, clock) * 60 * 60 * 24;
    clock->utc += clock->hour   * 60 * 60;
    clock->utc += clock->minute * 60;
    clock->utc += clock->second;
    clock->utc -= bclock.hour   * 60 * 60;
    clock->utc -= bclock.minute * 60;
    clock->utc -= bclock.second;
}

/*@brief utc转化为日期
 *@param clock 时钟实例{.utc,}
 *@param clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 */
void app_module_clock_to_dtime(app_module_clock_t *clock)
{
    /* 代码逻辑来源于liunx\..\kdb_gmtime(...), 有效期:1970-2099 */
    uint32_t leap_month[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint32_t seconds = clock->utc % (24 * 60 * 60);
    uint32_t days    = clock->utc / (24 * 60 * 60) + (2 * 365 + 1);
    /* 参考点转移到1968年, 因为这一年是闰年 */
    clock->second = seconds % 60;
    clock->minute = seconds / 60 % 60;
    clock->hour   = seconds / 60 / 60;
    clock->year   = 1968 + 4 * (days / (4 * 365 + 1));
    clock->month  = 1;
    days %= (4 * 365 + 1);
    while (days >= leap_month[clock->month - 1]) {
           days -= leap_month[clock->month - 1];
            clock->month++;
        if (clock->month == 13) {
            clock->month  = 1;
            clock->year++;
            leap_month[1] = 28;
        }
    }
    clock->day = days + 1;
}

/* 分界线:<上面是模组通用接口, 下面是模组功能定制接口> */
static uint64_t app_module_clock_sec_tick = {0};
static app_mutex_t app_module_clock_mutex = {0};
static app_sys_timer_t app_module_clock_timer = {0};
static app_module_clock_t app_module_clock[2] = {0};

/*@brief 获得系统开机时间(中断环境下不可调用)
 *@param clock 时钟实例
 */
uint64_t app_module_clock_get_sec_tick(void)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    uint64_t retval = app_module_clock_sec_tick;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
    return retval;
}

/*@brief 设置时区偏移(中断环境下不可调用)
 *@param zone_sec 时区偏移(秒)
 */
void app_module_clock_set_system_clock_zone(int32_t zone_sec)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    app_module_clock[0].zone_sec = zone_sec;
    app_module_clock[1].zone_sec = zone_sec;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
}

/*@brief 设置时区偏移(中断环境下不可调用)
 *@param is_24 时区偏移(秒)
 */
void app_module_clock_set_system_clock_mode(bool is_24)
{
    app_mutex_process(&app_module_clock_mutex, app_mutex_take);
    app_module_clock[0].is_24 = is_24;
    app_module_clock[1].is_24 = is_24;
    app_mutex_process(&app_module_clock_mutex, app_mutex_give);
}

/*@brief 获得系统时间(中断环境下不可调用)
 *@param clock 时钟实例
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

/*@brief 设置系统时间(中断环境下不可调用)
 *@param clock 时钟实例
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
    
    /* 更新数据中心资源 */
    app_module_clock_t local_clock = {0};
    app_module_clock_get_system_clock(&local_clock);
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.clock, &local_clock, sizeof(app_module_clock_t));
    app_module_data_center_give();
}

/*@brief 系统时间戳更新回调
 *       内部使用: 被mix irq线程使用
 *@param utc_new 硬件定时器派发给mix_irq包裹中携带的新utc信息
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
    
    /* 更新数据中心资源 */
    app_module_clock_t local_clock = {0};
    app_module_clock_get_system_clock(&local_clock);
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.clock, &local_clock, sizeof(app_module_clock_t));
    app_module_data_center_give();
}

/*@brief 系统时钟复位清除
 *@param clock 时钟实例
 */
void app_module_clock_reset(app_module_clock_t *clock)
{
    clock->year   = 2020;
    clock->month  = 1;
    clock->day    = 1;
    clock->hour   = 0;
    clock->minute = 0;
    clock->second = 0;
    app_module_clock_to_utc(clock);
    app_module_clock_to_week(clock);
}

/*@brief 系统时钟软件定时器模组回调
 */
static void app_module_clock_timer_handler(void *timer)
{
    static app_module_rtc_t rtc = {0};
    app_module_rtc_get(&rtc);
    app_thread_package_t package = {
        .thread     = app_thread_id_mix_irq,
        .module     = app_thread_mix_irq_clock,
        .event      = app_thread_mix_irq_clock_timestamp_update,
        .priority   = app_thread_package_priority_highest,
        .byte_fixed = rtc.utc,
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
    /* 提取数据中心资源 */
    app_module_clock_t clock = {0};
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&clock, &data_center_src->module_source.clock, sizeof(app_module_clock_t));
    app_module_clock_set_system_clock(&clock);
    app_module_data_center_give();
}
