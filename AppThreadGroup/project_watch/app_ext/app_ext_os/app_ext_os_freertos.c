/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_OS_IS_FREERTOS

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

/*@brief 线程操作流程集合
 *@param thread 实例
 *@param option 实例动作
 */
void app_thread_process(app_thread_t *thread, app_thread_option_t option)
{
    switch (option) {
    case app_thread_static: {
        xTaskCreate(thread->task,
                    thread->name,
                    thread->stack_size,
                    NULL,
                    thread->priority,
                   &thread->handle);
        break;
    }
    default:
        APP_DEV_LOG_MSG_FMT("app_thread_process option is not unsupported:%u", option);
        app_os_reset();
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
    case app_sem_static: {
        sem->sem = xSemaphoreCreateCounting(100, 0);
        break;
    }
    case app_sem_take: {
        if (app_os_not_in_irq())
            xSemaphoreTake(sem->sem, portMAX_DELAY);
        else
            xSemaphoreTakeFromISR(sem->sem, portMAX_DELAY);
        break;
    }
    case app_sem_give: {
        if (app_os_not_in_irq())
            xSemaphoreGive(sem->sem);
        else
            xSemaphoreGiveFromISR(sem->sem);
        break;
    }
    default:
        APP_DEV_LOG_MSG_FMT("app_sem_process option is not unsupported:%u", option);
        app_os_reset();
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
    case app_mutex_static: {
        mutex->mutex = xSemaphoreCreateMutex();
        break;
    }
    case app_mutex_take: {
        if (app_os_not_in_irq())
            xSemaphoreTake(mutex->mutex, portMAX_DELAY);
        break;
    }
    case app_mutex_give: {
        if (app_os_not_in_irq())
            xSemaphoreGive(mutex->mutex);
        break;
    }
    default:
        APP_DEV_LOG_MSG_FMT("app_mutex_process option is not unsupported:%u", option);
        app_os_reset();
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
    case app_critical_enter: {
        taskDISABLE_INTERRUPTS();
        break;
    }
    case app_critical_exit: {
        taskENABLE_INTERRUPTS();
        break;
    }
    default:
        APP_DEV_LOG_MSG_FMT("app_critical_process option is not unsupported:%u", option);
        app_os_reset();
        break;
    }
}

/*@brief 内存分配
 *@param size 分配空间字节大小
 *@retval 分配空间,失败为NULL
 */
void * app_mem_alloc(uint32_t size)
{
    return pvPortMalloc(size);
}

/*@brief 内存分配
 *@param pointer 分配空间回收
 *@param size    分配空间字节大小
 *@retval 分配空间,失败为NULL
 */
void * app_mem_realloc(void *pointer, uint32_t size)
{
    /* 等待适配 */
    return NULL;
}

/*@brief 内存释放
 *@param pointer 分配空间
 */
void app_mem_free(void *pointer)
{
    vPortFree(pointer);
}

/*@brief 毫秒延时
 */
void app_delay_ms(uint32_t ms)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/*@brief 微秒延时
 */
void app_delay_us(uint32_t us)
{
    /* 延时可能会涉及到调度 */
    /* 取决于操作系统单次轮转的时间片大小及RTC精度 */
    //根据平台提供;
}

/*@brief 计算一个延时点(us)
 */
uint64_t app_execute_us(void)
{
    TickType_t tick = xTaskGetTickCount();
    return (double)tick * 1000 * 1000 / configTICK_RATE_HZ;
}

#endif
