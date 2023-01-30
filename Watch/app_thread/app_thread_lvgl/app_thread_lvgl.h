#ifndef APP_THREAD_LVGL_H
#define APP_THREAD_LVGL_H

typedef enum {  /* 线程模组 */
    app_thread_lvgl_work,           /* 工作队列模组 */
    app_thread_lvgl_sched,          /* GUI事件调度模组 */
    app_thread_lvgl_ui_scene,       /* UI场景 */
} app_thread_lvgl_module;

typedef struct {    /* 工作队列模组数据项 */
    void (*routine)(void *parameter);   /* 数据处理例程 */
    void  *parameter;                   /* 数据处理例程参数 */
} app_thread_lvgl_work_t;

typedef enum {  /* GUI事件调度模组事件 */
    app_thread_lvgl_sched_reduce,
    app_thread_lvgl_sched_sdl,
} app_thread_lvgl_sched_event;

typedef enum {  /* GUI场景事件 */
    app_thread_lvgl_ui_scene_start,     /* UI场景进入 */
    app_thread_lvgl_ui_scene_sched,     /* UI场景调度 */
    app_thread_lvgl_ui_scene_wake,      /* UI场景唤醒 */
    app_thread_lvgl_ui_scene_sleep,     /* UI场景休眠 */
    app_thread_lvgl_ui_scene_stop,      /* UI场景退出 */
} app_thread_lvgl_ui_scene_event;

/* lvgl:tick */
#define LV_SCHED_TICK_REDUCE    5

/* lvgl:sdl check */
#define LV_SCHED_SDL_EVNET      10

/*@brief lvgl tick更新
 */
void app_lv_tick_reduce_update(void);

/*@brief lvgl sdl更新
 */
void app_lv_sdl_update(void);

#endif
