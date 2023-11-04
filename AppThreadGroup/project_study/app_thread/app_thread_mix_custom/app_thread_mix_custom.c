/*实现目标:
 *    杂项事件不归属与其他线程模组时放于此处
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_mix_custom_routine_ready_cb(void)
{
    /* 相关模组初始化 */
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_mix_custom_routine_package_cb(app_thread_package_t *package, uint32_t *discard_count)
{
    switch (package->module) {
    case app_thread_mix_custom_data_dump: {
        /* 将系统敏感的资源转储到外存 */
        app_module_data_dump_process();
        return true;
    }
    case app_thread_mix_custom_data_load: {
        /* 将系统敏感的资源加载到内存 */
        app_module_data_load_process();
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief 混合事件线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_mix_custom_routine)
{
    app_thread_slave_process(app_thread_id_mix_custom,
                             app_thread_mix_custom_routine_ready_cb,
                             app_thread_mix_custom_routine_package_cb);
}
