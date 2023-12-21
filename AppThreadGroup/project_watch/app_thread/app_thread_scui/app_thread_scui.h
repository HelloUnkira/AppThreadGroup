#ifndef APP_THREAD_SCUI_H
#define APP_THREAD_SCUI_H

/* 线程模组 */
typedef enum {
    app_thread_scui_none = 0,       /* 系统模组,占位符(线程组公共事件) */
    app_thread_scui_sched,          /* scui系统事件调度模组 */
    app_thread_scui_ui,             /* scui场景事件调度模组 */
} app_thread_scui_module_t;

/* 线程模组事件 */
typedef enum {
    /* GUI事件调度模组事件 */
    app_thread_scui_sched_s = 0,
    app_thread_scui_sched_e,
    /* GUI场景事件 */
    app_thread_scui_ui_s = 0,
    app_thread_scui_ui_e,
    /*  */
} app_thread_scui_module_event_t;

#endif
