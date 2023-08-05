/*实现目标:
 *    数据中心管理线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_group.h"
#include "app_module_data_center.h"
#include "app_module_data_dump.h"
#include "app_module_data_load.h"
#include "app_module_shutdown.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"

/*@brief 数据管理线程初始化
 */
void app_thread_manage_ready(void)
{
    /* 模组初始化 */
    app_module_data_center_ready();
    app_module_protocol_ready();
}

/*@brief 子线程服务例程就绪部
 */
static void app_thread_manage_routine_ready_cb(void)
{
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_manage_routine_package_cb(app_thread_package_t *package, uint32_t *discard_count)
{
    switch (package->module) {
    case app_thread_manage_data_dump: {
        /* 将系统敏感的资源转储到外存 */
        app_module_data_dump_process();
        return true;
    }
    case app_thread_manage_data_load: {
        /* 将系统敏感的资源加载到内存 */
        app_module_data_load_process();
        return true;
    }
    case app_thread_manage_protocol: {
        if (package->event == app_thread_manage_protocol_notify)
            app_module_protocol_notify_handler(package->data, package->size);
        if (package->event == app_thread_manage_protocol_respond)
            app_module_protocol_respond_handler(package->data, package->size);
        if (package->dynamic)
            app_mem_free(package->data);
        return true;
    }
    case app_thread_manage_transfer: {
        if (package->event == app_thread_manage_transfer_notify)
            app_module_transfer_notify(0, package->data, package->size);
        if (package->event == app_thread_manage_transfer_respond)
            app_module_transfer_respond(package->data);
        if (package->dynamic)
            app_mem_free(package->data);
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief 数据管理线程服务例程
 */
void app_thread_manage_routine(void)
{
    app_thread_slave_routine(app_thread_id_manage,
                             app_thread_manage_routine_ready_cb,
                             app_thread_manage_routine_package_cb);
}
