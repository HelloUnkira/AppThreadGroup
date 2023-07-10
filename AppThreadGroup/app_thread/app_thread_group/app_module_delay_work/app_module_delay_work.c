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

/*@brief 迟延工作定时器回调
 */
static void app_module_delay_work_timer_handler(void *timer)
{
    app_sys_timer_t *delay_work_timer = timer;
    app_module_delay_work_t *delay_work  = delay_work_timer->user_data;
    app_thread_package_t package = {
        .thread  = delay_work->thread_id,
        .module  = 0,                       /* 线程组系统模组 */
        .event   = app_thread_event_work,   /* 线程组工作事件 */
        .dynamic = true,
        .size    = sizeof(app_sys_work_t),
        .data    = app_sys_work_make(delay_work->work.routine, delay_work->work.argument),
    };
    app_thread_package_notify(&package);
}

/*@brief     迟延工作提交
 *@param[in] delay_work 一份延迟工作
 *@param[in] delay_ms   延迟时间
 */
void app_module_delay_work_submit(app_module_delay_work_t *delay_work, uint32_t delay_ms)
{
    app_sys_timer_stop(&delay_work->timer);
    delay_work->timer.user_data = delay_work;
    delay_work->timer.expired   = app_module_delay_work_timer_handler;
    delay_work->timer.peroid    = delay_ms;
    app_sys_timer_start(&delay_work->timer);
}

