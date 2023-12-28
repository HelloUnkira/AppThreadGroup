/*实现目标:
 *    简要的毫秒级系统滴答适配模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static app_sys_timer_t scui_tick_timer = {0};

/*@brief 软件定时器回调
 */
static void scui_tick_timer_handler(void *timer)
{
    /* 更新动画即可,动画更新了自己会产生事件调度 */
    if (scui_engine_execute_status_get())
        scui_anima_elapse_update(1);
}

/*@brief 毫秒级系统滴答初始化
 */
void scui_tick_ready(void)
{
    scui_tick_timer.expired = scui_tick_timer_handler;
    scui_tick_timer.peroid  = 1;
    scui_tick_timer.reload  = true;
    app_sys_timer_start(&scui_tick_timer);
}
