#ifndef APP_OS_ADAPTOR_WINDOWS_H
#define APP_OS_ADAPTOR_WINDOWS_H

#if APP_OS_IS_WINDOWS

#include <windows.h>
#include <Windows.h>

/* app thread */
typedef struct {
    //优先级枚举:
    //THREAD_PRIORITY_HIGHEST
    //THREAD_PRIORITY_ABOVE_NORMAL
    //THREAD_PRIORITY_NORMAL
    //THREAD_PRIORITY_BELOW_NORMAL
    HANDLE                  handle;     /* 线程句柄 */
    int                     priority;   /* 优先级 */
    size_t                  stacksize;  /* 堆栈大小 */
    LPTHREAD_START_ROUTINE  routine;    /* 线程体 */
    LPVOID                  args;       /* 线程参数 */
} app_thread_t;

/* app semaphore */
typedef struct {
    HANDLE sem;     /* 信号量空间 */
} app_sem_t;

/* app mutex */
typedef struct {
    HANDLE mutex;   /* 互斥锁空间 */
} app_mutex_t;

/*@brief 打印输出转接宏
 */
#define APP_OS_PRINT    printf

#endif

#endif
