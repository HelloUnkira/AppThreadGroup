/*实现目标:
 *    控件事件操作
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_find(scui_handle_t handle, scui_widget_event_t *event)
{
    APP_SYS_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    scui_widget_event_t *event_old = NULL;
    
    /* 一个事件至多一个响应回调, 优先匹配对应事件 */
    app_sys_list_dll_ftra(&widget->dl_list, node) {
        event_old = app_sys_own_ofs(scui_widget_event_t, dl_node, node);
        if (event->event == event_old->event &&
            event->order == event_old->order) {
            event->event_cb = event_old->event_cb;
            return;
        }
    }
    
    bool event_sched = event->event > scui_event_sched_s &&
                       event->event < scui_event_sched_e;
    bool event_ptr = event->event > scui_event_ptr_s &&
                     event->event < scui_event_ptr_e;
    bool event_enc = event->event > scui_event_enc_s &&
                     event->event < scui_event_enc_e;
    bool event_key = event->event > scui_event_key_s &&
                     event->event < scui_event_key_e;
    bool event_custom = event->event > scui_event_custom_s &&
                        event->event < scui_event_custom_e;
    
    /* 一个事件至多一个响应回调, 其次匹配集成事件 */
    app_sys_list_dll_ftra(&widget->dl_list, node) {
        event_old = app_sys_own_ofs(scui_widget_event_t, dl_node, node);
        if (event->order == event_old->order &&
           ((event_old->event == scui_event_sched_all && event_sched) ||
            (event_old->event == scui_event_key_all && event_ptr) ||
            (event_old->event == scui_event_enc_all && event_enc) ||
            (event_old->event == scui_event_key_all && event_key) ||
            (event_old->event == scui_event_custom_all && event_custom))) {
            event->event_cb = event_old->event_cb;
            return;
        }
    }
    
    event->event_cb = NULL;
}

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_widget_event_t *event)
{
    APP_SYS_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 一个事件至多一个响应回调,新的替换旧的 */
    scui_widget_event_t *event_old = NULL;
    app_sys_list_dll_btra(&widget->dl_list, node) {
        event_old = app_sys_own_ofs(scui_widget_event_t, dl_node, node);
        if (event_old->event == event->event &&
            event_old->order == event->order) {
            event_old->event_cb = event->event_cb;
            return;
        }
    }
    
    /* 初始化节点然后入列 */
    scui_widget_event_t *event_new = NULL;
     event_new = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_widget_event_t));
    *event_new = *event;
    app_sys_list_dln_reset(&event_new->dl_node);
    app_sys_list_dll_ainsert(&widget->dl_list, NULL, &event_new->dl_node);
}

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_widget_event_t *event)
{
    APP_SYS_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 一个事件至多一个响应回调 */
    scui_widget_event_t *event_old = NULL;
    app_sys_list_dll_btra(&widget->dl_list, node) {
        event_old = app_sys_own_ofs(scui_widget_event_t, dl_node, node);
        if (event_old->event == event->event &&
            event_old->order == event->order)
            break;
        event_old = NULL;
    }
    
    if (event_old == NULL)
        return;
    /* 抓取到目标事件,移除它 */
    app_sys_list_dll_remove(&widget->dl_list, &event_old->dl_node);
    SCUI_MEM_FREE(event_old);
}

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_clear(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    app_sys_list_dln_t *node = NULL;
    while ((node = app_sys_list_dll_head(&widget->dl_list)) != NULL) {
        scui_widget_event_t *event = app_sys_own_ofs(scui_widget_event_t, dl_node, node);
        /* 抓取事件,移除它 */
        app_sys_list_dll_remove(&widget->dl_list, &event->dl_node);
        SCUI_MEM_FREE(event);
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_proc(scui_event_t *event)
{
    /* 准备抓取控件事件响应回调 */
    uint32_t ret = scui_event_retval_continue;
    scui_widget_event_t event_match = {.event = event->type,};
    scui_widget_t *widget = scui_handle_get(event->object);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 事件前响应回调 */
    event_match.order = scui_widget_order_before,
    scui_widget_event_find(widget->myself, &event_match);
    if (event_match.event_cb != NULL)
        ret = event_match.event_cb(event);
    if (ret == scui_event_retval_break)
        return scui_event_retval_break;
    
    /* 事件处理中响应回调 */
    event_match.order = scui_widget_order_current,
    scui_widget_event_find(widget->myself, &event_match);
    if (event_match.event_cb != NULL)
        ret = event_match.event_cb(event);
    else
        ret = scui_widget_event_default(event);
    
    if (ret == scui_event_retval_break)
        return scui_event_retval_break;
    
    /* 事件后响应回调 */
    event_match.order = scui_widget_order_after,
    scui_widget_event_find(widget->myself, &event_match);
    if (event_match.event_cb != NULL)
        ret = event_match.event_cb(event);
    if (ret == scui_event_retval_break)
        return scui_event_retval_break;
    
    return ret;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_dispatch(scui_event_t *event)
{
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    uint32_t ret = scui_event_retval_continue;
    scui_widget_t *widget = scui_handle_get(event->object);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 刷新事件:就地处理 */
    if (event->type == scui_event_refr) {
        scui_scene_mgr_mix_surface();
        /* 混合绘制刷新流程结束 */
        /* 使用假绘制启动正式的刷新流程 */
        scui_surface_draw_routine(NULL);
    }
    /* 绘制事件:顺向递归 */
    if (event->type == scui_event_draw) {
        /* 无独立画布时不在draw绘制,转为到refr后绘制 */
        if (!scui_widget_surface_only(widget)) {
            if (event->style != scui_event_style_sync) {
                 scui_widget_refr(widget->myself, true);
                 return scui_event_retval_break;
            }
            /* 无独立画布,如果是同步绘制,就地直达绘制画布 */
            if (event->style == scui_event_style_sync) {
                /* 只需要根控件时执行一次即可 */
                /* 将整个控件树重定向到绘制画布上面去 */
                if (widget->parent == SCUI_HANDLE_INVALID) {
                    scui_surface_t *surface = scui_surface_fb_draw();
                    scui_widget_surface_change(widget, surface);
                }
            }
        }
        /* 先冒泡自己 */
        ret = scui_widget_event_proc(event);
        if (ret == scui_event_retval_break)
            return scui_event_retval_break;
        /* 去除自己的gc剪切域,因为已经绘制完毕 */
        widget->surface_clip = (scui_area_t){0};
        /* 如果需要继续冒泡,则继续下沉 */
        for (uint32_t idx = 0; idx < widget->child_num; idx++)
            if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
                event->object = widget->child_list[idx];
                scui_widget_event_dispatch(event);
            }
        event->object = widget->myself;
        return ret;
    }
    
    /* 输入事件ptr:回溯递归 */
    /* 输入事件key:回溯递归 */
    /* 输入事件enc:回溯递归 */
    if ((event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e) ||
        (event->type >= scui_event_enc_s && event->type <= scui_event_enc_e) ||
        (event->type >= scui_event_key_s && event->type <= scui_event_key_e)) {
        /* 如果需要继续冒泡,则继续下沉 */
        uint32_t child_num = widget->child_num;
        for (uint32_t idx = 0; idx < child_num; idx++)
            if (widget->child_list[child_num - 1 - idx] != SCUI_HANDLE_INVALID) {
                event->object = widget->child_list[child_num - 1 - idx];
                /* 事件如果被吸收,终止派发 */
                ret = scui_widget_event_dispatch(event);
                if (ret == scui_event_retval_break)
                    return scui_event_retval_break;
            }
        event->object = widget->myself;
        
        /* 是否自己吸收处理 */
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e && widget->style.indev_ptr) {
            scui_widget_t *root = scui_handle_get(scui_widget_root(widget->myself));
            /* 计算ptr的点是否坠落在此剪切域中,如果没有则放弃 */
            /* 通常来说ptr的点是屏幕上的点,屏幕相对窗口有偏移 */
            /* 窗口子控件相对窗口也有偏移 */
            scui_area_t clip = widget->clip;
            if (widget != root) {
                clip.x += root->clip.x;
                clip.y += root->clip.y;
            }
            /* 产生交集,事件可能被吸收 */
            if (scui_area_inside_point(&clip, &event->ptr_c) ||
               (scui_area_inside_point(&clip, &event->ptr_s) &&
                scui_area_inside_point(&clip, &event->ptr_e))) {
                /* 冒泡自己 */
                ret = scui_widget_event_proc(event);
                if (ret == scui_event_retval_break)
                    return scui_event_retval_break;
            }
        }
        
        /* 是否自己吸收处理 */
        if (event->type >= scui_event_enc_s && event->type <= scui_event_enc_e && widget->style.indev_enc) {
            /* 冒泡自己 */
            ret = scui_widget_event_proc(event);
            if (ret == scui_event_retval_break)
                return scui_event_retval_break;
        }
        
        /* 是否自己吸收处理 */
        if (event->type >= scui_event_key_s && event->type <= scui_event_key_e && widget->style.indev_key) {
            /* 冒泡自己 */
            ret = scui_widget_event_proc(event);
            if (ret == scui_event_retval_break)
                return scui_event_retval_break;
        }
        
        return ret;
    }
    
    return ret;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_default(scui_event_t *event)
{
    uint32_t ret = scui_event_retval_continue;
    scui_widget_t *widget = scui_handle_get(event->object);
    APP_SYS_ASSERT(widget != NULL);
    
    scui_widget_cb_t *widget_cb = scui_widget_cb_link(widget->type);
    
    switch (event->type) {
    case scui_event_draw: {
        /* 绘制事件不能被控件响应 */
        if (scui_widget_style_is_hide(widget->myself))
            return scui_event_retval_break;
        /* 绘制事件没有剪切域,忽略 */
        if (scui_area_empty(&widget->surface_clip))
            return scui_event_retval_break;
        
        if (widget_cb->event != NULL)
            ret = widget_cb->event(event);
        else
            ret = scui_widget_event_draw(event);
        
        scui_widget_clip_clear(widget);
        break;
    }
    case scui_event_lang_change: {
        #if 0
        /* wait adaptor */
        #endif
        break;
    }
    default:
        if (widget_cb->event != NULL)
            ret = widget_cb->event(event);
        
        break;
    }
    
    return ret;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_draw(scui_event_t *event)
{
    scui_widget_t *widget = scui_handle_get(event->object);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 控件默认绘制只处理背景皮肤 */
    if (widget->style.trans)
        return scui_event_retval_continue;
    
    /* 有背景图片则优先绘制背景 */
    /* 没有背景图片则绘制纯色背景 */
    if (widget->image != SCUI_HANDLE_INVALID)
        /* 优化点:如果背景图片和设备FB类型一致则可直达,否则就不直达 */
        scui_widget_surface_draw_image(widget, &widget->surface_clip, widget->image, NULL, widget->color);
    else
        scui_widget_surface_draw_color(widget, &widget->surface_clip, widget->color);
    
    return scui_event_retval_continue;
}