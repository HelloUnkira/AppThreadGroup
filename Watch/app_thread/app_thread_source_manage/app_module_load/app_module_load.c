/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_thread_interface.h"

/*@brief 资源数据从外存加载到内存
 */
void app_module_load(void)
{
    /* 加载系统时钟 */
    app_module_clock_load();
    app_module_stopwatch_load();
    app_module_countdown_load();
    /* 继续添加...... */
    APP_OS_LOG_WARN("\napp_module_load...\n");
    /* ... */
}
