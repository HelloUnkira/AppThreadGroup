/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_system.h"
#include "app_module_source_mix.h"
#include "app_module_shutdown.h"

/*@brief 资源数据从内存转储到外存
 */
void app_module_dump(void)
{
    APP_SYS_LOG_WARN("...");
    /* 转储系统时钟 */
    app_module_clock_dump();
    /* ... */
    app_module_shutdown_dump();
    /* 如果是系统要求的资源转储 */
    if (app_module_system_status_get() != app_module_system_valid)
        app_module_system_dump_set(true);
}