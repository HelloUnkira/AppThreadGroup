/*实现目标:
 *    输入设备事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_ptr_notify(scui_indev_data_t *data)
{
}

/*@brief 输入设备初始化
 */
void scui_indev_ptr_ready(void)
{
}
