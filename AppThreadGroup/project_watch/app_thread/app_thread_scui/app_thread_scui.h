#ifndef APP_THREAD_SCUI_H
#define APP_THREAD_SCUI_H

/* 线程模组 */
typedef enum {
    app_thread_scui_none = 0,       /* 系统模组,占位符(线程组公共事件) */
    app_thread_scui_sched,          /* scui系统事件调度模组 */
    app_thread_scui_ui,             /* scui窗口事件调度模组 */
} app_thread_scui_module_t;

/* 线程模组事件 */
typedef enum {
    /* GUI事件调度模组事件 */
    app_thread_scui_sched_s = 0,
    app_thread_scui_sched_exec,         /* scui时钟调度事件 */
    app_thread_scui_sched_drv,          /* scui驱动检查事件 */
    app_thread_scui_sched_check_time,   /* scui窗口计时检查 */
    app_thread_scui_sched_dlps_enter,   /* scui进入dlps事件 */
    app_thread_scui_sched_dlps_exit,    /* scui退出dlps事件 */
    app_thread_scui_sched_e,
    /* GUI窗口事件 */
    app_thread_scui_ui_s = 0,
    app_thread_scui_ui_scene_start,                 /* UI窗口进入 */
    app_thread_scui_ui_scene_stop,                  /* UI窗口退出 */
    app_thread_scui_ui_scene_shutdown,              /* UI窗口关机 */
    app_thread_scui_ui_e,
    /*  */
} app_thread_scui_module_event_t;

/* scui:tick */
#define SCUI_SCHED_TICK_INC     1
/* scui:exec */
#define SCUI_SCHED_TICK_EXEC    (1000 / 60)     // 60HZ
/* scui:drv check */
#define SCUI_SCHED_DRV_EVENT    (1000 / 60)     // 60HZ

#endif
