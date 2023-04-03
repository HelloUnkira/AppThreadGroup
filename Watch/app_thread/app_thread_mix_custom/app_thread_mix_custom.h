#ifndef APP_THREAD_MIX_CUSTOM_H
#define APP_THREAD_MIX_CUSTOM_H

typedef enum {  /* 线程模组 */
    app_thread_mix_custom_system = 0,   /* 系统模组 */
    app_thread_mix_custom_stopwatch,    /* 秒表模组 */
    app_thread_mix_custom_countdown,    /* 倒计时模组 */
    app_thread_mix_custom_remind_group, /* 提醒组模组 */
    app_thread_mix_custom_remind_misc,  /* 杂项提醒模组 */
    app_thread_mix_custom_alarm,        /* 闹钟模组 */
} app_thread_mix_custom_module;

typedef enum {  /* 秒表模组事件 */
    app_thread_mix_custom_stopwatch_msec_update,
} app_thread_mix_custom_stopwatch_event;

typedef enum {  /* 倒计时模组事件 */
    app_thread_mix_custom_countdown_msec_update,
    app_thread_mix_custom_countdown_expired,
} app_thread_mix_custom_countdown_event;

typedef enum {  /* 提醒组模组 */
    app_thread_mix_custom_remind_group_update,
    app_thread_mix_custom_remind_group_first,           /* 默认提醒点提醒 */
    app_thread_mix_custom_remind_group_first_offset,    /* 默认提醒点偏移提醒 */
    app_thread_mix_custom_remind_group_week,            /* 周轮转提醒 */
    app_thread_mix_custom_remind_group_week_offset,     /* 周轮转偏移提醒 */
    app_thread_mix_custom_remind_group_month,           /* 月轮转提醒 */
    app_thread_mix_custom_remind_group_month_offset,    /* 月轮转偏移提醒 */
    app_thread_mix_custom_remind_group_repeat,          /* 自定义周期轮转提醒 */
    app_thread_mix_custom_remind_group_repeat_offset,   /* 自定义周期轮转偏移提醒 */
}app_thread_mix_custom_remind_group_event;

typedef enum {  /* 闹钟模组事件 */
    /* 基础闹钟事件 */
    app_thread_mix_custom_alarm_expired,
    /* 常规式闹钟事件 */
    app_thread_mix_custom_alarm_month,
    app_thread_mix_custom_alarm_week,
    /* 提醒式闹钟事件 */
    app_thread_mix_custom_alarm_repeat,
} app_thread_mix_custom_alarm_event;

typedef enum {  /* 杂项提醒模组事件 */
    app_thread_mix_custom_remind_drink_update,
    app_thread_mix_custom_remind_drink_interval,
} app_thread_mix_custom_remind_misc_event;

#endif
