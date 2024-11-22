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
        [scui_widget_type_window].make = (scui_widget_cb_make_t)scui_window_make,
        [scui_widget_type_window].burn = (scui_widget_cb_burn_t)scui_window_burn,
        [scui_widget_type_custom].make = (scui_widget_cb_make_t)scui_custom_make,
        [scui_widget_type_custom].burn = (scui_widget_cb_burn_t)scui_custom_burn,
        [scui_widget_type_scroll].make = (scui_widget_cb_make_t)scui_scroll_make,
        [scui_widget_type_scroll].burn = (scui_widget_cb_burn_t)scui_scroll_burn,
        [scui_widget_type_string].make = (scui_widget_cb_make_t)scui_string_make,
        [scui_widget_type_string].burn = (scui_widget_cb_burn_t)scui_string_burn,
        
        [scui_widget_type_scroll].layout = (scui_widget_cb_layout_t)scui_scroll_layout,
        
        [scui_widget_type_scroll].invoke = (scui_event_cb_t)scui_scroll_event,
        [scui_widget_type_string].invoke = (scui_event_cb_t)scui_string_event,
        
        /* 扩展控件 */
        [scui_widget_type_objbtn].make = (scui_widget_cb_make_t)scui_objbtn_make,
        [scui_widget_type_objbtn].burn = (scui_widget_cb_burn_t)scui_objbtn_burn,
        
        [scui_widget_type_objbtn].invoke = (scui_event_cb_t)scui_objbtn_event,
        
        /* 扩展控件 */
        [scui_widget_type_button].make = (scui_widget_cb_make_t)scui_button_make,
        [scui_widget_type_button].burn = (scui_widget_cb_burn_t)scui_button_burn,
        [scui_widget_type_watch].make  = (scui_widget_cb_make_t)scui_watch_make,
        [scui_widget_type_watch].burn  = (scui_widget_cb_burn_t)scui_watch_burn,
        [scui_widget_type_chart].make  = (scui_widget_cb_make_t)scui_chart_make,
        [scui_widget_type_chart].burn  = (scui_widget_cb_burn_t)scui_chart_burn,
        
        [scui_widget_type_button].invoke = (scui_event_cb_t)scui_button_event,
        [scui_widget_type_watch].invoke  = (scui_event_cb_t)scui_watch_event,
        [scui_widget_type_chart].invoke  = (scui_event_cb_t)scui_chart_event,
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
        widget_cb->make(maker, &handle, true);
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

/*@brief 创建控件
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_create(void *maker, scui_handle_t *handle, bool layout)
{
    scui_widget_maker_t *widget_maker = maker;
    scui_widget_cb_t *widget_cb = NULL;
    scui_widget_cb_find(widget_maker->type, &widget_cb);
    widget_cb->make(maker, handle, layout);
}

/*@brief 销毁控件
 *@param handle 控件句柄
 */
void scui_widget_destroy(scui_handle_t handle)
{
    /* 重复的销毁过滤 */
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    scui_widget_cb_t *widget_cb = NULL;
    scui_widget_cb_find(widget->type, &widget_cb);
    widget_cb->burn(handle);
}

