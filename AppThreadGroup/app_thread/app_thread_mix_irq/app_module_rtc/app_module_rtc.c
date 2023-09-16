/*实现目标:
 *    RTC模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static app_critical_t app_module_rtc_critical = {0};
static app_module_rtc_t app_module_rtc = {0};

/*@brief RTC模组1ms事件中断回调
 */
void app_module_rtc_1ms_cb(void)
{
    /* 心跳是生命之源 */
    static uint32_t count = 0;
    app_critical_process(&app_module_rtc_critical, app_critical_enter);
    if (count++ % 1000 == 0)
        APP_SYS_LOG_INFO('1s handler');
    /* 线程组不在工作中,Tick是没有意义的 */
    if (app_thread_group_run_status()) {
        /* timer msec update */
        app_module_timer_1ms_update();
        /* 一些补充的扩展配置,与OS相关 */
        if (count == 3000)
            app_thread_group_extend();
    }
    app_critical_process(&app_module_rtc_critical, app_critical_exit);
}

/*@brief 设置RTC模组
 */
void app_module_rtc_set(app_module_rtc_t *rtc)
{
    app_critical_process(&app_module_rtc_critical, app_critical_enter);
    app_module_rtc = *rtc;
    app_dev_rtc_set_utc(&app_dev_rtc, &app_module_rtc.utc);
    app_critical_process(&app_module_rtc_critical, app_critical_exit);
}

/*@brief 获取RTC模组
 */
void app_module_rtc_get(app_module_rtc_t *rtc)
{
    app_critical_process(&app_module_rtc_critical, app_critical_enter);
    app_dev_rtc_get_utc(&app_dev_rtc, &app_module_rtc.utc);
    *rtc = app_module_rtc;
    app_critical_process(&app_module_rtc_critical, app_critical_exit);
}

/*@brief 初始化RTC模组
 */
void app_module_rtc_ready(void)
{
    app_critical_process(&app_module_rtc_critical, app_critical_static);
    app_dev_rtc_irq_cb_reg(&app_dev_rtc, app_module_rtc_1ms_cb);
    app_dev_rtc_ready(&app_dev_rtc);
    app_dev_rtc_execute(&app_dev_rtc);
}
