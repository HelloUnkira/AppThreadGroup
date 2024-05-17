/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字符串控件创建
 *@param maker  字符串控件创建参数
 *@param handle 字符串控件句柄
 *@param layout 通过布局创建
 */
void scui_string_create(scui_string_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_string);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建字符串控件实例 */
    scui_string_t *string = SCUI_MEM_ALLOC(scui_mem_type_none, sizeof(scui_string_t));
    memset(string, 0, sizeof(scui_string_t));
    
    /* 创建基础控件实例 */
    scui_widget_create(&string->widget, &maker->widget, handle, layout);
    
    /* 为字符串控件添加指定的事件回调 */
    scui_event_cb_node_t cb_node = {.event_cb = maker->widget.event_cb,};
    
    /* 事件默认全局接收 */
    cb_node.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &cb_node);
    
    /* 该控件未开发 */
    SCUI_ASSERT(false);
    
    /* 文字和字库这一步较为独立 */
    /* 后续可能考虑裁剪移植其他开源框架资源, 规避重复且无意义开发 */
}

/*@brief 字符串控件销毁
 *@param handle 字符串控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_string_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&string->widget);
    
    /* 销毁字符串控件实例 */
    SCUI_MEM_FREE(string);
}
