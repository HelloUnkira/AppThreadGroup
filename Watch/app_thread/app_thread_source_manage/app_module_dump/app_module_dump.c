/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_thread_interface.h"

/*@brief 资源数据从内存转储到外存
 */
void app_module_dump(void)
{
    APP_OS_LOG_WARN("\napp_module_dump...\n");
    /* 转储系统时钟 */
    app_module_clock_dump();
    app_module_stopwatch_dump();
    app_module_countdown_dump();
    /* ... */
    
    /* 如果是系统要求的资源转储 */
    if (app_module_system_status_get() != app_module_system_valid)
        app_module_system_dump_set(true);
}
