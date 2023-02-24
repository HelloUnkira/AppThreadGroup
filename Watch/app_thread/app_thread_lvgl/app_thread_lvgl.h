#ifndef APP_THREAD_LVGL_H
#define APP_THREAD_LVGL_H

typedef enum {  /* 线程模组 */
    app_thread_lvgl_system = 0,     /* 系统模组 */
    app_thread_lvgl_work,           /* 工作队列模组 */
    app_thread_lvgl_sched,          /* GUI事件调度模组 */
    app_thread_lvgl_ui_scene,       /* UI场景基础模组 */
} app_thread_lvgl_module;

typedef enum {  /* 系统模组事件 */
    app_thread_lvgl_system_watchdog_feed,
} app_thread_lvgl_system_event;

typedef struct {    /* 工作队列模组数据项 */
    void (*routine)(void *parameter);   /* 数据处理例程 */
    void  *parameter;                   /* 数据处理例程参数 */
} app_thread_lvgl_work_t;

typedef enum {  /* GUI事件调度模组事件 */
    app_thread_lvgl_sched_inc,          /* lvgl时钟约减事件 */
    app_thread_lvgl_sched_exec,         /* lvgl时钟调度事件 */
    app_thread_lvgl_sched_drv,          /* lvgl驱动检查事件 */
    app_thread_lvgl_sched_scene,        /* lvgl场景处理事件 */
    app_thread_lvgl_sched_dlps_enter,   /* lvgl进入dlps事件 */
    app_thread_lvgl_sched_dlps_exit,    /* lvgl退出dlps事件 */
} app_thread_lvgl_sched_event;

typedef enum {  /* GUI场景事件 */
    app_thread_lvgl_ui_scene_start,     /* UI场景进入 */
    app_thread_lvgl_ui_scene_stop,      /* UI场景退出 */
    /* 下述事件对应于特定场景内需要的事件 */
    app_thread_lvgl_ui_countdown_remind,            /* 倒计时提醒事件 */
} app_thread_lvgl_ui_scene_event;

/* lvgl:tick */
#define LV_SCHED_TICK_INC       1

/* lvgl:exec */
#define LV_SCHED_TICK_EXEC      5

/* lvgl:sdl check */
#define LV_SCHED_SDL_EVNET      5

/*@brief lvgl tick更新
 */
void app_lv_tick_inc_update(void);

/*@brief lvgl tick执行
 */
void app_lv_tick_exec_update(void);

/*@brief lvgl drv更新
 */
void app_lv_drv_update(void);

#endif
