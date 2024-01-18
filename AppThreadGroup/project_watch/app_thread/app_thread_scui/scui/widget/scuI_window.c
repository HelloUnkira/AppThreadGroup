/*实现目标:
 *    控件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 窗口创建
 *@param maker  窗口创建参数
 *@param handle 窗口句柄
 *@param layout 通过布局创建
 */
void scui_window_create(scui_window_maker_t *maker, scui_handle_t *handle, bool layout)
{
    if (!layout)
        *handle = scui_handle_new();
    
    scui_window_t *window = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_window_t));
    scui_handle_set(*handle, window);
    /* 创建实例 */
    scui_widget_create(&window->widget, &maker->widget, *handle);
    /* 窗口创建 */
}

/*@brief 窗口销毁
 *@param handle 窗口句柄
 */
void scui_window_destroy(scui_handle_t handle)
{
}

/*@brief 窗口事件归发回调
 *@param event 响应事件
 *@retval 事件处理状态
 */
scui_event_retval_t scui_window_event_proc(scui_event_t *event)
{
}
