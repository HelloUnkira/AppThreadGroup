#ifndef APP_THREAD_LVGL_H
#define APP_THREAD_LVGL_H

typedef enum {  /* 线程模组 */
    app_thread_lvgl_none = 0,       /* 系统模组,占位符(线程组公共事件) */
    app_thread_lvgl_sched,          /* lvgl系统事件调度模组 */
    app_thread_lvgl_sched_ui,       /* lvgl场景事件调度模组 */
} app_thread_lvgl_module_t;

typedef enum {  /* GUI事件调度模组事件 */
    app_thread_lvgl_sched_inc,          /* lvgl时钟约减事件 */
    app_thread_lvgl_sched_exec,         /* lvgl时钟调度事件 */
    app_thread_lvgl_sched_drv,          /* lvgl驱动检查事件 */
    app_thread_lvgl_sched_wheel,        /* lvgl wheel更新事件 */
    app_thread_lvgl_sched_check_time,   /* lvgl场景计时检查 */
    app_thread_lvgl_sched_dlps_enter,   /* lvgl进入dlps事件 */
    app_thread_lvgl_sched_dlps_exit,    /* lvgl退出dlps事件 */
} app_thread_lvgl_sched_event_t;

typedef enum {  /* GUI场景事件 */
    app_thread_lvgl_ui_scene_wheel,                 /* UI场景轮盘配置 */
    app_thread_lvgl_ui_scene_start,                 /* UI场景进入 */
    app_thread_lvgl_ui_scene_stop,                  /* UI场景退出 */
    app_thread_lvgl_ui_scene_shutdown,              /* UI场景关机 */
    /* 下述事件对应于特定场景内需要的事件 */
    app_thread_lvgl_ui_countdown_remind,            /* 倒计时提醒事件 */
    app_thread_lvgl_ui_remind_alarm,                /* 提醒闹钟事件 */
    app_thread_lvgl_ui_remind_sedentary,            /* 提醒走动事件 */
    app_thread_lvgl_ui_remind_drink,                /* 提醒喝水事件 */
} app_thread_lvgl_ui_scene_event_t;

/* lvgl:tick */
#define LV_SCHED_TICK_INC       1
/* lvgl:exec */
#define LV_SCHED_TICK_EXEC      10
/* lvgl:drv check */
#define LV_SCHED_DRV_EVENT      5

/* lvgl:developer model */
#define APP_LV_DEVELOPER_MODEL  1

#endif
