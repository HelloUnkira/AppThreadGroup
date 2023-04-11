/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_std_lib.h"
#include "app_os_adaptor.h"

#if APP_OS_IS_FREERTOS

/*@brief        准备与执行线程
 *              创建一个线程并启动(线程创建时立即启动)
 *@param[in]    thread 静态实例
 */
void app_thread_process(app_thread_t *thread)
{
    xTaskCreate(thread->task,
                thread->name,
                thread->stack_size,
                NULL,
                thread->priority,
               &thread->handle);
}

/*@brief 当前环境是否为中断环境(注意:当且仅当必要的使用)
 */
bool app_os_not_in_irq(void)
{
#if 0
    uint32_t interrupt;
    
    /* 获取当前正在执行的中断数 */
    __asm volatile("mrs %0, ipsr" : "=r" (interrupt)::"memory");
    
    if(interrupt == 0)
        return true;
    else
        return false;
#else
    if (pdTRUE == xPortIsInsideInterrupt())
        return false;
    else
        return true;
#endif
}

/*@brief        创建一个信号量并准备好使用
 *@param[in]    sem 静态实例
 */
void app_sem_process(app_sem_t *sem)
{
    sem->sem = xSemaphoreCreateCounting(100, 0);
}

/*@brief        获取一个信号量
 *@param[in]    sem 静态实例
 */
void app_sem_take(app_sem_t *sem)
{
    if (app_os_not_in_irq())
        xSemaphoreTake(sem->sem, portMAX_DELAY);
    else
        xSemaphoreTakeFromISR(sem->sem, portMAX_DELAY);
}

/*@brief        发布一个信号量
 *@param[in]    sem 静态实例
 */
void app_sem_give(app_sem_t *sem)
{
    if (app_os_not_in_irq())
        xSemaphoreGive(sem->sem);
    else
        xSemaphoreGiveFromISR(sem->sem);
}

/*@brief        创建一个互斥锁并准备好使用
 *@param[in]    mutex 静态实例
 */
void app_mutex_process(app_mutex_t *mutex)
{
    mutex->mutex = xSemaphoreCreateMutex();
}

/*@brief        抢占一个互斥锁(中断环境不可调用)
 *@param[in]    mutex 静态实例
 */
void app_mutex_take(app_mutex_t *mutex)
{
    if (app_os_not_in_irq())
        xSemaphoreTake(mutex->mutex, portMAX_DELAY);
}

/*@brief        释放一个互斥锁(中断环境不可调用)
 *@param[in]    mutex 静态实例
 */
void app_mutex_give(app_mutex_t *mutex)
{
    if (app_os_not_in_irq())
        xSemaphoreGive(mutex->mutex);
}

/*@brief        内存分配
 *@param[in]    size 分配空间字节大小
 *@retval       分配空间,失败为NULL
 */
void * app_mem_alloc(uint32_t size)
{
    return pvPortMalloc(size);
}

/*@brief        内存分配
 *@param[in]    pointer 分配空间回收
 *@param[in]    size    分配空间字节大小
 *@retval       分配空间,失败为NULL
 */
void * app_mem_realloc(void *pointer, uint32_t size)
{
    /* 等待适配 */
    return NULL;
}

/*@brief        内存释放
 *@param[in]    pointer 分配空间
 */
void app_mem_free(void *pointer)
{
    vPortFree(pointer);
}

/*@brief 临界区保护(注意:当且仅当必要的使用)
 */
void app_critical_enter(void)
{
    taskDISABLE_INTERRUPTS();
}

/*@brief 临界区退出(注意:当且仅当必要的使用)
 */
void app_critical_exit(void)
{
    taskENABLE_INTERRUPTS();
}

/*@brief 毫秒延时
 */
void app_delay_ms(uint32_t ms)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    vTaskDelay(ms);
}

/*@brief 微秒延时
 */
void app_delay_us(uint32_t us)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    //根据平台提供;
}

/*@brief 重启
 */
void app_os_reset(void)
{
}

#endif
