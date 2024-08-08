/*实现目标:
 *    主从线程组
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"

static bool app_thread_group_status = false;

/*@brief 子线程准备
 */
void app_thread_slave_prepare(void)
{
    /* 系统软件模组初始化 */
    app_module_data_center_ready();
    /*
     *
     */
    /* 就绪线程公共子模组 */
    app_module_system_ready();
}

/*@brief 子线程调度
 */
void app_thread_slave_schedule(void)
{
    app_thread_process(&app_thread_mix_irq,         app_thread_static);
    app_thread_process(&app_thread_mix_custom,      app_thread_static);
    app_thread_process(&app_thread_manage,          app_thread_static);
    #if 0
    #elif APP_EXT_DEV_GUI_IS_LVGL
    app_thread_process(&app_thread_lvgl,            app_thread_static);
    #elif APP_EXT_DEV_GUI_IS_SCUI
    app_thread_process(&app_thread_scui,            app_thread_static);
    #else
    #error "gui framework is unsupport"
    #endif
}

/*@brief 线程组运行
 *       准备并启动所有线程及其附属资源
 */
void app_thread_group_schedule(void)
{
    /*
     *!!!就绪app ext(arch)层
     */
    app_arch_ready();
    /*
     *!!!就绪app ext(os)层
     */
    app_os_ready();
    /*
     *!!!就绪app ext(dev)层
     */
    app_dev_1_ready();
    /*
     *!!!就绪app sys层
     */
    typedef void (*message1)(const char *format, ...);
    typedef void (*message2)(const char *format, va_list list);
    typedef void (*persistent)(const char *text);
    typedef void (*notify_assert)(void);
    
    app_sys_cfg_t sys_cfg = {
        .log = {
            .message1       = (message1)        app_dev_log_msg1,
            .message2       = (message2)        app_dev_log_msg2,
            .persistent     = (persistent)      app_sys_log_text_persistent,
            .notify_assert  = (notify_assert)   app_thread_assert_notify,
        },
    };
    app_sys_ready(&sys_cfg);
    /*
     *!!!就绪app dev层
     */
    app_dev_2_ready();
    /*
     *!!!就绪app thread层
     */
    /* 就绪主从线程组其附属模组 */
    app_thread_master_prepare();
    app_thread_slave_prepare();
    /* 就绪和启用线程组 */
    app_thread_master_schedule();
    app_thread_slave_schedule();
    /* 设置线程组就绪 */
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    app_thread_group_status = true;
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
    /*
     * 调度
     */
    APP_SYS_LOG_INFO("");
    app_os_schedule();
}

/*@brief 获得线程组初始化状态
 */
bool app_thread_group_run_status(void)
{
    app_critical_t critical = {0};
    app_critical_process(&critical, app_critical_create);
    app_critical_process(&critical, app_critical_enter);
    bool status = app_thread_group_status;
    app_critical_process(&critical, app_critical_exit);
    app_critical_process(&critical, app_critical_destroy);
    return status;
}
