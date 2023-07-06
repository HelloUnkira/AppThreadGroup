/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_OS_IS_WINDOWS

/*@brief 当前环境是否为中断环境(注意:当且仅当必要的使用)
 */
bool app_os_not_in_irq(void)
{
    /* 模拟器环境不存在硬件中断环境 */
    return true;
}

/*@brief        线程操作流程集合
*@param[in]    thread 实例
*@param[in]    option 实例动作
*/
 void app_thread_process(app_thread_t *thread, app_thread_option_t option)
{
    switch (option) {
    case app_thread_create: {
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
        app_ext_arch_log_msg1("app_thread_process option is not unsupported:%u", option);
        app_os_reset();
        break;
    }
}

/*@brief        信号量操作流程集合
 *@param[in]    sem    实例
 *@param[in]    option 实例动作
 */
void app_sem_process(app_sem_t *sem, app_sem_option_t option)
{
    switch (option) {
    case app_sem_create: {
        sem->sem = CreateSemaphore(NULL, 0, 100, NULL);
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
        app_ext_arch_log_msg1("app_sem_process option is not unsupported:%u", option);
        app_os_reset();
        break;
    }
}

/*@brief        创建一个互斥锁并准备好使用
 *@param[in]    mutex 静态实例
 */
void app_mutex_process(app_mutex_t *mutex)
{
    mutex->mutex = CreateMutex(NULL, FALSE, NULL);
}

/*@brief        抢占一个互斥锁(中断环境不可调用)
 *@param[in]    mutex 静态实例
 */
void app_mutex_take(app_mutex_t *mutex)
{
    if (app_os_not_in_irq())
        WaitForSingleObject(mutex->mutex, INFINITE);
}

/*@brief        释放一个互斥锁(中断环境不可调用)
 *@param[in]    mutex 静态实例
 */
void app_mutex_give(app_mutex_t *mutex)
{
    if (app_os_not_in_irq())
        ReleaseMutex(mutex->mutex);
}

/*@brief        创建一个临界区并准备好使用
 *@param[in]    critical 临界区实例
 */
void app_critical_process(app_critical_t *critical)
{
    /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
    app_mutex_process(&critical->mutex);
    critical->mutex_ready = true;
}

/*@brief 临界区保护(注意:当且仅当必要的使用)
 */
void app_critical_enter(app_critical_t *critical)
{
    /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
    if (critical->mutex_ready)
        app_mutex_take(&critical->mutex);
}

/*@brief 临界区退出(注意:当且仅当必要的使用)
 */
void app_critical_exit(app_critical_t *critical)
{
    /* Windows不需要临界区保护,因为资源不会被中断打断,临界区退化为互斥锁 */
    if (critical->mutex_ready)
        app_mutex_give(&critical->mutex);
}

/*@brief        内存分配
 *@param[in]    size 分配空间字节大小
 *@retval       分配空间,失败为NULL
 */
void * app_mem_alloc(uint32_t size)
{
    return malloc(size);
}

/*@brief        内存分配
 *@param[in]    pointer 分配空间回收
 *@param[in]    size    分配空间字节大小
 *@retval       分配空间,失败为NULL
 */
void * app_mem_realloc(void *pointer, uint32_t size)
{
    return realloc(pointer, size);
}

/*@brief        内存释放
 *@param[in]    pointer 分配空间
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

/*@brief 计算一段代码的延时时间(us)
 */
double app_execute_us(app_execute_us_t *execute_us, bool run)
{
    if (run) {
        QueryPerformanceFrequency(&execute_us->frequency);
        QueryPerformanceCounter(&execute_us->start);
        return 0;
    } else {
        QueryPerformanceCounter(&execute_us->end);
        return (double)(execute_us->end.QuadPart - execute_us->start.QuadPart) * 1000 * 1000 / execute_us->frequency.QuadPart;
    }
}

/*@brief 重启
 */
void app_os_reset(void)
{
    exit(-1);
}

#endif
