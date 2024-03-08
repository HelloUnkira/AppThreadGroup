/*实现目标:
 *    控件事件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件吸收
 */
static bool scui_widget_draw_absorb(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
    return true;
}

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param clip   剪切域
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, scui_area_t *clip, bool sync)
{
    SCUI_LOG_INFO("%u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    scui_handle_t  handle_root = scui_widget_root(handle);
    scui_widget_t *widget_root = scui_handle_get(handle_root);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget_root != NULL);
    
    scui_area_t clip_valid = widget->clip;
    
    if (clip != NULL) {
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget->clip, clip))
            clip_valid = clip_inter;
    }
    
    /* 为所有控件及其子控件添加指定剪切域 */
    // scui_widget_clip_check(widget_root, true);
    scui_widget_clip_reset(widget_root, &clip_valid, true);
    // scui_widget_clip_check(widget_root, true);
    scui_widget_clip_update(widget_root);
    // scui_widget_clip_check(widget_root, true);
    
    scui_event_t event = {
        .object = handle_root,
        .type   = scui_event_draw,
        .absorb = scui_widget_draw_absorb,
        .style  = sync ? scui_event_style_sync : 0,
    };
    scui_event_notify(&event);
}

/*@brief 事件吸收
 */
static bool scui_widget_refr_absorb(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
    return true;
}

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync)
{
    SCUI_LOG_INFO("%u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    scui_handle_t  handle_root = scui_widget_root(handle);
    scui_widget_t *widget_root = scui_handle_get(handle_root);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget_root != NULL);
    
    scui_event_t event = {
        .object = handle_root,
        .type   = scui_event_refr,
        .absorb = scui_widget_refr_absorb,
        .style  = sync ? scui_event_style_sync : 0,
    };
    scui_event_notify(&event);
}

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_show(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    
    if (scui_handle_remap(handle) &&
        scui_widget_style_is_show(handle)) {
        SCUI_LOG_DEBUG("is show");
        return;
    }
    
    scui_widget_cb_create(handle);
    
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    /* 设置控件状态为显示 */
    widget->style.state = true;
    
    /* 将该显示窗口加入到场景管理器中 */
    if (widget->parent == SCUI_HANDLE_INVALID)
        scui_window_list_add(widget->myself);
    
    SCUI_LOG_DEBUG("");
    scui_event_t event = {
        .object = widget->myself,
        .type   = scui_event_show,
        .style  = scui_event_style_sync,
    };
    scui_event_notify(&event);
    
    bool only = scui_widget_surface_only(widget);
    scui_widget_draw(widget->myself, NULL, only);
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 */
void scui_widget_hide(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    
    if (scui_handle_unmap(handle) ||
        scui_widget_style_is_hide(handle)) {
        SCUI_LOG_DEBUG("is hide");
        return;
    }
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    /* 设置控件状态为隐藏 */
    widget->style.state = false;
    
    SCUI_LOG_DEBUG("");
    scui_event_t event = {
        .object = widget->myself,
        .type   = scui_event_hide,
        .style  = scui_event_style_sync,
    };
    scui_event_notify(&event);
    
    /* 通知父窗口重绘 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        bool only = scui_widget_surface_only(widget);
        scui_widget_draw(widget->parent, NULL, only);
    }
    
    /* 将该显示窗口移除出场景管理器中 */
    if (widget->parent == SCUI_HANDLE_INVALID)
        scui_window_list_del(widget->myself);
    
    scui_widget_cb_destroy(widget->myself);
}

/*@brief 控件显示(时序异步)
 *@param handle 控件句柄
 */
void scui_widget_show_delay(scui_handle_t handle)
{
    scui_event_t event = {
        .object   = SCUI_HANDLE_SYSTEM,
        .type     = scui_event_show_delay,
        .priority = scui_event_priority_real_time,
        .handle   = scui_widget_root(handle),
    };
    scui_event_notify(&event);
}

/*@brief 控件隐藏(时序异步)
 *@param handle 控件句柄
 */
void scui_widget_hide_delay(scui_handle_t handle)
{
    scui_event_t event = {
        .object   = SCUI_HANDLE_SYSTEM,
        .type     = scui_event_hide_delay,
        .priority = scui_event_priority_real_time,
        .handle   = scui_widget_root(handle),
    };
    scui_event_notify(&event);
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_hide_without(scui_handle_t handle, scui_handle_t child)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_widget_child_list_btra(widget, idx)
        if (widget->child_list[idx] != child)
            scui_widget_hide(widget->child_list[idx]);
}

/*@brief 控件树镜像(相对父控件)
 *@param handle 控件句柄
 */
void scui_widget_mirror(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* @等待适配,要用的时候再去实现 */
    SCUI_LOG_ERROR("not finish yet");
    return;
    
    scui_widget_child_list_btra(widget, idx) {
        
        scui_handle_t  handle_child = widget->child_list[idx];
        scui_widget_t *widget_child = scui_handle_get(handle);
        scui_point_t    point_child = {0};
        
        point_child.x -= widget->clip.x;
        point_child.y -= widget->clip.y;
        
        /// ...
        
        scui_widget_repos(handle_child, &point_child);
    }
}

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_find(scui_handle_t handle, scui_widget_event_t *event)
{
    SCUI_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_widget_event_t *event_old = NULL;
    
    /* 一个事件至多一个响应回调, 优先匹配对应事件 */
    scui_list_dll_ftra(&widget->dl_list, node) {
        event_old = scui_own_ofs(scui_widget_event_t, dl_node, node);
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
    scui_list_dll_ftra(&widget->dl_list, node) {
        event_old = scui_own_ofs(scui_widget_event_t, dl_node, node);
        if (event->order == event_old->order &&
           ((event_old->event == scui_event_sched_all && event_sched) ||
            (event_old->event == scui_event_ptr_all && event_ptr) ||
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
    SCUI_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_LOG_DEBUG("widget %u", handle);
    
    /* 一个事件至多一个响应回调,新的替换旧的 */
    scui_widget_event_t *event_old = NULL;
    scui_list_dll_btra(&widget->dl_list, node) {
        event_old = scui_own_ofs(scui_widget_event_t, dl_node, node);
        if (event_old->event == event->event &&
            event_old->order == event->order) {
            event_old->event_cb = event->event_cb;
            return;
        }
    }
    
    /* 初始化节点然后入列 */
    scui_widget_event_t *event_new = NULL;
     event_new = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_widget_event_t));
    *event_new = *event;
    scui_list_dln_reset(&event_new->dl_node);
    scui_list_dll_ainsert(&widget->dl_list, NULL, &event_new->dl_node);
}

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_widget_event_t *event)
{
    SCUI_ASSERT(event != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_LOG_DEBUG("widget %u", handle);
    
    /* 一个事件至多一个响应回调 */
    scui_widget_event_t *event_old = NULL;
    scui_list_dll_btra(&widget->dl_list, node) {
        event_old = scui_own_ofs(scui_widget_event_t, dl_node, node);
        if (event_old->event == event->event &&
            event_old->order == event->order)
            break;
        event_old = NULL;
    }
    
    if (event_old == NULL)
        return;
    /* 抓取到目标事件,移除它 */
    scui_list_dll_remove(&widget->dl_list, &event_old->dl_node);
    SCUI_MEM_FREE(event_old);
}

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_clear(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_list_dln_t *node = NULL;
    while ((node = scui_list_dll_head(&widget->dl_list)) != NULL) {
        scui_widget_event_t *event = scui_own_ofs(scui_widget_event_t, dl_node, node);
        /* 抓取事件,移除它 */
        scui_list_dll_remove(&widget->dl_list, &event->dl_node);
        SCUI_MEM_FREE(event);
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_proc(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    /* 准备抓取控件事件响应回调 */
    scui_event_retval_t ret = scui_event_retval_quit;
    scui_widget_event_t event_match = {.event = event->type,};
    scui_handle_t handle = event->object;
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    
    /*************************************************************************/
    /* 事件前响应回调 ****************************************************** */
    /*************************************************************************/
    event_match.order = scui_widget_order_before,
    scui_widget_event_find(handle, &event_match);
    if (event_match.event_cb != NULL)
        ret |= event_match.event_cb(event);
    if ((ret & scui_event_retval_over) != 0)
        return scui_event_retval_over;
    /*************************************************************************/
    /* 事件处理中响应回调 ************************************************** */
    /*************************************************************************/
    event_match.order = scui_widget_order_current,
    scui_widget_event_find(handle, &event_match);
    if (event_match.event_cb != NULL)
        ret |= event_match.event_cb(event);
    if ((ret & scui_event_retval_over) != 0)
        return scui_event_retval_over;
    /*************************************************************************/
    /* 事件后响应回调 ****************************************************** */
    /*************************************************************************/
    event_match.order = scui_widget_order_after,
    scui_widget_event_find(handle, &event_match);
    if (event_match.event_cb != NULL)
        ret |= event_match.event_cb(event);
    if ((ret & scui_event_retval_over) != 0)
        return scui_event_retval_over;
    
    return ret;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_dispatch(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    scui_event_retval_t ret = scui_event_retval_quit;
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    /*************************************************************************/
    /* 动画事件:顺向递归**************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_anima_elapse) {
        /* 继续冒泡,继续下沉 */
        scui_widget_child_list_btra(widget, idx) {
            event->object = widget->child_list[idx];
            ret |= scui_widget_event_dispatch(event);
        }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        /* 冒泡自己 */
        if (widget->style.anima_sched)
            scui_widget_event_proc(event);
        return ret;
    }
    /*************************************************************************/
    /* 刷新事件:就地处理**************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_refr) {
        scui_window_mix_surface();
        /* 混合绘制刷新流程结束 */
        /* 使用假绘制启动正式的刷新流程 */
        scui_surface_draw_routine(NULL);
        ret |= scui_event_retval_keep;
        return ret;
    }
    /*************************************************************************/
    /* 绘制事件:顺向递归**************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_draw) {
        /* 无独立画布时不在draw绘制,转为到refr后绘制 */
        if (widget->parent == SCUI_HANDLE_INVALID)
            if (!scui_widget_surface_only(widget)) {
                if (event->style != scui_event_style_sync) {
                    scui_widget_refr(widget->myself, true);
                    ret |= scui_event_retval_keep;
                    return ret;
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
        /* 绘制事件不能被控件响应 */
        if (scui_widget_style_is_hide(widget->myself)) {
            SCUI_LOG_INFO("widget is hide");
            return ret;
        }
        /* 先冒泡自己,绘制事件没有剪切域,忽略 */
        if (scui_clip_empty(&widget->clip_set))
            SCUI_LOG_INFO("widget clip is empty");
        else {
            ret |= scui_widget_event_draw(event);
            ret |= scui_widget_event_proc(event);
            /* 去除surface剪切域,因为已经绘制完毕 */
            scui_widget_clip_clear(widget, false);
        }
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_btra(widget, idx) {
            event->object = widget->child_list[idx];
            scui_widget_event_dispatch(event);
        }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        /* 绘制结束产生一次异步刷新 */
        scui_widget_refr(widget->myself, false);
        return ret;
    }
    /*************************************************************************/
    /* 输入事件ptr:回溯递归************************************************* */
    /*************************************************************************/
    if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e) {
        SCUI_LOG_INFO("event %u", event->type);
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_ftra(widget, idx) {
            event->object = widget->child_list[idx];
            /* 有些事件是不允许被吸收的,它可能涉及到系统状态的维护 */
            bool event_filter = true;
            event_filter = event_filter || event->type != scui_event_ptr_hold;
            event_filter = event_filter || event->type != scui_event_ptr_down;
            event_filter = event_filter || event->type != scui_event_ptr_up;
            /* 事件如果被吸收,终止派发 */
            ret |= scui_widget_event_dispatch(event);
            if ((ret & scui_event_retval_over) != 0 && event_filter)
                return scui_event_retval_over;
        }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        /* 是否自己吸收处理 */
        if (!widget->style.indev_ptr)
             return ret;
        scui_widget_t *root = scui_handle_get(scui_widget_root(widget->myself));
        /* 计算ptr的点是否坠落在此剪切域中,如果没有则放弃 */
        /* 通常来说ptr的点是屏幕上的点,屏幕相对窗口有偏移 */
        /* 窗口子控件相对窗口也有偏移 */
        scui_area_t clip = widget->clip;
        if (widget != root) {
            clip.x += root->clip.x;
            clip.y += root->clip.y;
        }
        /* 产生交集,事件可能被吸收(冒泡自己) */
        if (widget->parent == SCUI_HANDLE_INVALID ||
            scui_area_point(&clip, &event->ptr_c) ||
           (scui_area_point(&clip, &event->ptr_s) &&
            scui_area_point(&clip, &event->ptr_e)))
            ret |= scui_widget_event_proc(event);
        
        return ret;
    }
    /*************************************************************************/
    /* 输入事件enc:回溯递归************************************************* */
    /*************************************************************************/
    if (event->type >= scui_event_enc_s && event->type <= scui_event_enc_e) {
        SCUI_LOG_INFO("event %u", event->type);
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_ftra(widget, idx) {
            event->object = widget->child_list[idx];
            /* 事件如果被吸收,终止派发 */
            ret |= scui_widget_event_dispatch(event);
            if ((ret & scui_event_retval_over) != 0)
                return scui_event_retval_over;
        }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        /* 是否自己吸收处理(冒泡自己) */
        if (!widget->style.indev_enc)
             return ret;
        ret |= scui_widget_event_proc(event);
        return ret;
    }
    /*************************************************************************/
    /* 输入事件key:回溯递归************************************************* */
    /*************************************************************************/
    if (event->type >= scui_event_key_s && event->type <= scui_event_key_e) {
        SCUI_LOG_INFO("event %u", event->type);
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_ftra(widget, idx) {
            event->object = widget->child_list[idx];
            /* 事件如果被吸收,终止派发 */
            ret |= scui_widget_event_dispatch(event);
            if ((ret & scui_event_retval_over) != 0)
                return scui_event_retval_over;
        }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        /* 是否自己吸收处理(冒泡自己) */
        if (!widget->style.indev_key)
             return ret;
        ret |= scui_widget_event_proc(event);
        return ret;
    }
    /*************************************************************************/
    /*显示事件:流程派发 **************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_focus_get ||
        event->type == scui_event_show) {
        /* 是否自己吸收处理(冒泡自己) */
        ret |=scui_widget_event_proc(event);
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_btra(widget, idx) {
                event->object = widget->child_list[idx];
                scui_widget_event_dispatch(event);
            }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        return ret;
    }
    /*************************************************************************/
    /*隐藏事件:流程派发 **************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_focus_lost ||
        event->type == scui_event_hide) {
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_btra(widget, idx) {
                event->object = widget->child_list[idx];
                scui_widget_event_dispatch(event);
            }
        event->object = widget->myself;
        ret |= scui_event_retval_keep;
        /* 是否自己吸收处理(冒泡自己) */
        ret |=scui_widget_event_proc(event);
        return ret;
    }
    /*************************************************************************/
    /*其余事件:单次派发 **************************************************** */
    /*************************************************************************/
    /* 其他未列举事件走默认派发流程,单次派发 */
    SCUI_LOG_WARN("unknown dispatch");
    event->object = scui_widget_root(widget->myself);
    ret |= scui_widget_event_proc(event);
    event->object = widget->myself;
    
    return ret;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_draw(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    scui_event_retval_t ret = scui_event_retval_quit;
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    /* 控件默认绘制只处理背景皮肤 */
    if (widget->style.trans)
        return ret;
    
    /* 有背景图片则优先绘制背景 */
    /* 没有背景图片则绘制纯色背景 */
    if (widget->image != SCUI_HANDLE_INVALID)
        /* 优化点:如果背景图片和设备FB类型一致则可直达,否则就不直达 */
        scui_widget_surface_draw_image(widget, widget->image, NULL, widget->color);
    else
        scui_widget_surface_draw_color(widget, widget->color);
    
    ret |= scui_event_retval_keep;
    return ret;
}
