/*实现目标:
 *    主从线程组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_sys_ext_mem.h"
#include "app_sys_trace_text.h"
#include "app_thread_adaptor.h"
#include "app_thread_group.h"
#include "app_module_system.h"
#include "app_module_watchdog.h"

static bool app_thread_group_status = false;

/*@brief 线程组运行
 *       准备并启动所有线程及其附属资源
 */
void app_thread_group_schedule(void)
{
    /* 就绪系统子模组 */
    app_sys_log_t log = {
        .message1   = app_ext_arch_log_msg1,
        .message2   = app_ext_arch_log_msg2,
        .persistent = app_sys_trace_log_persistent,
    };
    app_sys_log_ready(log);
    app_sys_timer_ready();
    app_sys_ext_mem_ready();
    app_sys_trace_text_ready();
    app_sys_build_time();
    /* 就绪线程公共子模组 */
    app_module_system_ready();
    app_module_watchdog_ready();
    /* 就绪线程子模组 */
    app_thread_master_ready();
    app_thread_mix_irq_ready();
    app_thread_mix_custom_ready();
    app_thread_manage_ready();
    app_thread_lvgl_ready();
    app_thread_jerryscript_ready();
    /* 就绪和启用线程组 */
    app_thread_process(&app_thread_master,          app_thread_static);
    app_thread_process(&app_thread_mix_irq,         app_thread_static);
    app_thread_process(&app_thread_mix_custom,      app_thread_static);
    app_thread_process(&app_thread_manage,          app_thread_static);
    app_thread_process(&app_thread_lvgl,            app_thread_static);
    app_thread_process(&app_thread_jerryscript,     app_thread_static);
    /* 设置线程组就绪 */
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    app_thread_group_status = true;
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
}

/*@brief 获得线程组初始化状态
 */
bool app_thread_group_status_get(void)
{
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    bool status = app_thread_group_status;
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
    return status;
}
