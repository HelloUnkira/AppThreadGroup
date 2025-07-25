/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_OS_IS_WINDOWS

/*@brief 初始化OS
 */
void app_os_ready(void)
{
}

/*@brief 调度OS
 */
void app_os_schedule(void)
{
}

/*@brief 当前环境是否为中断环境(注意:当且仅当必要的使用)
 */
bool app_os_not_in_irq(void)
{
    /* 模拟器环境不存在硬件中断环境 */
    return true;
}

/*@brief 线程操作流程集合
 *@param thread 实例
 *@param option 实例动作
 */
void app_thread_process(app_thread_t *thread, app_thread_option_t option)
{
    switch (option) {
    case app_thread_static: {
        thread->stacksize = 8 * 1024 * 1024;
        // thread->handle = _beginthreadex(NULL,
                                        // thread->stacksize, /* 堆栈大小 */
                                        // thread->routine,   /* 线程体 */
                                        // thread->args,      /* 线程参数 */
                                        // CREATE_SUSPENDED,
                                        // NULL);
        DWORD ThreadID = 0;
        thread->handle = CreateThread(NULL,
            thread->stacksize, /* 堆栈大小 */
            (LPTHREAD_START_ROUTINE)
            thread->routine,   /* 线程体 */
            thread->args,      /* 线程参数 */
            CREATE_SUSPENDED,
            &ThreadID);
        SetThreadPriority(thread->handle, thread->priority);
        ResumeThread(thread->handle);
        break;
    }
    default:
        APP_OS_PRINT("app_thread_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 信号量操作流程集合
 *@param sem    实例
 *@param option 实例动作
 */
void app_sem_process(app_sem_t *sem, app_sem_option_t option)
{
    switch (option) {
    case app_sem_create:
    case app_sem_static: {
        sem->sem = CreateSemaphore(NULL, 0, 100, NULL);
        break;
    }
    case app_sem_destroy: {
        CloseHandle(sem->sem);
        break;
    }
    case app_sem_take: {
        WaitForSingleObject(sem->sem, INFINITE);
        break;
    }
    case app_sem_give: {
        ReleaseSemaphore(sem->sem, 1, NULL);
        break;
    }
    default:
        APP_OS_PRINT("app_sem_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 互斥锁操作流程集合
 *@param mutex  实例
 *@param option 实例动作
 */
void app_mutex_process(app_mutex_t *mutex, app_mutex_option_t option)
{
    switch (option) {
    case app_mutex_create:
    case app_mutex_static: {
        mutex->mutex = CreateMutex(NULL, FALSE, NULL);
        break;
    }
    case app_mutex_destroy: {
        CloseHandle(mutex->mutex);
        break;
    }
    case app_mutex_take: {
        if (app_os_not_in_irq())
            WaitForSingleObject(mutex->mutex, INFINITE);
        break;
    }
    case app_mutex_give: {
        if (app_os_not_in_irq())
            ReleaseMutex(mutex->mutex);
        break;
    }
    default:
        APP_OS_PRINT("app_mutex_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 临界区操作流程集合
 *@param critical 实例
 *@param option   实例动作
 */
void app_critical_process(app_critical_t *critical, app_critical_option_t option)
{
    switch (option) {
    case app_critical_static: {
        /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        app_mutex_process(&critical->mutex, app_mutex_static);
        critical->mutex_ready = true;
        break;
    }
    case app_critical_create: {
        /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        app_mutex_process(&critical->mutex, app_mutex_create);
        critical->mutex_ready = true;
        break;
    }
    case app_critical_destroy: {
        /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        app_mutex_process(&critical->mutex, app_mutex_destroy);
        critical->mutex_ready = false;
        break;
    }
    case app_critical_enter: {
        /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        if (critical->mutex_ready)
            app_mutex_process(&critical->mutex, app_mutex_take);
        break;
    }
    case app_critical_exit: {
        /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
        if (critical->mutex_ready)
            app_mutex_process(&critical->mutex, app_mutex_give);
        break;
    }
    default:
        APP_OS_PRINT("app_critical_process option is not unsupported:%u", option);
        app_arch_reset();
        break;
    }
}

/*@brief 内存分配
 *@param size 分配空间字节大小
 *@retval 分配空间,失败为NULL
 */
void * app_mem_alloc(uint32_t size)
{
    return malloc(size);
}

/*@brief 内存分配
 *@param pointer 分配空间回收
 *@param size    分配空间字节大小
 *@retval 分配空间,失败为NULL
 */
void * app_mem_realloc(void *pointer, uint32_t size)
{
    return realloc(pointer, size);
}

/*@brief 内存释放
 *@param pointer 分配空间
 */
void app_mem_free(void* pointer)
{
    free(pointer);
}

/*@brief 毫秒延时
 */
void app_delay_ms(uint32_t ms)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    Sleep(ms);
}

/*@brief 微秒延时
 */
void app_delay_us(uint32_t us)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    LARGE_INTEGER dw_freq;
    if (!QueryPerformanceFrequency(&dw_freq))
        return;
    LONGLONG counter = dw_freq.QuadPart * us / 1000 / 1000;
    
    LARGE_INTEGER dw_s;
    LARGE_INTEGER dw_c;
    for (QueryPerformanceCounter(&dw_s),
         QueryPerformanceCounter(&dw_c);
         dw_c.QuadPart - dw_s.QuadPart < counter;
         QueryPerformanceCounter(&dw_c));
}

/*@brief 计算一个延时点(us)
 */
uint64_t app_execute_us(void)
{
    LARGE_INTEGER freq;
    LARGE_INTEGER tick;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&tick);
    return (double)tick.QuadPart * 1000 * 1000 / freq.QuadPart;
}

#endif
