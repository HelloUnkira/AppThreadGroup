/*实现目标:
 *    日历布局与辅助信息
 *    本模组不含控件, 布局可由layout与btn组装
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 日期转绝对天数(距1970-01-01)
 *@param date 日期
 *@retval 绝对天数
 */
static int32_t scui_calendar_serial(scui_calendar_date_t *date)
{
    int32_t y = (int32_t)date->year - (date->month <= 2 ? 1 : 0);
    int32_t era = (y >= 0 ? y : y - 399) / 400;
    uint32_t yoe = (uint32_t)(y - era * 400);
    uint32_t mp  = (uint32_t)(date->month + (date->month >  2 ? -3 : 9));
    uint32_t doy = (153 * mp + 2) / 5 + date->day - 1;
    uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int32_t)doe - 719468;
}

/*@brief 绝对天数转日期(不含星期)
 *@param z     绝对天数
 *@param date  日期输出
 */
static void scui_calendar_civil(int32_t z, scui_calendar_date_t *date)
{
    z += 719468;
    int32_t era = (z >= 0 ? z : z - 146096) / 146097;
    uint32_t doe = (uint32_t)(z - era * 146097);
    uint32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    uint32_t mp  = (5 * doy + 2) / 153;
    
    date->year  = (uint16_t)(yoe + era * 400 + (mp < 10 ? 0 : 1));
    date->month = (uint8_t)(mp + (mp < 10 ? 3 : -9));
    date->day   = (uint8_t)(doy - (153 * mp + 2) / 5 + 1);
}

/*@brief 星期推算(1970-01-01为周四)
 *@param date 日期
 *@retval 星期(0-6; 0为周日)
 */
static uint8_t scui_calendar_week_calc(scui_calendar_date_t *date)
{
    int32_t z = scui_calendar_serial(date) + 4;
    z %= 7;
    if (z < 0) z += 7;
    return (uint8_t)z;
}

/*@brief 周内序号(距首列的天数)
 *@param week   星期(0-6; 0为周日)
 *@param monday 周一为首列
 *@retval 序号(0-6)
 */
static scui_coord_t scui_calendar_index(uint8_t week, bool monday)
{
    if (!monday)
        return week;
    return week == 0 ? 6 : week - 1;
}

/*@brief 条目填充
 *@param item  条目
 *@param date  日期
 *@param today 今日(NULL:不标记)
 *@param cur   归属当前视图
 */
static void scui_calendar_item_fill(scui_calendar_item_t *item, scui_calendar_date_t *date,
    scui_calendar_date_t *today, bool cur)
{
    item->year  = date->year;
    item->month = date->month;
    item->day   = date->day;
    item->week  = scui_calendar_week_calc(date);
    item->cur   = cur;
    item->rest  = item->week == 0 || item->week == 6;
    item->today = today != NULL &&
        today->year == date->year && today->month == date->month && today->day == date->day;
}

/*@brief 闰年检查
 *@param year 年
 *@retval 闰年
 */
bool scui_calendar_leap(uint16_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

/*@brief 月份天数
 *@param year  年
 *@param month 月(1-12)
 *@retval 天数(0:月份越界)
 */
scui_coord_t scui_calendar_days(uint16_t year, uint8_t month)
{
    static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12)
        return 0;
    if (month == 2 && scui_calendar_leap(year))
        return 29;
    return days[month - 1];
}

/*@brief 日期设立(自动推算星期)
 *@param date  日期
 *@param year  年
 *@param month 月(1-12)
 *@param day   日(1-31)
 */
void scui_calendar_date_set(scui_calendar_date_t *date,
    uint16_t year, uint8_t month, uint8_t day)
{
    SCUI_ASSERT(date != NULL);
    
    date->year  = year;
    date->month = month;
    date->day   = day;
    date->week  = scui_calendar_week_calc(date);
}

/*@brief 日期递进(自动推算星期)
 *@param date 日期
 *@param step 递进天数(可为负)
 */
void scui_calendar_date_walk(scui_calendar_date_t *date, int32_t step)
{
    SCUI_ASSERT(date != NULL);
    
    int32_t z = scui_calendar_serial(date) + step;
    scui_calendar_civil(z, date);
    date->week = scui_calendar_week_calc(date);
}

/*@brief 日期差值
 *@param date_s 起始日期
 *@param date_e 结束日期
 *@retval 相差天数(date_e - date_s)
 */
int32_t scui_calendar_date_diff(scui_calendar_date_t *date_s, scui_calendar_date_t *date_e)
{
    SCUI_ASSERT(date_s != NULL && date_e != NULL);
    
    return scui_calendar_serial(date_e) - scui_calendar_serial(date_s);
}

/*@brief 布局容量查询
 *@param type 布局格式
 *@retval item最大项数
 */
scui_coord_t scui_calendar_caps(scui_calendar_type_t type)
{
    /* 月视图最宽: 跨6周的31天月份(周首列前置会多占一行) */
    if (type == scui_calendar_type_week)
        return 7;
    if (type == scui_calendar_type_month)
        return 7 * 6;
    if (type == scui_calendar_type_year)
        return 12;
    return 0;
}

/*@brief 日历布局查询
 *@param type   布局格式
 *@param monday 周一为首列
 *@param date   锚定日期(year视图仅取year)
 *@param today  今日(NULL:不标记)
 *@param layout 布局输出
 */
void scui_calendar_layout(scui_calendar_type_t type, bool monday,
    scui_calendar_date_t *date, scui_calendar_date_t *today, scui_calendar_layout_t *layout)
{
    SCUI_ASSERT(date != NULL && layout != NULL);
    SCUI_ASSERT(layout->item != NULL);
    
    scui_calendar_date_t walk = *date;
    walk.week = scui_calendar_week_calc(&walk);
    
    /* 年视图: 12月等分, 无前后补位 */
    if (type == scui_calendar_type_year) {
        layout->row_num = 3;
        layout->col_num = 4;
        layout->num     = 0;
        
        scui_calendar_date_t date_m = *date;
        for (uint8_t month = 1; month <= 12; month++) {
            if (layout->num >= layout->cap)
                break;
            scui_calendar_date_set(&date_m, date->year, month, 1);
            scui_calendar_item_fill(&layout->item[layout->num], &date_m, today, true);
            layout->num++;
        }
        return;
    }
    
    /* 周视图: 锚定当日所在周 */
    if (type == scui_calendar_type_week) {
        layout->row_num = 1;
        layout->col_num = 7;
        layout->num     = 0;
        
        scui_calendar_date_t date_d = walk;
        scui_calendar_date_walk(&date_d, -scui_calendar_index(walk.week, monday));
        for (scui_coord_t idx = 0; idx < 7; idx++) {
            if (layout->num >= layout->cap)
                break;
            scui_calendar_item_fill(&layout->item[layout->num], &date_d, today, true);
            layout->num++;
            scui_calendar_date_walk(&date_d, 1);
        }
        return;
    }
    
    /* 月视图: 锚定当月, 前后补满整周 */
    if (type == scui_calendar_type_month) {
        scui_calendar_date_t date_1 = *date;
        scui_calendar_date_set(&date_1, date->year, date->month, 1);
        scui_coord_t days_of_month = scui_calendar_days(date->year, date->month);
        
        /* 当月1日的周内序号, 即网格前置格数 */
        scui_coord_t lead = scui_calendar_index(scui_calendar_week_calc(&date_1), monday);
        
        /* 网格起点: 当月1日所在周的周首 */
        scui_calendar_date_t date_d = date_1;
        scui_calendar_date_walk(&date_d, -(int32_t)lead);
        
        /* 网格总项数: 前置+当月+后置, 补满整周 */
        scui_coord_t total = lead + days_of_month;
        if (total % 7 != 0)
            total += 7 - total % 7;
        
        layout->row_num = total / 7;
        layout->col_num = 7;
        layout->num     = 0;
        
        for (scui_coord_t idx = 0; idx < total; idx++) {
            if (layout->num >= layout->cap)
                break;
            bool cur = date_d.month == date->month && date_d.year == date->year;
            scui_calendar_item_fill(&layout->item[layout->num], &date_d, today, cur);
            layout->num++;
            scui_calendar_date_walk(&date_d, 1);
        }
        return;
    }
    
    layout->row_num = 0;
    layout->col_num = 0;
    layout->num     = 0;
}
