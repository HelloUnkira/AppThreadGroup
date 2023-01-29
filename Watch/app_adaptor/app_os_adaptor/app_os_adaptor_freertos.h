#ifndef APP_OS_ADAPTOR_FREERTOS_H
#define APP_OS_ADAPTOR_FREERTOS_H

#if APP_OS_IS_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* app thread */
typedef struct {
    TaskHandle_t         handle;        /* 线程空间 */
    TaskFunction_t       task;          /* 线程体 */
    const char          *name;          /* 线程名字 */
    StackType_t          stack;         /* 栈空间 */
    uint16_t             stack_size;    /* 栈空间大小 */
    UBaseType_t          priority;      /* 优先级 */
} app_thread_t;

/* app semaphone */
typedef struct {
    SemaphoreHandle_t sem;  /* 信号量空间 */
} app_sem_t;

/* app mutex */
typedef struct {
    SemaphoreHandle_t mutex;  /* 互斥锁空间 */
} app_mutex_t;

/*@brief 打印输出转接宏
 */
#define APP_OS_PRINT

#endif

#endif