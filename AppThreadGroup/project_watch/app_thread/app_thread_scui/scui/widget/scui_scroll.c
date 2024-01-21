/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 可滚动控件创建
 *@param maker  可滚动控件创建参数
 *@param handle 可滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout)
{
}

/*@brief 可滚动控件销毁
 *@param handle 可滚动控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_scroll_destroy(scui_handle_t handle, bool parent_way)
{
}
