/*实现目标:
 *    软件定时器测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"

/*@brief 软件定时器模组测试回调函数
 */
static void app_sys_timer_test_callback(void *timer)
{
    app_sys_timer_t *sys_timer = timer;
    uint32_t *data = sys_timer->user_data;
    APP_SYS_LOG_INFO("timer %d callback %d", data[0], data[1]++);
    if (data[1] == 3)
        app_sys_timer_stop(sys_timer);
}

/*@brief 软件定时器模组测试
 */
void app_sys_timer_test(void)
{
    static uint32_t label_1[2] = {1, 0};
    static uint32_t label_2[2] = {2, 0};
    static uint32_t label_3[2] = {3, 0};
    
    static app_sys_timer_t timer1 = {
        .expired   = app_sys_timer_test_callback,
        .user_data = &label_1,
        .peroid    = 1000,
        .reload    = true};
    static app_sys_timer_t timer2 = {
        .expired   = app_sys_timer_test_callback,
        .user_data = &label_2,
        .peroid    = 2000,
        .reload    = true};
    static app_sys_timer_t timer3 = {
        .expired   = app_sys_timer_test_callback,
        .user_data = &label_3,
        .peroid    = 3000,
        .reload    = true};
    
    app_sys_timer_start(&timer1);
    app_sys_timer_start(&timer2);
    app_sys_timer_start(&timer3);
}
