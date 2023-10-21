#ifndef APP_MODULE_DAILY_TIME_H
#define APP_MODULE_DAILY_TIME_H

/* 每日时间表结构 */
#define APP_MODULE_DAILY_TIME_T(type_t, num)        \
typedef struct {                                    \
    uint8_t week:7;                                 \
    uint8_t bits:num;                               \
    struct {                                        \
        uint8_t time_s[3];  /* 开始[时,分,秒] */    \
        uint8_t time_e[3];  /* 结束[时,分,秒] */    \
    } table[num];                                   \
} type_t;                                           \

/* 每日时间表检查函数 */
#define APP_MODULE_DAILY_TIME_F(type_t, func, num)              \
static bool func(type_t *inst)                                  \
{                                                               \
    static uint64_t utc = -1;                                   \
    static bool  status = false;                                \
    app_module_clock_t clock = {0};                             \
    app_module_clock_get_system_clock((&(clock)));              \
    if (utc == clock.utc)   /* 单个秒内的多发访问 */            \
        return status;                                          \
        utc  = clock.utc;                                       \
    if (inst->week & (1 << clock.week) == 0)                    \
        return status = false;                                  \
    uint16_t secs_s = 0;                                        \
    uint16_t secs_e = 0;                                        \
    uint16_t secs_c = clock.hour * 60 * 60 +                    \
                      clock.minute * 60 +                       \
                      clock.second;                             \
    for (uint32_t idx = 0; idx < num; idx++)                    \
        if (inst->bits & (1 << idx) != 0) {                     \
            secs_s  = 0;                                        \
            secs_s += inst->table[idx].time_s[0] * 60 * 60;     \
            secs_s += inst->table[idx].time_s[1] * 60;          \
            secs_s += inst->table[idx].time_s[2];               \
            secs_e  = 0;                                        \
            secs_s += inst->table[idx].time_e[0] * 60 * 60;     \
            secs_s += inst->table[idx].time_e[1] * 60;          \
            secs_s += inst->table[idx].time_e[2];               \
            if (secs_s <= secs_c && secs_c <= secs_e)           \
                return status = true;                           \
        }                                                       \
    return status = false;                                      \
}                                                               \

#endif
