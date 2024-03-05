/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 可滚动控件创建
 *@param maker  可滚动控件创建参数
 *@param handle 可滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_scroll);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建自定义控件实例 */
    scui_custom_t *scroll = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_scroll_t));
    memset(scroll, 0, sizeof(scui_custom_t));
    
    /* 创建基础控件实例 */
    scui_widget_create(&scroll->widget, &maker->widget, handle, layout);
    
}

/*@brief 可滚动控件销毁
 *@param handle 可滚动控件句柄
 */
void scui_scroll_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_scroll);
    scui_custom_t *scroll = (void *)widget;
    SCUI_ASSERT(scroll != NULL);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&scroll->widget);
    
    /* 销毁自定义控件实例 */
    SCUI_MEM_FREE(scroll);
}

/*@brief 可滚动控件事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scroll_event(scui_event_t *event)
{
    return scui_event_retval_quit;
}
