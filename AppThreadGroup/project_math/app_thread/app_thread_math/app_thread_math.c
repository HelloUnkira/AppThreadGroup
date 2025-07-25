/*实现目标:
 *    math线程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_math_routine_ready_cb(void)
{
    app_thread_package_t package = {
        .thread = app_thread_id_math,
        .module = app_thread_math_test,
    };
    app_thread_package_notify(&package);
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_math_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_math_test:
        AppMathTest();
        return true;
    default:
        break;
    }
    return false;
}

/*@brief 混合中断线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_math_routine)
{
    app_thread_slave_process(app_thread_id_math,
                             app_thread_math_routine_ready_cb,
                             app_thread_math_routine_package_cb);
}
