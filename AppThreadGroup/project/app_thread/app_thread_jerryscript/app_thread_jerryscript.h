#ifndef APP_THREAD_JERRYSCRIPT_H
#define APP_THREAD_JERRYSCRIPT_H

typedef enum {  /* 线程模组 */
    app_thread_jerryscript_none = 0,        /* 系统模组,占位符(线程组公共事件) */
    app_thread_jerryscript_sched,           /* 事件调度模组 */
} app_thread_jerryscript_module_t;

#endif
