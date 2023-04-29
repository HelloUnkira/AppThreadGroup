#ifndef APP_EXT_OS_ZEPHYR_H
#define APP_EXT_OS_ZEPHYR_H

#if APP_OS_IS_ZEPHYR

#include <kernel.h>

/* app thread */
typedef struct {
    struct k_thread      thread;        /* 线程空间 */
    k_thread_stack_t    *stack;         /* 静态分配一个栈 */
    k_thread_entry_t     entry;         /* 线程体 */
    size_t               stack_size;    /* 栈大小 */
    int                  prio;          /* 优先级 */
} app_thread_t;

/* app semaphone */
typedef struct {
    struct k_sem sem;   /* 信号量空间 */
} app_sem_t;

/* app mutex */
typedef struct {
    struct k_mutex mutex;   /* 互斥锁空间 */
} app_mutex_t;

/* app execute ms */
typedef struct {
    uint32_t start;
    uint32_t end;
} app_execute_ms_t;

/*@brief 打印输出转接宏
 */
#define APP_OS_PRINT

#endif

#endif