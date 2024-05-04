/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图表控件创建
 *@param maker  图表控件创建参数
 *@param handle 图表控件句柄
 *@param layout 通过布局创建
 */
void scui_chart_create(scui_chart_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_chart);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建图表控件实例 */
    scui_chart_t *chart = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_chart_t));
    memset(chart, 0, sizeof(scui_chart_t));
    
    /* 创建基础控件实例 */
    scui_widget_create(&chart->widget, &maker->widget, handle, layout);
    
    /* 为图表控件添加指定的事件回调 */
    scui_event_cb_node_t cb_node = {.event_cb = scui_chart_event,};
    
    /* 事件默认全局接收 */
    cb_node.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &cb_node);
}

/*@brief 图表控件销毁
 *@param handle 图表控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_chart_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_chart_t  *chart  = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_chart);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&chart->widget);
    
    /* 销毁图表控件实例 */
    SCUI_MEM_FREE(chart);
}

/*@brief 图表控件事件处理回调
 *@param event 事件
 */
void scui_chart_event(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_chart_t   *chart = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        
        break;
    }
    default:
        break;
    }
}

