/*实现目标:
 *    RTC模组
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"

/*@brief 设置RTC模组
 */
void app_module_rtc_set(app_module_rtc_t *rtc)
{
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    app_dev_rtc_set_utc(&app_dev_rtc, &rtc->utc, &rtc->utc_ms);
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
}

/*@brief 获取RTC模组
 */
void app_module_rtc_get(app_module_rtc_t *rtc)
{
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    app_dev_rtc_get_utc(&app_dev_rtc, &rtc->utc, &rtc->utc_ms);
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
}

/*@brief RTC模组1ms事件中断回调
 */
void app_module_rtc_1ms_cb(void)
{
    static uint32_t count = 0; count++;
    /* 线程组不在工作中,Tick是没有意义的 */
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    if (app_thread_group_run_status())
        app_module_timer_1ms_update();
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
    /* 心跳是生命之源 */
    if (count % 1000 == 0)
        APP_SYS_LOG_INFO("1s handler");
}

/*@brief 同步RTC模组
 */
void app_module_rtc_sync(void)
{
    app_dev_rtc_sync(&app_dev_rtc);
}

/*@brief 初始化RTC模组
 */
void app_module_rtc_ready(void)
{
    app_dev_rtc_ready(&app_dev_rtc);
    app_dev_rtc_irq_cb_reg(&app_dev_rtc, app_module_rtc_1ms_cb);
    app_dev_rtc_irq_switch(&app_dev_rtc, true);
}
