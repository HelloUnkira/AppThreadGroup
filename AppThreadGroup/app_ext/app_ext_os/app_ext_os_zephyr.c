/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_OS_IS_ZEPHYR

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

/*@brief        线程操作流程集合
 *@param[in]    thread 实例
 *@param[in]    option 实例动作
 */
void app_thread_process(app_thread_t *thread, app_thread_option_t option)
{
    switch (option) {
    case app_thread_static: {
        k_thread_create(&thread->thread,
                         thread->stack,
                         thread->stack_size,
                         thread->entry,
                         NULL, NULL, NULL,
                         thread->prio, 0,
                         K_NO_WAIT);
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
    case app_sem_static: {
        k_sem_init(&sem->sem, 0, 100);
        break;
    }
    case app_sem_take: {
        k_sem_take(&sem->sem, K_FOREVER);
        break;
    }
    case app_sem_give: {
        k_sem_give(&sem->sem);
        break;
    }
    default:
        app_ext_arch_log_msg1("app_sem_process option is not unsupported:%u", option);
        app_os_reset();
        break;
    }
}

/*@brief        互斥锁操作流程集合
 *@param[in]    mutex  实例
 *@param[in]    option 实例动作
 */
void app_mutex_process(app_mutex_t *mutex, app_mutex_option_t option)
{
    switch (option) {
    case app_mutex_static: {
        k_mutex_init(&mutex->mutex);
        break;
    }
    case app_mutex_take: {
        if (app_os_not_in_irq())
            k_mutex_lock(&mutex->mutex, K_FOREVER);
        break;
    }
    case app_mutex_give: {
        if (app_os_not_in_irq())
            k_mutex_unlock(&mutex->mutex);
        break;
    }
    default:
        app_ext_arch_log_msg1("app_mutex_process option is not unsupported:%u", option);
        app_os_reset();
        break;
    }
}

/*@brief        临界区操作流程集合
 *@param[in]    critical 实例
 *@param[in]    option   实例动作
 */
void app_critical_process(app_critical_t *critical, app_critical_option_t option)
{
    switch (option) {
    case app_critical_enter: {
        __disable_irq();
        break;
    }
    case app_critical_exit: {
        __enable_irq();
        break;
    }
    default:
        app_ext_arch_log_msg1("app_critical_process option is not unsupported:%u", option);
        app_os_reset();
        break;
    }
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

/*@brief 计算一段代码的延时时间(us)
 */
double app_execute_us(app_execute_us_t *execute_us, bool run)
{
    if (run) {
        execute_us->start = k_cycle_get_32();
        return 0;
    } else {
        execute_us->end = k_cycle_get_32();
        return (double)(execute_us->end - execute_us->start) * 1000 * 1000 / sys_clock_hw_cycles_per_sec;
    }
}

/*@brief 重启
 */
void app_os_reset(void)
{
}

#endif
