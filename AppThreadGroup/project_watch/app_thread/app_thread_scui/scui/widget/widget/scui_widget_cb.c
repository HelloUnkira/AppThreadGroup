/*实现目标:
 *    控件组织
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件类型检查
 *@param handle 控件句柄
 *@param type   控件类型
 *@retval 控件类型检查
 */
bool scui_widget_type_check(scui_handle_t handle, scui_widget_type_t type)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    // 控件类型相等
    if (widget->type == type)
        return true;
    
    // 控件是派生控件(继承过)
    switch (widget->type) {
    default:
        break;
    }
    
    return false;
}

/*@brief 查找控件映射表
 *@param type 控件类型
 *@param widget_cb 控件映射表
 */
void scui_widget_cb_find(scui_widget_type_t type, scui_widget_cb_t **widget_cb)
{
    /* 控件组织表 */
    static const scui_widget_cb_t scui_widget_cb[scui_widget_type_num] = {
        [scui_widget_type_window].create    = (scui_widget_cb_create_t)   scui_window_create,
        [scui_widget_type_window].destroy   = (scui_widget_cb_destroy_t)  scui_window_destroy,
        
        [scui_widget_type_custom].create    = (scui_widget_cb_create_t)   scui_custom_create,
        [scui_widget_type_custom].destroy   = (scui_widget_cb_destroy_t)  scui_custom_destroy,
        
        [scui_widget_type_scroll].create    = (scui_widget_cb_create_t)   scui_scroll_create,
        [scui_widget_type_scroll].destroy   = (scui_widget_cb_destroy_t)  scui_scroll_destroy,
        [scui_widget_type_scroll].layout    = (scui_widget_cb_layout_t)   scui_scroll_layout,
        [scui_widget_type_scroll].event_cb  = (scui_event_cb_t)           scui_scroll_event,
        
        [scui_widget_type_string].create    = (scui_widget_cb_create_t)   scui_string_create,
        [scui_widget_type_string].destroy   = (scui_widget_cb_destroy_t)  scui_string_destroy,
        [scui_widget_type_string].event_cb  = (scui_event_cb_t)           scui_string_event,
        
        /* 扩展控件 */
        [scui_widget_type_objbtn].create    = (scui_widget_cb_create_t)   scui_objbtn_create,
        [scui_widget_type_objbtn].destroy   = (scui_widget_cb_destroy_t)  scui_objbtn_destroy,
        [scui_widget_type_objbtn].event_cb  = (scui_event_cb_t)           scui_objbtn_event,
        
        /* 扩展控件 */
        [scui_widget_type_button].create    = (scui_widget_cb_create_t)   scui_button_create,
        [scui_widget_type_button].destroy   = (scui_widget_cb_destroy_t)  scui_button_destroy,
        [scui_widget_type_button].event_cb  = (scui_event_cb_t)           scui_button_event,
        
        [scui_widget_type_watch].create     = (scui_widget_cb_create_t)   scui_watch_create,
        [scui_widget_type_watch].destroy    = (scui_widget_cb_destroy_t)  scui_watch_destroy,
        [scui_widget_type_watch].event_cb   = (scui_event_cb_t)           scui_watch_event,
        
        [scui_widget_type_chart].create     = (scui_widget_cb_create_t)   scui_chart_create,
        [scui_widget_type_chart].destroy    = (scui_widget_cb_destroy_t)  scui_chart_destroy,
        [scui_widget_type_chart].event_cb   = (scui_event_cb_t)           scui_chart_event,
    };
    
    SCUI_ASSERT(type < scui_widget_type_num);
    *widget_cb = &scui_widget_cb[type];
}

/*@brief 通过映射表调用创建一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 控件句柄
 */
void scui_widget_create_layout_tree(scui_handle_t handle)
{
    if (scui_handle_remap(handle))
        return;
    
    scui_widget_t *widget = NULL;
    scui_widget_maker_t *maker = NULL;
    widget = maker = scui_handle_get(handle);
    SCUI_ASSERT(maker != NULL);
    SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
    
    scui_handle_table_t *handle_table = scui_handle_table_find(handle);
    SCUI_ASSERT(handle_table != NULL);
    
    do {
        /* 先创建根控件,然后延续依次创建剩下的控件 */
        /* 静态控件规则为,一个场景为一段连续句柄,父控件在前子控件在后 */
        scui_widget_cb_t *widget_cb = NULL;
        scui_widget_cb_find(maker->type, &widget_cb);
        widget_cb->create(maker, &handle, true);
        /* 迭代到下一个句柄 */
        handle++;
        widget = maker = scui_handle_get(handle);
        if (maker == NULL)
            break;
        /* 一直迭代到下一个根控件句柄前停下 */
        if ((scui_handle_unmap(handle) && maker->parent == SCUI_HANDLE_INVALID) ||
            (scui_handle_remap(handle) && widget->parent == SCUI_HANDLE_INVALID))
            break;
    } while (handle < handle_table->offset + handle_table->number);
}
