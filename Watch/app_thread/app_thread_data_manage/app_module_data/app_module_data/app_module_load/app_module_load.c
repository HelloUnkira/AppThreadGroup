/*实现目标:
 *    一些和资源转储相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_source_mix.h"

/*@brief 资源数据从外存加载到内存
 */
void app_module_load(void)
{
    /* 加载系统时钟 */
    app_module_clock_load();
    /* 继续添加...... */
    APP_SYS_LOG_WARN("\napp_module_load...\n");
    /* ... */
}
