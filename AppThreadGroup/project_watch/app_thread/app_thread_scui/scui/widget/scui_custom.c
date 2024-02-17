/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件创建
 *@param maker  自定义控件创建参数
 *@param handle 自定义控件句柄
 *@param layout 通过布局创建
 */
void scui_custom_create(scui_custom_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_custom);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建自定义控件实例 */
    scui_custom_t *custom = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_custom_t));
    memset(custom, 0, sizeof(scui_custom_t));
    
    /* 创建基础控件实例 */
    scui_widget_create(&custom->widget, &maker->widget, handle, layout);
    
    /* 为自定义控件添加指定的事件回调 */
    scui_widget_event_t event = {0};
    event.order    = scui_widget_order_current;
    event.event_cb = maker->widget.event_cb;
    
    /* 事件默认全局接收 */
    event.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_ptr_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_enc_all;
    scui_widget_event_add(*handle, &event);
    event.event = scui_event_key_all;
    scui_widget_event_add(*handle, &event);
}

/*@brief 自定义控件销毁
 *@param handle 自定义控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_custom_destroy(scui_handle_t handle, bool parent_way)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_custom);
    scui_custom_t *custom = (void *)widget;
    SCUI_ASSERT(custom != NULL);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&custom->widget);
    
    /* 销毁自定义控件实例 */
    SCUI_MEM_FREE(custom);
}
