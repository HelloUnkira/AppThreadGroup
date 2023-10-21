/*实现目标:
 *    工作处理
 *    工作迟延处理
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 处理一份工作清单
 *@param[in] work 工作实例
 */
void app_module_work_execute(app_module_work_t *work)
{
    APP_SYS_ASSERT(work != NULL);
    for (uint32_t idx = 0; idx < work->number; idx++)
        work->array[idx].routine(work->array[idx].argument);
    app_mem_free(work);
}

/*@brief 生成一份工作清单
 *@param[in] number    工作数量
 *@param[in] argv[0,1] 工作回调1, 工作回调1参数
 *@param[in] argv[2,3] 工作回调2, 工作回调2参数...依此类推
 */
app_module_work_t * app_module_work_make(uint32_t number, ...)
{
    va_list  list;
    va_start(list, number);
    
    uint32_t work_zone = sizeof(app_module_work_t) + sizeof(uint32_t) + sizeof(void *) * 2 * number;
    app_module_work_t * work = app_mem_alloc(work_zone);
    for (uint32_t idx = 0; idx < (work->number = number); idx++) {
        work->array[idx].routine  = (void (*)(void *))  (va_arg(list, void *));
        work->array[idx].argument = (void *)            (va_arg(list, void *));
    }
    
    va_end(list);
    return work;
}

/*@brief 迟延工作或工作清单资源回收回调
 */
static void app_module_work_delay_recycle_handler(void *argument)
{
    APP_SYS_ASSERT(argument != NULL);
    app_mem_free(argument);
}

/*@brief 迟延工作或工作清单定时器回调
 */
static void app_module_work_delay_timer_handler(void *timer)
{
    app_module_work_delay_t *work_delay = ((app_sys_timer_t *)timer)->user_data;
    /* 提交工作到指定线程 */ {
        app_thread_package_t package = {
            .thread  = work_delay->thread,
            .module  = 0,                       /* 线程组系统模组 */
            .event   = app_thread_event_work,   /* 线程组工作事件 */
            .data    = work_delay->work,
        };
        app_thread_package_notify(&package);
    }
    /* 额外提交资源回收工作到指定线程 */ {
        app_thread_package_t package = {
            .thread  = work_delay->thread,
            .module  = 0,                       /* 线程组系统模组 */
            .event   = app_thread_event_work,   /* 线程组工作事件 */
            .data    = app_module_work_make(1, app_module_work_delay_recycle_handler, work_delay),
        };
        app_thread_package_notify(&package);
    }
}

/*@brief     迟延工作提交
 *@param[in] work       一份工作(生成)
 *@param[in] delay_ms   延迟时间
 *@param[in] thread     目标线程
 */
void app_module_work_delay_submit(app_module_work_t *work, uint32_t delay_ms, uint32_t thread)
{
    app_module_work_delay_t *work_delay = app_mem_alloc(sizeof(app_module_work_delay_t));
    memset(&work_delay->timer, 0, sizeof(app_sys_timer_t));
    work_delay->work            = (void *)work;
    work_delay->timer.user_data = (void *)work_delay;
    work_delay->timer.expired   = app_module_work_delay_timer_handler;
    work_delay->timer.peroid    = delay_ms;
    work_delay->thread          = thread;
    app_sys_timer_start(&work_delay->timer);
}
