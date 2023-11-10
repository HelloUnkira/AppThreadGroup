#ifndef APP_EXT_OS_LINUX_H
#define APP_EXT_OS_LINUX_H

#if APP_OS_IS_LINUX

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h> 

/* app thread */
typedef struct {
    pthread_t       thread;                 /* 线程空间 */
    pthread_attr_t  attribute;              /* 线程属性 */
    int             priority;               /* 优先级 */
    size_t          stacksize;              /* 堆栈大小 */
    void *        (*routine)(void *args);   /* 线程体 */
    void *          args;                   /* 线程参数 */
} app_thread_t;

/* app semaphore */
typedef struct {
    sem_t sem;  /* 信号量空间 */
} app_sem_t;

/* app mutex */
typedef struct {
    pthread_mutex_t mutex;  /* 互斥锁空间 */
} app_mutex_t;

/* app critical */
typedef struct {
    /* 纯软件环境,临界区使用互斥锁代替 */
    bool mutex_ready;
    app_mutex_t mutex;
} app_critical_t;

/* app execute ms */
typedef struct {
    struct timespec start;
    struct timespec end;
} app_execute_ms_t;

/*@brief 打印输出转接宏
 */
#define APP_OS_PRINT    printf

#endif

#endif
