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
    
    /* 不给无效目标添加剪切域 */
    if (scui_handle_unmap(handle))
        return;
    if (scui_widget_is_hide(handle))
        return;
    
    #if SCUI_MEM_FEAT_MINI
    /* 分段绘制直接全局添加即可 */
    /* 所以迟延到最终绘制前进行 */ {
        scui_event_t event = {
            .object     = scui_widget_root(handle),
            .style.sync = sync,
            .type       = scui_event_draw,
            .absorb     = scui_event_absorb_none,
        };
        scui_event_notify(&event);
        
        // 同步绘制后移除调度队列中
        // 可能存在的异步绘制
        if (sync) {
            scui_event_t event = {0};
            event.object = scui_widget_root(handle);
            event.type   = scui_event_draw;
            // 移除跟主窗口相关所有绘制事件
            while (scui_event_dequeue(&event, true, false));
        }
        return;
    }
    #endif
    
    /* 没有绘制剪切域时不要绘制 */
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_handle_t  handle_r = scui_widget_root(handle);
    scui_widget_t *widget_r = scui_handle_source_check(handle_r);
    
    /* 非窗口控件树兼容 */
    /* 如果控件没有画布, 将事件转义给根控件 */
    if (widget_r->type != scui_widget_type_window)
    if (widget->surface == NULL) {
        scui_event_t event = {
            .object     = handle_r,
            .style.sync = true,
            .type       = scui_event_draw_empty,
            .absorb     = scui_event_absorb_none,
        };
        scui_event_notify(&event);
        #if SCUI_MEM_FEAT_MINI
        SCUI_ASSERT(false);
        #endif
        return;
    }
    
    scui_area_t widget_clip = widget->clip;
    if (clip != NULL && !scui_area_inter2(&widget_clip, clip))
        return;
    
    if (widget->parent == SCUI_HANDLE_INVALID &&
        scui_widget_surface_only(widget)) {
        widget_clip.x -= widget->clip.x;
        widget_clip.y -= widget->clip.y;
    }
    
    /* 为所有控件及其子控件添加指定剪切域 */
    // scui_widget_clip_check(handle_r, true);
    scui_widget_clip_reset(widget_r, &widget_clip, true);
    
    scui_event_t event = {
        .object     = handle_r,
        .style.sync = sync,
        .type       = scui_event_draw,
        .absorb     = scui_event_absorb_none,
    };
    scui_event_notify(&event);
    
    // 同步绘制后移除调度队列中
    // 可能存在的异步绘制
    if (sync) {
        scui_event_t event = {0};
        event.object = handle_r;
        event.type   = scui_event_draw;
        // 移除跟主窗口相关所有绘制事件
        while (scui_event_dequeue(&event, true, false));
    }
}

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync)
{
    SCUI_LOG_INFO("%u", handle);
    
    scui_event_t event = {
        .object     = SCUI_HANDLE_SYSTEM,
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
    SCUI_LOG_INFO("widget :%u", handle);
    
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
    
    scui_widget_t *widget = scui_handle_source_check(handle);
    /* 设置控件状态为显示 */
    widget->style.state = true;
    
    /* 将该显示窗口加入到场景管理器中 */
    if (widget->type == scui_widget_type_window)
        scui_window_list_add(widget->myself);
    
    /* 布局更新 */
    scui_event_t event = {
        .object = widget->myself,
        .type   = scui_event_layout,
        .absorb = scui_event_absorb_none,
    };
    scui_event_notify(&event);
    
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
    SCUI_LOG_INFO("widget :%u", handle);
    
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = scui_handle_source_check(handle);
    /* 设置控件状态为隐藏 */
    widget->style.state = false;
    
    /* 通知父窗口重绘 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        bool only = scui_widget_surface_only(widget);
        scui_widget_draw(widget->parent, NULL, only);
    }
    
    /* 布局更新 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_source_check(widget->parent);
        
        scui_event_t event = {
            .object = widget->parent,
            .type   = scui_event_layout,
            .absorb = scui_event_absorb_none,
        };
        scui_event_notify(&event);
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
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_event_cb_clear(&widget->list);
}

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调节点
 */
void scui_widget_event_find(scui_handle_t handle, scui_event_cb_node_t *node)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    SCUI_ASSERT(node != NULL);
    scui_event_cb_find(&widget->list, node);
}

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_event_cb_node_t *node)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    SCUI_ASSERT(node != NULL);
    scui_event_cb_add(&widget->list, node);
}

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_event_cb_node_t *node)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    SCUI_ASSERT(node != NULL);
    scui_event_cb_del(&widget->list, node);
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
static void scui_widget_event_process(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    
    /* 可以在此处标记为访问 */
    /* 未知事件不会流转此处 */
    scui_event_mask_keep(event);
    
    /* 默认控件事件处理(ptr) */
    if (event->type >= scui_event_ptr_s &&
        event->type <= scui_event_ptr_e)
        if (!widget->style.indev_ptr)
             return;
    /* 默认控件事件处理(enc) */
    if (event->type >= scui_event_enc_s &&
        event->type <= scui_event_enc_e)
        if (!widget->style.indev_enc)
             return;
    /* 默认控件事件处理(key) */
    if (event->type >= scui_event_key_s &&
        event->type <= scui_event_key_e)
        if (!widget->style.indev_key)
             return;
    
    /* 默认控件事件处理 */
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        // 仅仅被标记的控件才可响应该事件
        if (!widget->style.sched_anima)
             return;
        break;
    }
    case scui_event_draw: {
        
        /* 绘制事件不能被控件响应 */
        if (scui_widget_is_hide(widget->myself)) {
            SCUI_LOG_INFO("widget is hide");
            return;
        }
        /* 绘制事件没有剪切域,忽略 */
        if (scui_widget_draw_empty(event->object)) {
            SCUI_LOG_INFO("widget clip is empty");
            return;
        }
        
        #if SCUI_WIDGET_BORDER_TAG
        /* 控件绘制流程在完结步(最前) */
        if (scui_event_check_finish(event)) {
            scui_draw_graph_dsc_t graph_dsc = {
                .type = scui_draw_graph_type_line,
                .src_color.color.ch.a = 0xFF,
                .src_color.color.ch.r = scui_rand(0xFF),
                .src_color.color.ch.g = scui_rand(0xFF),
                .src_color.color.ch.b = scui_rand(0xFF),
                .line.src_width = 1,
            };
            
            scui_area_t clip = widget->clip;
            scui_area_m_to_s(&clip, &clip);
            
            graph_dsc.line.src_pos_1.x = clip.x1;
            graph_dsc.line.src_pos_2.x = clip.x2;
            
            graph_dsc.line.src_pos_1.y = clip.y1;
            graph_dsc.line.src_pos_2.y = clip.y1;
            scui_widget_draw_graph(event->object, NULL, &graph_dsc);
            graph_dsc.line.src_pos_1.y = clip.y2;
            graph_dsc.line.src_pos_2.y = clip.y2;
            scui_widget_draw_graph(event->object, NULL, &graph_dsc);
            
            graph_dsc.line.src_pos_1.y = clip.y1;
            graph_dsc.line.src_pos_2.y = clip.y2;
            
            graph_dsc.line.src_pos_1.x = clip.x1;
            graph_dsc.line.src_pos_2.x = clip.x1;
            scui_widget_draw_graph(event->object, NULL, &graph_dsc);
            graph_dsc.line.src_pos_1.x = clip.x2;
            graph_dsc.line.src_pos_2.x = clip.x2;
            scui_widget_draw_graph(event->object, NULL, &graph_dsc);
        }
        #endif
        
        /* 控件绘制流程在执行步 */
        if (scui_event_check_execute(event)) {
            
            /* 控件背景透明则不绘制 */
            if (widget->style.trans)
                break;
            
            /* 绘制图片背景(有背景图片) */
            /* 绘制纯色背景(没有背景图片) */
            if (widget->image != SCUI_HANDLE_INVALID)
                scui_widget_draw_image(event->object, NULL, widget->image, NULL, widget->color);
            else
                scui_widget_draw_color(event->object, NULL, widget->color);
        }
        break;
    }
    case scui_event_ptr_click:
    case scui_event_ptr_fling:
    case scui_event_ptr_move: {
        /* 计算ptr的点是否坠落在此剪切域中,如果没有则放弃 */
        scui_handle_t handle = scui_widget_root(widget->myself);
        scui_widget_t  *root = scui_handle_source_check(handle);
        /* 通常来说ptr的点是屏幕上的点,屏幕相对窗口有偏移 */
        /* 窗口子控件相对窗口也有偏移 */
        scui_area_t clip = widget->clip;
        if (widget->parent != SCUI_HANDLE_INVALID) {
            clip.x += root->clip.x;
            clip.y += root->clip.y;
        }
        /* 事件不能被控件响应 */
        if (scui_widget_is_hide(widget->myself)) {
            SCUI_LOG_INFO("widget is hide");
            return;
        }
        /* 不产生交集,事件不能被响应,无需发送 */
        if (!(widget->parent == SCUI_HANDLE_INVALID ||
              scui_area_point(&clip, &event->ptr_c) ||
             (scui_area_point(&clip, &event->ptr_s) &&
              scui_area_point(&clip, &event->ptr_e))))
              return;
        
        /* 基础控件拖动效果 */
        if (event->type == scui_event_ptr_move) {
            
            /* 控件指针移动跟随标记 */
            if (widget->style.indev_ptr_move) {
                
                scui_point_t offset = {0};
                offset.x = event->ptr_e.x - event->ptr_s.x;
                offset.y = event->ptr_e.y - event->ptr_s.y;
                scui_widget_move_ofs(widget->myself, &offset, true);
            }
        }
        break;
    }
    
    case scui_event_size_adjust: {
        if (widget->parent != SCUI_HANDLE_INVALID) {
            /* 子控件更新,父控件布局更新 */
            scui_event_t event = {
                .object = widget->parent,
                .type   = scui_event_layout,
                .absorb = scui_event_absorb_none,
            };
            scui_event_notify(&event);
        }
    }
    default:
        break;
    }
    
    
    
    /* 获取控件事件响应回调, 并且执行它 */
    SCUI_ASSERT(event->object != SCUI_HANDLE_INVALID);
    scui_event_cb_node_t cb_node = {.event = event->type,};
    scui_widget_event_find(event->object, &cb_node);
    if (cb_node.event_cb != NULL)
        cb_node.event_cb(event);
    
    
    
    /* 默认控件事件处理 */
    /* 有些事件不允许被吸收 */
    /* 它涉及到系统状态维护 */
    switch (event->type) {
    case scui_event_anima_elapse:
    case scui_event_draw:
    case scui_event_show:
    case scui_event_hide:
    case scui_event_ptr_down:
    case scui_event_ptr_hold:
    case scui_event_ptr_up:
    case scui_event_key_down:
    case scui_event_key_hold:
    case scui_event_key_up:
        /* 有些事件不允许被吸收 */
        /* 它涉及到系统状态维护 */
        scui_event_mask_quit(event);
        scui_event_mask_keep(event);
        break;
    default:
        break;
    }
}

/*@brief 控件事件冒泡
 *       亦可用于控件迭代等其他动作
 *@param event    事件(可以是假事件)
 *@param event_cb 事件回调(可以是假事件回调)
 *@param first    优先冒泡自己
 */
static void scui_widget_event_bubble(scui_event_t *event, scui_event_cb_t event_cb, bool first)
{
    scui_widget_t *widget = scui_handle_source_check(event->object);
    
    if (first) {
        if (event_cb != NULL) event_cb(event);
        else scui_widget_event_process(event);
        if (scui_event_check_over(event))
            return;
    }
    
    scui_widget_child_list_btra(widget, idx) {
        event->object = widget->child_list[idx];
        scui_widget_event_bubble(event, event_cb, first);
        event->object = widget->myself;
        if (scui_event_check_over(event))
            return;
    }
    
    if (!first) {
        if (event_cb != NULL) event_cb(event);
        else scui_widget_event_process(event);
        if (scui_event_check_over(event))
            return;
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_dispatch(scui_event_t *event)
{
    /* 事件调度序列可能有无效事件 */
    if (scui_handle_unmap(event->object))
        return;
    
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    SCUI_LOG_DEBUG("event %u", event->type);
    
    /* 输入事件ptr:回溯递归 */
    if (event->type >= scui_event_ptr_s &&
        event->type <= scui_event_ptr_e) {
        scui_widget_event_bubble(event, NULL, false);
        return;
    }
    /* 输入事件enc:顺向递归 */
    if (event->type >= scui_event_enc_s &&
        event->type <= scui_event_enc_e) {
        scui_widget_event_bubble(event, NULL, true);
        return;
    }
    /* 输入事件key:顺向递归 */
    if (event->type >= scui_event_key_s &&
        event->type <= scui_event_key_e) {
        scui_widget_event_bubble(event, NULL, true);
        return;
    }
    
    /* 控件事件widget:单次派发 */
    if (event->type >= scui_event_widget_s &&
        event->type <= scui_event_widget_e) {
        scui_widget_event_process(event);
        scui_event_mask_over(event);
        return;
    }
    
    /* 其他事件:泛用流程 */
    switch (event->type) {
    case scui_event_anima_elapse:
        scui_widget_event_bubble(event, NULL, false);
        return;
    case scui_event_draw: {
        scui_event_mask_keep(event);
        scui_widget_t *widget = scui_handle_source_check(event->object);
        /* 在调度的最开始, 需要根据实际情况做调整 */
        if (widget->parent == SCUI_HANDLE_INVALID) {
            bool surface_only = scui_widget_surface_only(widget);
            /* cond 1: 无独立画布,异步绘制 --> 传递到refr前绘制 */
            if (!surface_only && !event->style.sync) {
                scui_widget_refr(widget->myself, true);
                return;
            }
            /* cond 2: 无独立画布,同步绘制 --> 就地直达绘制画布 */
            if (!surface_only && event->style.sync) {
                scui_surface_t *surface_fb = scui_frame_buffer_draw();
                scui_widget_surface_remap(widget->myself, surface_fb);
            }
            /* cond 3: 有独立画布,绘制结束 --> 产生一次异步刷新 */
            if (surface_only && scui_event_check_finish(event))
                scui_widget_refr(widget->myself, false);
            
            /* 更新surface剪切域, 因为绘制即将开始 */
            if (scui_event_check_prepare(event)) {
                
                #if SCUI_MEM_FEAT_MINI
                /* 为所有控件及其子控件添加全局剪切域 */
                // scui_widget_clip_check(handle_r, true);
                scui_area_t clip = widget->clip;
                scui_widget_clip_reset(widget, &clip, true);
                #endif
                
                scui_multi_t size_old = 0, size_new = 0;
                // scui_widget_clip_sizes(widget->myself, &size_old);
                // scui_widget_clip_check(widget->myself, true);
                scui_widget_clip_update(widget);
                // scui_widget_clip_check(widget->myself, true);
                // scui_widget_clip_sizes(widget->myself, &size_new);
                SCUI_LOG_DEBUG("size_old: %d, size_new:%d", size_old, size_new);
            }
            /* 去除surface剪切域, 因为已经绘制完毕 */
            if (scui_event_check_finish(event))
                scui_widget_clip_clear(widget, false);
        }
        
        // 启用集成事件冒泡流程
        scui_tick_calc(0x20, NULL, NULL, NULL);
        scui_widget_event_bubble(event, NULL, true);
        scui_tick_calc(0x21, NULL, NULL, NULL);
        return;
    }
    case scui_event_show:
        scui_widget_event_bubble(event, NULL, true);
        return;
    case scui_event_hide:
        scui_widget_event_bubble(event, NULL, false);
        return;
    case scui_event_change_lang:
        scui_widget_event_bubble(event, NULL, false);
        return;
    case scui_event_focus_get:
    case scui_event_focus_lost:
    case scui_event_layout:
    case scui_event_size_auto:
    case scui_event_size_adjust:
        /* 其他事件:单次派发 */
        scui_widget_event_process(event);
        scui_event_mask_over(event);
        return;
    default: {
        // SCUI_LOG_INFO("unknown dispatch");
        /* 其他未列举事件走默认派发流程,单次派发 */
        scui_handle_t handle = event->object;
        event->object = scui_widget_root(handle);
        scui_widget_event_process(event);
        event->object = handle;
        return;
    }
    }
}
