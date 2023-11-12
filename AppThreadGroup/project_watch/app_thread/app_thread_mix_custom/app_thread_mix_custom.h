#ifndef APP_THREAD_MIX_CUSTOM_H
#define APP_THREAD_MIX_CUSTOM_H

/* 线程模组 */
typedef enum {
    app_thread_mix_custom_none = 0,     /* 系统模组,占位符(线程组公共事件) */
    app_thread_mix_custom_data_dump,    /* 资源转储模组 */
    app_thread_mix_custom_data_load,    /* 资源加载模组 */
    app_thread_mix_custom_stopwatch,    /* 秒表模组 */
    app_thread_mix_custom_countdown,    /* 倒计时模组 */
    app_thread_mix_custom_remind_group, /* 提醒组模组 */
    app_thread_mix_custom_remind_misc,  /* 杂项提醒模组 */
    app_thread_mix_custom_alarm,        /* 闹钟模组 */
} app_thread_mix_custom_module_t;

/* 线程模组 */
typedef enum {
    /* 秒表模组事件 */
    app_thread_mix_custom_stopwatch_s = 0,
    app_thread_mix_custom_stopwatch_msec_update,
    app_thread_mix_custom_stopwatch_e,
    /* 倒计时模组事件 */
    app_thread_mix_custom_countdown_s = 0,
    app_thread_mix_custom_countdown_msec_update,
    app_thread_mix_custom_countdown_expired,
    app_thread_mix_custom_countdown_e,
    /* 提醒组模组 */
    app_thread_mix_custom_remind_group_s = 0,
    app_thread_mix_custom_remind_group_update,
    app_thread_mix_custom_remind_group_package,
    /* 下面是package中携带的事件,将其打包是因为需要做进一步派发 */
    app_thread_mix_custom_remind_group_first,           /* 默认提醒点提醒 */
    app_thread_mix_custom_remind_group_first_offset,    /* 默认提醒点偏移提醒 */
    app_thread_mix_custom_remind_group_week,            /* 周轮转提醒 */
    app_thread_mix_custom_remind_group_week_offset,     /* 周轮转偏移提醒 */
    app_thread_mix_custom_remind_group_month,           /* 月轮转提醒 */
    app_thread_mix_custom_remind_group_month_offset,    /* 月轮转偏移提醒 */
    app_thread_mix_custom_remind_group_repeat,          /* 自定义周期轮转提醒 */
    app_thread_mix_custom_remind_group_repeat_offset,   /* 自定义周期轮转偏移提醒 */
    app_thread_mix_custom_remind_group_e,
    /* 杂项提醒模组事件 */
    app_thread_mix_custom_remind_misc_s = 0,
    app_thread_mix_custom_remind_sedentary_update,
    app_thread_mix_custom_remind_sedentary_interval,
    app_thread_mix_custom_remind_drink_update,
    app_thread_mix_custom_remind_drink_interval,
    app_thread_mix_custom_remind_misc_e,
    /*  */
} app_thread_mix_custom_module_event_t;

/* 子线程模组 */
#include "app_module_countdown.h"
#include "app_module_stopwatch.h"
#include "app_module_weather.h"
#include "app_module_world_time.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_remind_calendar.h"
#include "app_module_remind_menstruation.h"
#include "app_module_remind_matter.h"
#include "app_module_daily_time.h"
#include "app_module_remind_drink.h"
#include "app_module_remind_sedentary.h"
#include "app_module_do_not_disturb.h"
/* 子线程模组 */
#include "app_module_data_center_type.h"
#include "app_module_data_center_src.h"
#include "app_module_data_center.h"
#include "app_module_data_load.h"
#include "app_module_data_dump.h"

#endif
