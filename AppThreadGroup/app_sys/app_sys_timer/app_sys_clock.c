/*实现目标:
 *    时间换算相关的功能组件
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 闰年判断
 *@param clock 时钟实例{.year,}
 *@retval 是否为闰年
 */
bool app_sys_clock_is_leap_year(app_sys_clock_t *clock)
{
    return (clock->year % 4 == 0 && clock->year % 100 != 0) || (clock->year % 400 == 0);
}

/*@brief 下一闰年
 *@param clock 时钟实例{.year,}
 *@param leap_clock 下一闰年时钟实例{.year,}
 */
void app_sys_clock_next_leap_year(app_sys_clock_t *clock, app_sys_clock_t *leap_clock)
{
    *leap_clock = *clock;
    // 根据闰年定义，可知8年内必有一个闰年
    for(uint32_t idx = 0; idx < 8; idx++) {
        leap_clock->year++;
        if(app_sys_clock_is_leap_year(leap_clock))
            break;
    }
}

/*@brief 一个月有多少天
 *@param clock 时钟实例{.year,.month,}
 *@retval 天数
 */
uint32_t app_sys_clock_month_days(app_sys_clock_t *clock)
{
    // const month = {31, (28, 29), 31, 30, 31, 30, 31
    //                31,  30     , 31, 30, 31,}
    return clock->month == 2 ? app_sys_clock_is_leap_year(clock) ? 29 : 28 :
           clock->month == 7 ? 31 : clock->month % 7 % 2 == 0 ? 30 : 31;
}

/*@brief 一年过了多少天
 *@param clock 时钟实例{.year,.month,.day,}
 *@retval 天数
 */
uint32_t app_sys_clock_year_days(app_sys_clock_t *clock)
{
    uint32_t days = clock->day;
    for (uint32_t month = 1; month < clock->month; month++) {
        app_sys_clock_t clock1 = {.year = clock->year,.month = month,};
        days += app_sys_clock_month_days(&clock1);
    }
    /* 已经过去的天数,今天不能算进去 */
    return days - 1;
}

/*@brief 俩个日期间的天数
 *@param clock1 时钟实例{.year,.month,.day,}
 *@param clock2 时钟实例{.year,.month,.day,}
 *@retval 天数
 */
uint32_t app_sys_clock_how_many_days(app_sys_clock_t *clock1, app_sys_clock_t *clock2)
{
    if (!(clock1->year  <= clock2->year  ||
          clock1->month <= clock2->month ||
          clock1->day   <= clock2->day))
          return -1;
    
    uint32_t days = 0;
    app_sys_clock_t clock = {0};
    for (clock.year = clock1->year + 1; clock.year <= clock2->year - 1; clock.year++)
        days += app_sys_clock_is_leap_year(&clock) ? 366 : 365;
    if (clock1->year != clock2->year)
        days += app_sys_clock_is_leap_year(clock1) ? 366 : 365;
        days += app_sys_clock_year_days(clock2);
        days -= app_sys_clock_year_days(clock1);
    return days;
}

/*@brief 星期转化(蔡勒公式)
 *@param clock 时钟实例{.year,.month,.day,}
 *@param clock 时钟实例{.week,}
 */
void app_sys_clock_to_week(app_sys_clock_t *clock)
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
void app_sys_clock_to_utc(app_sys_clock_t *clock)
{
    app_sys_clock_t bclock = {
        .year = 1970, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0,
    };
    
    clock->utc  = app_sys_clock_how_many_days(&bclock, clock) * 60 * 60 * 24;
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
void app_sys_clock_to_dtime(app_sys_clock_t *clock)
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
