/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 这里不存在并发读写导致的时序不同步,无需保护 */
static bool app_module_data_load_status_not_over = true;

/*@brief 加载流程是否结束
 */
bool app_module_data_load_not_over(void)
{
    return app_module_data_load_status_not_over;
}

/*@brief 资源数据从外存加载到内存
 */
void app_module_data_load_process(void)
{
    APP_SYS_LOG_WARN("start");
    APP_SYS_LOG_WARN("...");
    /* ... */
    app_module_data_load_status_not_over = false;
    APP_SYS_LOG_WARN("end");
}

/*@brief 系统加载模组启动
 */
void app_module_data_load_event(void)
{
    app_module_data_load_status_not_over = true;
    /* 向线程发送加载事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_mix_custom,
        .module = app_thread_mix_custom_data_load,
        .event  = 0,
    };
    app_thread_package_notify(&package);
    APP_SYS_LOG_WARN("");
}
