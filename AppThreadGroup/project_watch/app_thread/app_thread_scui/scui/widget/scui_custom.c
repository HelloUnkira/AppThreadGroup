/*实现目标:
 *    控件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 自定义控件创建
 *@param maker  自定义控件创建参数
 *@param handle 自定义控件句柄
 *@param layout 通过布局创建
 */
void scui_custom_create(scui_custom_maker_t *maker, scui_handle_t *handle, bool layout)
{
}

/*@brief 自定义控件销毁
 *@param handle 自定义控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_custom_destroy(scui_handle_t handle, bool parent_way)
{
}