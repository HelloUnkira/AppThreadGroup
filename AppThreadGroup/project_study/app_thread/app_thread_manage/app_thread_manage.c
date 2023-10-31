/*实现目标:
 *    数据中心管理线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_manage_routine_ready_cb(void)
{
    /* 软件模组初始化 */
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_manage_routine_package_cb(app_thread_package_t *package, uint32_t *discard_count)
{
    switch (package->module) {
    default:
        break;
    }
    return false;
}

/*@brief 数据管理线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_manage_routine)
{
    app_thread_slave_process(app_thread_id_manage,
                             app_thread_manage_routine_ready_cb,
                             app_thread_manage_routine_package_cb);
}
