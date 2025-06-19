/* 实现目标:
 *     设备资源
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

/*@brief 初始化设备
 */
void app_dev_ready(void)
{
    app_dev_log_ready(&app_dev_log);
}
