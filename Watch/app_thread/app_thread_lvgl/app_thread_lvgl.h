#ifndef APP_THREAD_LVGL_H
#define APP_THREAD_LVGL_H

typedef enum {  /* 线程模组 */
    app_thread_lvgl_work,            /* 工作队列模组 */
} app_thread_lvgl_module;

typedef struct {    /* 工作队列模组数据项 */
    void (*routine)(void *parameter);   /* 数据处理例程 */
    void  *parameter;                   /* 数据处理例程参数 */
} app_thread_lvgl_work_t;

#endif
