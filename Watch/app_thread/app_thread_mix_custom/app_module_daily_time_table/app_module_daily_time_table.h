#ifndef APP_MODULE_DAILY_TIME_TABLE_H
#define APP_MODULE_DAILY_TIME_TABLE_H

/* 每日时间表结构定义 */
#define APP_MODULE_DAILY_TIME_TABLE_T           \
       (app_module_daily_time_table_t, number)  \
typedef struct {                                \
    uint8_t week:7;                             \
    uint8_t bits:number;                        \
    struct {                                    \
        struct {                                \
            uint8_t hour;                       \
            uint8_t minute;                     \
            uint8_t second;                     \
        } start, end;                           \
    } table[number];                            \
} app_module_daily_time_table_t;                \

/* 每日时间表有效性检查函数 */
#define APP_MODULE_DAILY_TIME_TABLE_FUNC                    \
       (app_module_daily_time_table_func, number)           \
bool    app_module_daily_time_table_func(void *instance)    \
{                                                           \
    app_module_clock_t clock = {0};                         \
    app_module_clock_get_system_clock(&clock);              \
    APP_MODULE_DAILY_TIME_TABLE_T(table_t, number);         \
    table_t * table = instance;                             \
    if (table.week & (1 << clock.week) == 0)                \
        return false;                                       \
    uint16_t c_secs = clock.hour * 60 * 60 +                \
                      clock.minute * 60 +                   \
                      clock.second;                         \
    for (uint32_t idx = 0; idx < len; idx++)                \
    if (table.bits & (1 << idx) != 0) {                     \
        uint16_t t_secs_s = table.table[idx].start.hour * 60 * 60 +     \
                            table.table[idx].start.minute * 60 +        \
                            table.table[idx].start.second;              \
        uint16_t t_secs_e = table.table[idx].end.hour * 60 * 60 +       \
                            table.table[idx].end.minute * 60 +          \
                            table.table[idx].end.second;                \
        if (t_secs_s <= c_secs && c_secs <= t_secs_e)                   \
            return true;                                    \
    }                                                       \
    return false;                                           \
}                                                           \

#endif
