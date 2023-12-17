/*实现目标:
 *    lvgl软件定时器的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

static app_sys_timer_t app_lv_timer = {0};

/*@brief lvgl软件定时器启动
 */
void app_lv_timer_start(void)
{
    app_sys_timer_start(&app_lv_timer);
}

/*@brief lvgl软件定时器停止
 */
void app_lv_timer_stop(void)
{
    app_sys_timer_stop(&app_lv_timer);
}

/*@brief lvgl软件定时器回调
 */
static void app_lv_timer_handler(void *timer)
{
    static uint8_t count = 0;
    count++;
    
    /* lvgl时钟约减事件(跨线程就地更新) */
    if (count % LV_SCHED_TICK_INC == 0)
        lv_tick_inc(LV_SCHED_TICK_INC);
    /* lvgl时钟调度事件 */
    if (count % LV_SCHED_TICK_EXEC == 0)
        app_lv_tick_exec_update();
    /* lvgl驱动检查事件 */
    if (count % LV_SCHED_DRV_EVENT == 0)
        app_lv_drv_update();
}

/*@brief lvgl软件定时器初始化
 */
void app_lv_timer_ready(void)
{
    app_lv_timer.expired = app_lv_timer_handler;
    app_lv_timer.peroid  = 1;
    app_lv_timer.reload  = true;
}

