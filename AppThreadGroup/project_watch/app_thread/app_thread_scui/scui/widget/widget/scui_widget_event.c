/*实现目标:
 *    控件事件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param clip   剪切域
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, scui_area_t *clip, bool sync)
{
    SCUI_LOG_INFO("%u", handle);
    
    // 不要给已经销毁的目标添加剪切域
    if (scui_handle_unmap(handle))
        return;
    if (scui_widget_style_is_hide(handle))
        return;
    
    scui_widget_t *widget = scui_handle_get(handle);
    scui_handle_t  handle_root = scui_widget_root(handle);
    scui_widget_t *widget_root = scui_handle_get(handle_root);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget_root != NULL);
    
    // 如果该控件控件树为自定义控件树
    // 不走系统管理,转为回调直达,自定义控件树会重定向
    if (widget->parent != SCUI_HANDLE_INVALID &&
        widget_root->type != scui_widget_type_window) {
        scui_event_t event = {
            .object = handle,
            .type   = scui_event_draw,
        };
        scui_widget_event_proc(&event);
        return;
    }
    
    scui_area_t widget_clip = widget->clip;
    if (clip != NULL)
    if (!scui_area_inter2(&widget_clip, clip))
         return;
    
    if (widget->parent == SCUI_HANDLE_INVALID &&
        scui_widget_surface_only(widget)) {
        widget_clip.x -= widget->clip.x;
        widget_clip.y -= widget->clip.y;
    }
    
    /* 为所有控件及其子控件添加指定剪切域 */
    // scui_widget_clip_check(handle_root, true);
    scui_widget_clip_reset(widget_root, &widget_clip, true);
    // scui_widget_clip_check(handle_root, true);
    scui_widget_clip_update(widget_root);
    // scui_widget_clip_check(handle_root, true);
    
    scui_event_t event = {
        .object     = handle_root,
        .style.sync = sync,
        .type       = scui_event_draw,
        .absorb     = scui_event_absorb_none,
    };
    scui_event_notify(&event);
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
        .object     = handle_root,
        .style.sync = sync,
        .type       = scui_event_refr,
        .absorb     = scui_event_absorb_none,
    };
    scui_event_notify(&event);
}

/*@brief 控件显示
 *@param handle 控件句柄
 */
static void scui_widget_show_delay(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    
    if (scui_handle_unmap(handle)) {
        scui_widget_create_layout_tree(handle);
        
        SCUI_LOG_DEBUG("");
        scui_event_t event = {
            .object     = handle,
            .type       = scui_event_show,
            .style.sync = true,
        };
        scui_event_notify(&event);
    }
    
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    widget->style.state = true;
    
    /* 将该显示窗口加入到场景管理器中 */
    if (widget->type == scui_widget_type_window)
        scui_window_list_add(widget->myself);
    
    /* 布局更新 */
    scui_widget_cb_t *widget_cb = NULL;
    scui_widget_cb_find(widget->type, &widget_cb);
    if (widget_cb->layout != NULL)
        widget_cb->layout(widget->myself);
    
    bool only = scui_widget_surface_only(widget);
    scui_widget_draw(widget->myself, NULL, only);
}

/*@brief 控件显示
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_show(scui_handle_t handle, bool delay)
{
    if (delay) {
        scui_event_t event = {
            .object         = SCUI_HANDLE_SYSTEM,
            .style.priority = scui_event_priority_real_time,
            .type           = scui_event_sched_delay,
            .sched          = scui_widget_show_delay,
            .handle         = handle,
        };
        scui_event_notify(&event);
        return;
    }
    scui_widget_show_delay(handle);
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 */
static void scui_widget_hide_delay(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("");
    
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    /* 设置控件状态为隐藏 */
    widget->style.state = false;
    
    /* 通知父窗口重绘 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        bool only = scui_widget_surface_only(widget);
        scui_widget_draw(widget->parent, NULL, only);
    }
    
    /* 布局更新 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        SCUI_ASSERT(widget_parent != NULL);
        scui_widget_cb_t *widget_cb = NULL;
        scui_widget_cb_find(widget_parent->type, &widget_cb);
        if (widget_cb->layout != NULL)
            widget_cb->layout(widget->parent);
    }
    
    // 重新刷新窗口列表
    scui_widget_refr(widget->myself, false);
    
    /* 将该显示窗口移除出场景管理器中 */
    if (widget->type == scui_widget_type_window)
        scui_window_list_del(widget->myself);
    
    /* 只有销毁窗口时才做整体销毁 */
    if (widget->parent == SCUI_HANDLE_INVALID) {
        
        SCUI_LOG_DEBUG("");
        scui_event_t event = {
            .object     = widget->myself,
            .type       = scui_event_hide,
            .style.sync = true,
        };
        scui_event_notify(&event);
        
        scui_widget_clip_clear(widget, true);
        scui_widget_destroy(widget->myself);
        
        // 窗口已销毁,移除掉跟本窗口相关的所有绘制事件
        event.object = widget->myself;
        event.type = scui_event_draw;
        while (scui_event_dequeue(&event, true));
    }
}

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_hide(scui_handle_t handle, bool delay)
{
    if (delay) {
        scui_event_t event = {
            .object         = SCUI_HANDLE_SYSTEM,
            .style.priority = scui_event_priority_real_time,
            .type           = scui_event_sched_delay,
            .sched          = scui_widget_hide_delay,
            .handle         = handle,
        };
        scui_event_notify(&event);
        return;
    }
    
    scui_widget_hide_delay(handle);
}

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 */
void scui_widget_event_clear(scui_handle_t handle)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_event_cb_clear(&widget->list);
}

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调节点
 */
void scui_widget_event_find(scui_handle_t handle, scui_event_cb_node_t *node)
{
    SCUI_ASSERT(node != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_event_cb_find(&widget->list, node);
}

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_event_cb_node_t *node)
{
    SCUI_ASSERT(node != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_LOG_DEBUG("widget %u", handle);
    
    scui_event_cb_add(&widget->list, node);
}

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_event_cb_node_t *node)
{
    SCUI_ASSERT(node != NULL);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_LOG_DEBUG("widget %u", handle);
    
    scui_event_cb_del(&widget->list, node);
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_proc(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    /* 准备抓取控件事件响应回调 */
    scui_handle_t handle = event->object;
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_event_cb_node_t cb_node = {.event = event->type,};
    scui_widget_event_find(handle, &cb_node);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (cb_node.event_cb != NULL)
        cb_node.event_cb(event);
        return;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_dispatch(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_handle_unmap(widget->myself))
        return;
    
    /*************************************************************************/
    /* 动画事件:顺向递归**************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_anima_elapse) {
        // 全局帧动画不分三步调度
        if (!scui_widget_event_check_execute(event))
             return;
        scui_widget_event_mask_keep(event);
        /* 全局滚动检查 */
        #if SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL
        scui_handle_t handle = SCUI_HANDLE_INVALID;
        if (scui_widget_event_scroll_flag(0x02, &handle))
            return;
        #endif
        /* 继续冒泡,继续下沉 */
        scui_widget_child_list_btra(widget, idx) {
            event->object = widget->child_list[idx];
            scui_widget_event_dispatch(event);
        }
        event->object = widget->myself;
        /* 冒泡自己 */
        if (widget->style.sched_anima)
            scui_widget_event_proc(event);
        return;
    }
    /*************************************************************************/
    /* 绘制事件:顺向递归**************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_draw) {
        
        if (scui_widget_event_check_execute(event))
            SCUI_LOG_INFO("widget:%d", widget->myself);
        
        bool surface_only = scui_widget_surface_only(widget);
        /* 无独立画布时不在draw绘制,转为到refr后绘制 */
        if (widget->parent == SCUI_HANDLE_INVALID) {
            if (!surface_only) {
                /* 无独立画布,如果是同步绘制,就地直达绘制画布 */
                if (event->style.sync) {
                    /* 只需要根控件时执行一次即可 */
                    /* 将整个控件树重定向到绘制画布上面去 */
                    if (widget->parent == SCUI_HANDLE_INVALID) {
                        scui_surface_t *surface = scui_frame_buffer_draw();
                        scui_widget_surface_remap(widget->myself, surface);
                    }
                } else {
                    scui_widget_refr(widget->myself, true);
                    scui_widget_event_mask_keep(event);
                    return;
                }
            }
        }
        /* 绘制事件不能被控件响应 */
        if (scui_widget_style_is_hide(widget->myself)) {
            SCUI_LOG_INFO("widget is hide");
            return;
        }
        /* 先冒泡自己,绘制事件没有剪切域,忽略 */
        if (scui_clip_empty(&widget->clip_set))
            SCUI_LOG_INFO("widget clip is empty");
        /* 先走控件绘制流程,再进行自定义绘制流程 */
        else {
            scui_tick_calc(0x20, NULL, NULL, NULL);
            scui_widget_event_draw(event);
            scui_widget_event_proc(event);
            scui_tick_calc(0x21, NULL, NULL, NULL);
            
            /* 去除surface剪切域,因为已经绘制完毕 */
            if (scui_widget_event_check_finish(event))
                scui_widget_clip_clear(widget, false);
        }
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_btra(widget, idx) {
            event->object = widget->child_list[idx];
            scui_widget_event_dispatch(event);
        }
        event->object = widget->myself;
        scui_widget_event_mask_keep(event);
        /* 绘制结束产生一次异步刷新 */
        if (surface_only)
            scui_widget_refr(widget->myself, false);
        return;
    }
    /*************************************************************************/
    /* 输入事件ptr:回溯递归************************************************* */
    /*************************************************************************/
    if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e) {
        // 输入事件不分三步调度
        if (!scui_widget_event_check_execute(event))
             return;
        SCUI_LOG_INFO("event %u", event->type);
        /* 有些事件是不允许被吸收的,它可能涉及到系统状态的维护 */
        bool event_filter = true;
        event_filter = event_filter && event->type != scui_event_ptr_hold;
        event_filter = event_filter && event->type != scui_event_ptr_down;
        event_filter = event_filter && event->type != scui_event_ptr_up;
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_ftra(widget, idx) {
            event->object = widget->child_list[idx];
            /* 事件如果被吸收,终止派发 */
            scui_widget_event_dispatch(event);
            if (scui_widget_event_check_over(event))
                break;
        }
        event->object = widget->myself;
        scui_widget_event_mask_keep(event);
        if (scui_widget_event_check_over(event) && event_filter)
            return;
        /* 是否自己吸收处理 */
        if (!widget->style.indev_ptr)
             return;
        /* 有些事件涉及到全局状态维护,需要同步传递 */
        if (!event_filter)
             scui_widget_event_proc(event);
        /* 有些事件可能被吸收,选择传递 */
        if (event_filter) {
            scui_handle_t handle = scui_widget_root(widget->myself);
            scui_widget_t  *root = scui_handle_get(handle);
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
                scui_widget_event_proc(event);
        }
        
        return;
    }
    /*************************************************************************/
    /* 输入事件enc:回溯递归************************************************* */
    /*************************************************************************/
    if (event->type >= scui_event_enc_s && event->type <= scui_event_enc_e) {
        // 输入事件不分三步调度
        if (!scui_widget_event_check_execute(event))
             return;
        SCUI_LOG_INFO("event %u", event->type);
        /* 是否自己吸收处理(冒泡自己) */
        if (widget->style.indev_enc)
            scui_widget_event_proc(event);
            scui_widget_event_mask_keep(event);
        if (scui_widget_event_check_over(event))
            return;
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_ftra(widget, idx) {
            event->object = widget->child_list[idx];
            /* 事件如果被吸收,终止派发 */
            scui_widget_event_dispatch(event);
            if (scui_widget_event_check_over(event))
                break;
        }
        event->object = widget->myself;
        return;
    }
    /*************************************************************************/
    /* 输入事件key:回溯递归************************************************* */
    /*************************************************************************/
    if (event->type >= scui_event_key_s && event->type <= scui_event_key_e) {
        // 输入事件不分三步调度
        if (!scui_widget_event_check_execute(event))
             return;
        SCUI_LOG_INFO("event %u", event->type);
        /* 是否自己吸收处理(冒泡自己) */
        if (widget->style.indev_key)
            scui_widget_event_proc(event);
            scui_widget_event_mask_keep(event);
        if (scui_widget_event_check_over(event))
            return;
        /* 如果需要继续冒泡,则继续下沉 */
        scui_widget_child_list_ftra(widget, idx) {
            event->object = widget->child_list[idx];
            /* 事件如果被吸收,终止派发 */
            scui_widget_event_dispatch(event);
            if (scui_widget_event_check_over(event))
                break;
        }
        event->object = widget->myself;
        return;
    }
    /*************************************************************************/
    /*流程派发 ************************************************************* */
    /*************************************************************************/
    if (event->type == scui_event_font_change) {
        // 控件事件不分三步调度
        if (!scui_widget_event_check_execute(event))
             return;
        scui_widget_event_mask_keep(event);
        scui_widget_event_bubble(event, NULL, true);
        return;
    }
    /*************************************************************************/
    /*流程派发 ************************************************************* */
    /*************************************************************************/
    if (event->type == scui_event_focus_get ||
        event->type == scui_event_show) {
        scui_widget_event_mask_keep(event);
        scui_widget_event_bubble(event, NULL, true);
        return;
    }
    /*************************************************************************/
    /*流程派发 ************************************************************* */
    /*************************************************************************/
    if (event->type == scui_event_focus_lost ||
        event->type == scui_event_hide) {
        scui_widget_event_mask_keep(event);
        scui_widget_event_bubble(event, NULL, false);
        return;
    }
    /*************************************************************************/
    /*其余事件:单次派发 **************************************************** */
    /*************************************************************************/
    if (event->type == scui_event_size_adjust) {
        scui_widget_event_mask_over(event);
        scui_widget_event_proc(event);
        return;
    }
    /*************************************************************************/
    /*其余事件:单次派发 **************************************************** */
    /*************************************************************************/
    /* 其他未列举事件走默认派发流程,单次派发 */
    // SCUI_LOG_INFO("unknown dispatch");
    event->object = scui_widget_root(widget->myself);
    scui_widget_event_proc(event);
    event->object = widget->myself;
    
    return;
}

/*@brief 控件事件冒泡
 *       亦可用于控件迭代等其他动作
 *@param event    事件(可以是假事件)
 *@param event_cb 事件回调(可以是假事件回调)
 *@param first    优先冒泡自己
 */
void scui_widget_event_bubble(scui_event_t *event, scui_event_cb_t event_cb, bool first)
{
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    if (first) {
        if (event_cb != NULL)
            event_cb(event);
        else
            scui_widget_event_proc(event);
        if (scui_widget_event_check_over(event))
            return;
    }
    
    scui_widget_child_list_btra(widget, idx) {
        event->object = widget->child_list[idx];
        scui_widget_event_bubble(event, event_cb, first);
        if (scui_widget_event_check_over(event))
            break;
    }
    event->object = widget->myself;
    if (scui_widget_event_check_over(event))
        return;
    
    if (!first) {
        if (event_cb != NULL)
            event_cb(event);
        else
            scui_widget_event_proc(event);
        if (scui_widget_event_check_over(event))
            return;
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_draw(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    
    SCUI_LOG_DEBUG("event %u", event->type);
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    /* 控件透明则不绘制 */
    if (widget->style.trans)
        return;
    
    scui_widget_event_mask_keep(event);
    /* 有背景图片则优先绘制背景 */
    /* 没有背景图片则绘制纯色背景 */
    if (widget->image != SCUI_HANDLE_INVALID)
        /* 优化点:如果背景图片和设备FB类型一致则可直达,否则就不直达 */
        scui_widget_draw_image(event->object, NULL, widget->image, NULL, widget->color);
    else
        scui_widget_draw_color(event->object, NULL, widget->color);
}

/*@prief 事件滚动状态检查更新
 *@param type 0x00:锁定; 0x01:解锁; 0x02:检查
 *@param key  锁定标记句柄(浮动校验密钥)
 */
bool scui_widget_event_scroll_flag(uint8_t state, scui_handle_t *key)
{
    static struct {
        bool lock;
        scui_handle_t key;
    } scroll_flag = {0};
    
    switch (state) {
    case 0x00:
        if (scroll_flag.lock && scroll_flag.key == *key)
            return true;
        if (scroll_flag.lock && scroll_flag.key != *key) {
            *key = SCUI_HANDLE_INVALID;
            return false;
        }
        scroll_flag.key  = scui_handle_find();
        scroll_flag.lock = true;
        *key = scroll_flag.key;
        return true;
    case 0x01:
        if (scroll_flag.lock && scroll_flag.key == *key) {
            scui_handle_set(scroll_flag.key, NULL);
            scroll_flag.key  = SCUI_HANDLE_INVALID;
            scroll_flag.lock = false;
            *key = SCUI_HANDLE_INVALID;
             return true;
        }
        *key = SCUI_HANDLE_INVALID;
        return false;
    case 0x02:
        if (scroll_flag.lock && scroll_flag.key == *key)
            return false;
        *key = SCUI_HANDLE_INVALID;
        return scroll_flag.lock;
    default:
        SCUI_LOG_ERROR("unknown state");
        return false;
    }
}
