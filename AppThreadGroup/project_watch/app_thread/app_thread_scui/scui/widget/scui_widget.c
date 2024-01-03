/*实现目标:
 *    控件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 设置事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_cb_set(scui_handle_t handle, scui_widget_event_reg_t *event)
{
    APP_SYS_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    
    /* 一个事件至多一个响应回调,新的替换旧的 */
    scui_widget_event_reg_t *event_old = NULL;
    app_sys_list_dll_btra(&widget->dl_list, node) {
        event_old = app_sys_own_ofs(scui_widget_event_reg_t, dl_node, node);
        if (event_old->event == event->event &&
            event_old->order == event->order)
            event_old->event_cb = event->event_cb;
            return;
    }
    
    /* 初始化节点然后入列 */
    scui_widget_event_reg_t *event_new = NULL;
     event_new = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_widget_event_reg_t));
    *event_new = *event;
    app_sys_list_dln_reset(&event_new->dl_node);
    app_sys_list_dll_ainsert(&widget->dl_list, NULL, &event_new->dl_node);
}

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_cb_get(scui_handle_t handle, scui_widget_event_reg_t *event)
{
    APP_SYS_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    
    /* 一个事件至多一个响应回调,新的替换旧的 */
    scui_widget_event_reg_t *event_old = NULL;
    app_sys_list_dll_ftra(&widget->dl_list, node) {
        event_old = app_sys_own_ofs(scui_widget_event_reg_t, dl_node, node);
        if (event->event == event_old->event &&
            event->order == event_old->order)
            event->event_cb = event_old->event_cb;
            return;
    }
    event->event_cb = NULL;
    event->order = scui_widget_order_after;
}

/*@brief 控件默认事件处理回调
 *@param handle 控件句柄
 */
uint32_t scui_widget_event_paint(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    
    /* 控件默认绘制只处理背景皮肤 */
    if (widget->style & scui_widget_style_is_transparent)
        return scui_event_retval_continue;
    
    scui_area_t widget_clip = {0};
    scui_widget_gc_clip_get(widget, &widget_clip);
    
    /* 有背景图片则优先绘制背景 */
    /* 没有背景图片则绘制纯色背景 */
    if (widget->image.handle != SCUI_HANDLE_INVALID)
        /* 优化点:如果背景图片和设备FB类型一致则可直达,否则就不直达 */
        scui_widget_gc_draw_image(widget, &widget_clip, widget->image.handle, NULL, widget->color);
    else
        scui_widget_gc_draw_color(widget, &widget_clip, widget->color);
    
    return scui_event_retval_continue;
}

/*@brief 控件默认事件处理回调
 *@param handle 控件句柄
 *@param event  事件
 */
uint32_t scui_widget_event_proc(scui_handle_t handle, scui_event_t *event)
{
    scui_widget_t *widget = scui_handle_get(handle);
    
    /* 绘制事件没有剪切域,忽略 */
    if (event->type == scui_event_paint && scui_widget_gc_clip_empty(widget))
        return scui_event_retval_continue;
    
    uint32_t ret = scui_event_retval_continue;
    scui_event_t event_new = *event;
    event_new.object = widget->handle;
    scui_widget_event_reg_t event_reg = {.event = event->type,};
    
    /* 事件前响应回调 */
    event_reg.order = scui_widget_order_before,
    scui_widget_event_cb_get(widget->handle, &event_reg);
    if (event_reg.event_cb != NULL)
        ret = event_reg.event_cb(&event_new);
    
    if (ret == scui_event_retval_break)
        return scui_event_retval_break;
    
    switch (event->type) {
    case scui_event_paint: {
        scui_widget_ogz_t *ogz = scui_widget_ogz_func(widget->type);
        ret = ogz->event(widget, event);
        break;
    }
    case scui_event_language_change: {
        #if 0
        /* wait adaptor */
        #endif
        break;
    }
    default:
        break;
    }
    
    if (ret == scui_event_retval_break)
        return scui_event_retval_break;
    
    /* 事件后响应回调 */
    event_reg.order = scui_widget_order_after,
    scui_widget_event_cb_get(widget->handle, &event_reg);
    if (event_reg.event_cb != NULL)
        ret = event_reg.event_cb(&event_new);
    
    return ret;
}

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    if (widget->parent == SCUI_HANDLE_INVALID)
        return scui_widget_root(widget->parent);
    return handle;
}

/*@brief 控件alpha获取
 *@param handle 控件句柄
 *@param alpha  控件alpha
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    return widget->alpha;
}

/*@brief 控件alpha设置
 *@param handle 控件句柄
 *@param alpha  控件alpha
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_widget_list_alpha_update(widget, alpha);
    widget->alpha = alpha;
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_status_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    
    /* 如果它的父容器隐藏则它也隐藏(这是递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *parent = scui_handle_get(widget->parent);
        if (!scui_widget_status_is_show(parent->handle))
             return false;
    }
    
    /* 它自己的显示状态 */
    if ((widget->status & scui_widget_status_show) != 0)
        return true;
    
    return false;
}



