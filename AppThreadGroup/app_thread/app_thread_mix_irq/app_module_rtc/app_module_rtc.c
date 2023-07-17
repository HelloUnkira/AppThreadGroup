/*实现目标:
 *    RTC模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_work.h"
#include "app_thread_group.h"
#include "app_module_rtc.h"
#include "app_module_system.h"

static app_mutex_t app_module_rtc_mutex = {0};
static app_module_rtc_t app_module_rtc = {0};

/*@brief RTC模组1ms事件中断回调
 */
static void app_module_rtc_cb(void)
{
    #if APP_ARCH_IS_PC
    #else
    static uint32_t count = 0;
    app_mutex_process(&app_module_rtc_mutex, app_mutex_take);
    app_module_system_1msec_update(count++);
    app_mutex_process(&app_module_rtc_mutex, app_mutex_give);
    #endif
}

/*@brief 设置RTC模组
 */
void app_module_rtc_set(app_module_rtc_t *rtc)
{
    app_mutex_process(&app_module_rtc_mutex, app_mutex_take);
    app_module_rtc = *rtc;
    app_dev_rtc_set_utc(&app_dev_rtc, &app_module_rtc.utc);
    app_mutex_process(&app_module_rtc_mutex, app_mutex_give);
}

/*@brief 获取RTC模组
 */
void app_module_rtc_get(app_module_rtc_t *rtc)
{
    app_mutex_process(&app_module_rtc_mutex, app_mutex_take);
    app_dev_rtc_get_utc(&app_dev_rtc, &app_module_rtc.utc);
    *rtc = app_module_rtc;
    app_mutex_process(&app_module_rtc_mutex, app_mutex_give);
}

/*@brief 初始化RTC模组
 */
void app_module_rtc_ready(void)
{
    app_mutex_process(&app_module_rtc_mutex, app_mutex_static);
    app_dev_rtc_ready(&app_dev_rtc);
    app_dev_rtc_irq_cb_reg(&app_dev_rtc, app_module_rtc_cb);
    app_dev_rtc_execute(&app_dev_rtc);
}
