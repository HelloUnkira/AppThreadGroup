#ifndef APP_THREAD_LVGL_H
#define APP_THREAD_LVGL_H

typedef enum {  /* 线程模组 */
    app_thread_lvgl_work,            /* 工作队列模组 */
    app_thread_lvgl_sched,           /* GUI事件调度模组 */
} app_thread_lvgl_module;

typedef struct {    /* 工作队列模组数据项 */
    void (*routine)(void *parameter);   /* 数据处理例程 */
    void  *parameter;                   /* 数据处理例程参数 */
} app_thread_lvgl_work_t;

typedef enum {  /* GUI事件调度模组事件 */
    app_thread_lvgl_sched_reduce,
    app_thread_lvgl_sched_sdl,
} app_thread_lvgl_sched_event;

/* lvgl:tick */
#define LV_SCHED_TICK_REDUCE    5

/* lvgl:sdl check */
#define LV_SCHED_SDL_EVNET      10

#endif
