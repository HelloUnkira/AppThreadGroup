/*实现目标:
 *    工作迟延处理
 */
    
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_sys_work.h"
#include "app_thread_group.h"
#include "app_module_delay_work.h"

/*@brief 迟延工作或工作清单资源回收回调
 */
static void app_module_delay_work_recycle_handler(void *argument)
{
    APP_SYS_ASSERT(argument != NULL);
    app_mem_free(argument);
}

/*@brief 迟延工作或工作清单定时器回调
 */
static void app_module_delay_work_timer_handler(void *timer)
{
    app_sys_timer_t *delay_timer = timer;
    app_module_delay_work_t *delay_work = delay_timer->user_data;
    /* 提交工作到指定线程 */ {
        app_thread_package_t package = {
            .thread  = delay_work->thread,
            .module  = 0,                       /* 线程组系统模组 */
            .event   = app_thread_event_work,   /* 线程组工作事件 */
            .data    = delay_work->work,
        };
        app_thread_package_notify(&package);
    }
    /* 额外提交资源回收工作到指定线程 */ {
        app_thread_package_t package = {
            .thread  = delay_work->thread,
            .module  = 0,                       /* 线程组系统模组 */
            .event   = app_thread_event_work,   /* 线程组工作事件 */
            .data    = app_sys_work_make(1, app_module_delay_work_recycle_handler, delay_work),
        };
        app_thread_package_notify(&package);
    }
}

/*@brief     迟延工作提交
 *@param[in] work       一份工作(生成)
 *@param[in] delay_ms   延迟时间
 *@param[in] thread     目标线程
 */
void app_module_delay_work_submit(app_sys_work_t *work, uint32_t delay_ms, uint32_t thread)
{
    app_module_delay_work_t *delay_work = app_mem_alloc(sizeof(app_module_delay_work_t));
    memset(&delay_work->timer, 0, sizeof(app_sys_timer_t));
    delay_work->work            = work;
    delay_work->timer.user_data = delay_work;
    delay_work->timer.expired   = app_module_delay_work_timer_handler;
    delay_work->timer.peroid    = delay_ms;
    delay_work->thread          = thread;
    app_sys_timer_start(&delay_work->timer);
}
