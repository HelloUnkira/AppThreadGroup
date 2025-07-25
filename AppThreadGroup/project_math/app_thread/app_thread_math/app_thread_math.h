#ifndef APP_THREAD_MATH_H
#define APP_THREAD_MATH_H

typedef enum {  /* 线程模组 */
    app_thread_math_none = 0,           /* 系统模组,占位符(线程组公共事件) */
    app_thread_math_sched,              /* 事件调度模组 */
} app_thread_math_module_t;

#endif
