/*实现目标:
 *    scui线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "scui_lib.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_scui_routine_ready_cb(void)
{
    scui_engine_ready();
    scui_engine_execute_status_set(true);
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_scui_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_scui_sched: {
        /// if (package->event == xxx) {
        /// }
        return true;
    }
    case app_thread_scui_ui: {
        /// if (package->event == xxx) {
        /// }
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief scui线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_scui_routine)
{
    app_thread_slave_process(app_thread_id_scui,
                             app_thread_scui_routine_ready_cb,
                             app_thread_scui_routine_package_cb);
}
