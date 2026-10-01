#ifndef SCUI_CALENDAR_H
#define SCUI_CALENDAR_H

/* 日期 */
typedef struct {
    uint16_t year;      /* 年 */
    uint8_t  month;     /* 月(1-12) */
    uint8_t  day;       /* 日(1-31) */
    uint8_t  week;      /* 周(0-6; 0为周日) */
} scui_calendar_date_t;

/* 布局格式 */
typedef enum {
    scui_calendar_type_week = 0,    /* 周视图(1行7列) */
    scui_calendar_type_month,       /* 月视图(7列, 行数自适应; 含前后月补位) */
    scui_calendar_type_year,        /* 年视图(12月) */
    scui_calendar_type_num,
} scui_calendar_type_t;

/* 单元格条目(布局与辅助信息) */
typedef struct {
    uint16_t      year;             /* 年 */
    uint8_t       month;            /* 月(1-12) */
    uint8_t       day;              /* 日(1-31) */
    uint8_t       week;             /* 周(0-6; 0为周日) */
    scui_sbitfd_t cur:1;            /* 归属当前视图(否则为前后补位) */
    scui_sbitfd_t today:1;          /* 今日 */
    scui_sbitfd_t rest:1;           /* 周末 */
} scui_calendar_item_t;

/* 日历布局
 * item缓冲区由调用方提供, 容量应不小于scui_calendarCaps查询值
 */
typedef struct {
    scui_coord_t         row_num;   /* 轨道数量 */
    scui_coord_t         col_num;   /* 列数量 */
    scui_coord_t         num;       /* 实际项数 */
    scui_coord_t         cap;       /* item容量 */
    scui_calendar_item_t *item;     /* item缓冲区 */
} scui_calendar_layout_t;

/*@brief 闰年检查
 *@param year 年
 *@retval 闰年
 */
bool scui_calendar_leap(uint16_t year);

/*@brief 月份天数
 *@param year  年
 *@param month 月(1-12)
 *@retval 天数(0:月份越界)
 */
scui_coord_t scui_calendar_days(uint16_t year, uint8_t month);

/*@brief 日期设立(自动推算星期)
 *@param date  日期
 *@param year  年
 *@param month 月(1-12)
 *@param day   日(1-31)
 */
void scui_calendar_date_set(scui_calendar_date_t *date,
    uint16_t year, uint8_t month, uint8_t day);

/*@brief 日期递进(自动推算星期)
 *@param date 日期
 *@param step 递进天数(可为负)
 */
void scui_calendar_date_walk(scui_calendar_date_t *date, int32_t step);

/*@brief 日期差值
 *@param date_s 起始日期
 *@param date_e 结束日期
 *@retval 相差天数(date_e - date_s)
 */
int32_t scui_calendar_date_diff(scui_calendar_date_t *date_s, scui_calendar_date_t *date_e);

/*@brief 布局容量查询
 *@param type 布局格式
 *@retval item最大项数
 */
scui_coord_t scui_calendar_caps(scui_calendar_type_t type);

/*@brief 日历布局查询
 *@param type   布局格式
 *@param monday 周一为首列
 *@param date   锚定日期(year视图仅取year)
 *@param today  今日(NULL:不标记)
 *@param layout 布局输出
 */
void scui_calendar_layout(scui_calendar_type_t type, bool monday,
    scui_calendar_date_t *date, scui_calendar_date_t *today, scui_calendar_layout_t *layout);

#endif
