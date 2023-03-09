/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_data_manage.h"
#include "app_module_clock.h"

static bool app_module_load_status_over = false;
static app_mutex_t app_module_load_mutex = {0};

/*@brief 加载流程是否结束
 */
bool app_module_load_over(void)
{
    app_mutex_take(&app_module_load_mutex);
    uint8_t over = app_module_load_status_over;
    app_mutex_give(&app_module_load_mutex);
    return over;
}

/*@brief 资源数据从外存加载到内存
 */
void app_module_load_process(void)
{
    /* 加载系统时钟 */
    app_module_clock_load();
    /* 继续添加...... */
    APP_SYS_LOG_WARN("...");
    /* ... */
    app_mutex_take(&app_module_load_mutex);
    app_module_load_status_over = true;
    app_mutex_give(&app_module_load_mutex);
}

/*@brief 系统加载模组启动
 */
void app_module_load_event(void)
{
    static bool not_load_yet = true;
    /* 向线程发送加载事件 */
    if (not_load_yet) {
        app_package_t package = {
            .thread = app_thread_id_data_manage,
            .module = app_thread_data_manage_load,
            .event  = 0,
        };
        app_package_notify(&package);
        APP_SYS_LOG_WARN("");
        not_load_yet = false;
    }
}

/*@brief 初始化系统加载模组
 */
void app_module_load_ready(void)
{
    app_mutex_process(&app_module_load_mutex);
}

