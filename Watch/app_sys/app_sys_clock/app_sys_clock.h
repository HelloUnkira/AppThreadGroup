#ifndef APP_SYS_CLOCK_H
#define APP_SYS_CLOCK_H

#pragma pack(push)
#pragma pack(1)
typedef struct {
    uint64_t utc;       /* 协调世界时UTC */
    uint16_t year;      /* 年 */
    uint8_t  month;     /* 月 */
    uint8_t  day;       /* 天 */
    uint8_t  hour;      /* 时 */
    uint8_t  minute;    /* 分 */
    uint8_t  second;    /* 秒 */
    uint8_t  week;      /* 日一二三四五六:[0, 6] */
} app_sys_clock_t;
#pragma pack(pop)

/*@brief     闰年判断
 *@param[in] clock 时钟实例{.year,}
 *@retval    是否为闰年
 */
bool app_sys_clock_is_leap_year(app_sys_clock_t *clock);

/*@brief     下一闰年
 *@param[in] clock 时钟实例{.year,}
 *@param[in] leap_clock 下一闰年时钟实例{.year,}
 */
void app_sys_clock_next_leap_year(app_sys_clock_t *clock, app_sys_clock_t *leap_clock);

/*@brief     一个月有多少天
 *@param[in] clock 时钟实例{.year,.month,}
 *@retval    天数
 */
uint32_t app_sys_clock_month_days(app_sys_clock_t *clock);

/*@brief     一年过了多少天
 *@param[in] clock 时钟实例{.year,.month,.day,}
 *@retval    天数
 */
uint32_t app_sys_clock_year_days(app_sys_clock_t *clock);

/*@brief     俩个日期间的天数
 *@param[in] clock1 时钟实例{.year,.month,.day,}
 *@param[in] clock2 时钟实例{.year,.month,.day,}
 *@retval    天数
 */
uint32_t app_sys_clock_how_many_days(app_sys_clock_t *clock1, app_sys_clock_t *clock2);

/*@brief      星期转化(蔡勒公式)
 *@param[in]  clock 时钟实例{.year,.month,.day,}
 *@param[out] clock 时钟实例{.week,}
 */
void app_sys_clock_to_week(app_sys_clock_t *clock);

/*@brief      日期转化为utc
 *@param[in]  clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 *@param[out] clock 时钟实例{.utc,}
 */
void app_sys_clock_to_utc(app_sys_clock_t *clock);

/*@brief      utc转化为日期
 *@param[in]  clock 时钟实例{.utc,}
 *@param[out] clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 */
void app_sys_clock_to_dtime(app_sys_clock_t *clock);

#endif
