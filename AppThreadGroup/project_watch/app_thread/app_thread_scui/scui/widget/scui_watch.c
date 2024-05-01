/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 表盘指针控件创建
 *@param maker  表盘指针控件创建参数
 *@param handle 表盘指针控件句柄
 *@param layout 通过布局创建
 */
void scui_watch_create(scui_watch_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_watch);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建表盘指针控件实例 */
    scui_watch_t *watch = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_watch_t));
    memset(watch, 0, sizeof(scui_watch_t));
    
    /* 创建基础控件实例 */
    scui_widget_create(&watch->widget, &maker->widget, handle, layout);
    
    /* 为表盘指针控件添加指定的事件回调 */
    scui_event_cb_node_t cb_node = {.event_cb = maker->widget.event_cb,};
}

/*@brief 表盘指针控件销毁
 *@param handle 表盘指针控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_watch_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_watch_t *watch = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_watch);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&watch->widget);
    
    /* 销毁表盘指针控件实例 */
    SCUI_MEM_FREE(watch);
}
