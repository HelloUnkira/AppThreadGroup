#ifndef APP_MODULE_CLOCK_H
#define APP_MODULE_CLOCK_H

typedef struct {
    uint64_t utc;       /* 协调世界时UTC */
    uint16_t year;      /* 年 */
    uint8_t  month;     /* 月 */
    uint8_t  day;       /* 天 */
    uint8_t  hour;      /* 时 */
    uint8_t  minute;    /* 分 */
    uint8_t  second;    /* 秒 */
    uint8_t  week;      /* 日一二三四五六:[0, 6] */
} app_module_clock_t;

/*@brief        闰年判断
 *@param[in]    clock 时钟实例{.year,}
 *@retval       是否为闰年
 */
bool app_module_clock_is_leap_year(app_module_clock_t *clock);

/*@brief        下一闰年
 *@param[in]    clock 时钟实例{.year,}
 *@retval       clock 时钟实例{.year,}
 */
app_module_clock_t app_module_clock_next_leap_year(app_module_clock_t *clock);

/*@brief        一个月有多少天
 *@param[in]    clock 时钟实例{.year,.month,}
 *@retval       天数
 */
uint32_t app_module_clock_month_days(app_module_clock_t *clock);

/*@brief        一年过了多少天
 *@param[in]    clock 时钟实例{.year,.month,.day,}
 *@retval       天数
 */
uint32_t app_module_clock_year_days(app_module_clock_t *clock);

/*@brief        俩个日期间的天数
 *@param[in]    clock1 时钟实例{.year,.month,.day,}
 *@param[in]    clock2 时钟实例{.year,.month,.day,}
 *@retval       天数
 */
uint32_t app_module_clock_how_many_days(app_module_clock_t *clock1, app_module_clock_t *clock2);

/*@brief        星期转化(蔡勒公式)
 *@param[in]    clock 时钟实例{.year,.month,.day,}
 *@param[out]   clock 时钟实例{.week,}
 */
void app_module_clock_to_week(app_module_clock_t *clock);

/*@brief        日期转化为utc
 *@param[in]    clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 *@param[out]   clock 时钟实例{.utc,}
 */
void app_module_clock_to_utc(app_module_clock_t *clock);

/*@brief        utc转化为日期
 *@param[in]    clock 时钟实例{.utc,}
 *@param[out]   clock 时钟实例{.year,.month,.day,.hour,.minute,.second,}
 */
void app_module_clock_to_dtime(app_module_clock_t *clock);

/* 分界线:<上面模组通用接口, 下面是模组功能定制接口> */

typedef enum {
    app_module_clock_update_default,
    app_module_clock_update_first_set,
} app_module_clock_update_event;

/* 本地时钟触发回调 */
typedef void (*app_module_clock_cb1)(app_module_clock_t clock[1]);
/* 本地时钟更新回调 */
typedef void (*app_module_clock_cb2)(app_module_clock_t clock[2], uint32_t event);

/*@brief     获得系统时间(中断环境下不可调用)
 *@param[in] clock 时钟实例
 */
void app_module_clock_get_system_clock(app_module_clock_t *clock);

/*@brief     设置系统时间(中断环境下不可调用)
 *@param[in] clock 时钟实例
 */
void app_module_clock_set_system_clock(app_module_clock_t *clock);

/*@brief 系统时钟更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_clock_event_update(void);

/*@brief     系统时间戳更新回调
 *           内部使用: 被mix custom线程使用
 *@param[in] utc_new 硬件定时器派发给mix custom包裹中携带的新utc信息
 */
void app_module_clock_timestamp_update(uint64_t utc_new);

/*@brief 系统时钟模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_clock_ready(void);

#endif
