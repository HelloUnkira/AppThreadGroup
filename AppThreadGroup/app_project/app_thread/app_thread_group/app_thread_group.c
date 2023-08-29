/*实现目标:
 *    主从线程组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static bool app_thread_group_status = false;

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
    app_dev_ready();
    /*
     *!!!就绪app sys层
     */
    app_sys_ready();
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
    APP_SYS_LOG_INFO("app thread group schedule");
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
