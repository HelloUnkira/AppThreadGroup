/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_OS_IS_ZEPHYR

/*@brief        准备与执行线程
 *              创建一个线程并启动(线程创建时立即启动)
 *@param[in]    thread 静态实例
 */
void app_thread_process(app_thread_t *thread)
{
    k_thread_create(&thread->thread,
                     thread->stack,
                     thread->stack_size,
                     thread->entry,
                     NULL, NULL, NULL,
                     thread->prio, 0,
                     K_NO_WAIT);
}

/*@brief 当前环境是否为中断环境(注意:当且仅当必要的使用)
 */
bool app_os_not_in_irq(void)
{        
    uint32_t interrupt;
    
    /* 获取当前正在执行的中断数 */
    __asm volatile("mrs %0, ipsr" : "=r" (interrupt)::"memory");
    
    if(interrupt == 0)
        return true;
    else
        return false;
}

/*@brief        创建一个信号量并准备好使用
 *@param[in]    sem 静态实例
 */
void app_sem_process(app_sem_t *sem)
{
    k_sem_init(&sem->sem, 0, 100);
}

/*@brief        获取一个信号量
 *@param[in]    sem 静态实例
 */
void app_sem_take(app_sem_t *sem)
{
    k_sem_take(&sem->sem, K_FOREVER);
}

/*@brief        发布一个信号量
 *@param[in]    sem 静态实例
 */
void app_sem_give(app_sem_t *sem)
{
    k_sem_give(&sem->sem);
}

/*@brief        创建一个互斥锁并准备好使用
 *@param[in]    mutex 静态实例
 */
void app_mutex_process(app_mutex_t *mutex)
{
    k_mutex_init(&mutex->mutex);
}

/*@brief        抢占一个互斥锁(中断环境不可调用)
 *@param[in]    mutex 静态实例
 */
void app_mutex_take(app_mutex_t *mutex)
{
    if (app_os_not_in_irq())
        k_mutex_lock(&mutex->mutex, K_FOREVER);
}

/*@brief        释放一个互斥锁(中断环境不可调用)
 *@param[in]    mutex 静态实例
 */
void app_mutex_give(app_mutex_t *mutex)
{
    if (app_os_not_in_irq())
        k_mutex_unlock(&mutex->mutex);
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
void app_mem_free(void *pointer)
{
    free(pointer);
}

/*@brief 临界区保护(注意:当且仅当必要的使用)
 */
void app_critical_enter(void)
{
    __disable_irq();
}

/*@brief 临界区退出(注意:当且仅当必要的使用)
 */
void app_critical_exit(void)
{
    __enable_irq();
}

/*@brief 毫秒延时
 */
void app_delay_ms(uint32_t ms)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    k_sleep(K_MSEC(ms));
}

/*@brief 微秒延时
 */
void app_delay_us(uint32_t us)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    k_busy_wait(us);
}

/*@brief 计算一段代码的延时时间(ms)
 */
uint32_t app_execute_ms(app_execute_ms_t *execute_ms, bool run)
{
    if (run) {
        execute_ms->start = k_cycle_get_32();
        return 0;
    } else {
        execute_ms->end = k_cycle_get_32();
        return (double)(execute_ms->end - execute_ms->start) * 1000 / sys_clock_hw_cycles_per_sec;
    }
}

/*@brief 重启
 */
void app_os_reset(void)
{
}

#endif
