/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 滚动控件创建
 *@param maker  可滚动控件创建参数
 *@param handle 可滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_scroll);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建自定义控件实例 */
    scui_scroll_t *scroll = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_scroll_t));
    memset(scroll, 0, sizeof(scui_scroll_t));
    
    /* 滚动控件必须设置事件响应标记 */
    SCUI_ASSERT(maker->widget.style.indev_ptr);
    SCUI_ASSERT(maker->widget.style.indev_enc);
    
    /* 创建基础控件实例 */
    scui_widget_create(&scroll->widget, &maker->widget, handle, layout);
    
    /* 状态初始化 */
    scroll->dir         = maker->dir;
    scroll->pos         = maker->pos;
    scroll->space       = maker->space;
    scroll->springback  = maker->springback;
    scroll->loop        = maker->loop;
    
    scroll->layout = true;
    
    /* 为滚动控件添加指定的事件回调 */
    scui_event_cb_node_t cb_node = {.event_cb = scui_scroll_event,};
    
    /* 事件默认全局接收 */
    cb_node.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &cb_node);
    cb_node.event = scui_event_ptr_all;
    scui_widget_event_add(*handle, &cb_node);
    cb_node.event = scui_event_enc_all;
    scui_widget_event_add(*handle, &cb_node);
}

/*@brief 滚动控件销毁
 *@param handle 可滚动控件句柄
 */
void scui_scroll_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_scroll);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&scroll->widget);
    
    /* 销毁自定义控件实例 */
    SCUI_MEM_FREE(scroll);
}

/*@brief 滚动控件布局更新
 *@param handle 可滚动控件句柄
 */
void scui_scroll_layout(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_scroll);
    
    scroll->layout = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
    
    /* 这里需要考虑回弹效果,包括回弹点,边界对齐 */
    
}

/*@brief 滚动控件动画回调
 */
void scui_scroll_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_handle_t  handle = anima->handle;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (scroll->dir) {
    case scui_event_dir_none: {
        /* 无方向移动,统一偏移量,限制到回弹点 */
        int32_t value_s = anima->value_s;
        int32_t value_e = anima->value_e;
        int32_t value_c = anima->value_c;
        scui_point_t point_s = scroll->point_s;
        scui_point_t point_e = scroll->point_e;
        
        scui_point_t offset = {
            .x = scui_map(value_c, value_s, value_e, point_s.x, point_e.x),
            .y = scui_map(value_c, value_s, value_e, point_s.y, point_e.y),
        };
        
        /* 在当前点偏移,现在去除该偏移 */
        offset.x -= scroll->ofs_cur.x;
        offset.y -= scroll->ofs_cur.y;
        
        /* 使用偏移范围限制offset,最多允许偏移到回弹点 */
        offset.x = scui_max(offset.x, scroll->ofs_min.x - scroll->springback);
        offset.x = scui_min(offset.x, scroll->ofs_max.x + scroll->springback);
        offset.y = scui_max(offset.y, scroll->ofs_min.y - scroll->springback);
        offset.y = scui_min(offset.y, scroll->ofs_max.y + scroll->springback);
        
        /* 更新偏移记录 */
        scroll->ofs_cur.x += offset.x;
        scroll->ofs_cur.y += offset.y;
        
        /* 偏移所有子控件 */
        scui_widget_refr_ofs_child_list(widget->myself, &offset);
        scui_widget_draw(widget->myself, NULL, false);
        break;
    }
    case scui_event_dir_hor:
    case scui_event_dir_ver: {
        int32_t value_s = anima->value_s;
        int32_t value_e = anima->value_e;
        int32_t value_c = anima->value_c;
        if (scroll->loop) {
            /* 方向移动,循环,忽略回弹点,重映射坐标区域 */
        } else {
            /* 方向移动,非循环,统一偏移量,限制到回弹点 */
            scui_coord_t delta = 0;
            delta = scui_map(value_c, value_s, value_e, 0, scroll->dis_lim);
            
            /* 在当前点偏移,现在去除该偏移 */
            delta -= scroll->dis_cur;
            
            /* 使用偏移范围限制offset,最多允许偏移到回弹点 */
            delta = scui_max(delta, 0 - scroll->springback);
            delta = scui_min(delta, scroll->dis_lim + scroll->springback);
            /* 更新偏移记录 */
            scroll->dis_cur += delta;
            
            scui_point_t offset = {0};
            if (scroll->dir == scui_event_dir_hor)
                offset.x = delta;
            if (scroll->dir == scui_event_dir_ver)
                offset.y = delta;
            
            /* 偏移所有子控件 */
            scui_widget_refr_ofs_child_list(widget->myself, &offset);
            scui_widget_draw(widget->myself, NULL, false);
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 滚动控件动画自动化
 */
void scui_scroll_anima_auto(scui_handle_t handle, int32_t value_s, int32_t value_e, uint32_t peroid)
{
    scui_anima_t anima = {0};
    anima.start   = scui_scroll_anima_start;
    anima.ready   = scui_scroll_anima_ready;
    anima.expired = scui_scroll_anima_expired;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.peroid  = peroid != 0 ? peroid : scui_abs(anima.value_e - anima.value_s) / 2;
    anima.handle  = handle;
    
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_scroll_anima_start(&anima);
        scui_scroll_anima_expired(&anima);
        scui_scroll_anima_ready(&anima);
        return;
    }
    scui_anima_create(&anima, &scroll->anima);
    scui_anima_start(scroll->anima);
}

/*@brief 滚动控件更新布局回调
 *@param event 事件
 */
void scui_scroll_update_layout(scui_event_t *event)
{
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    /* 仅标记布局触发布局更新 */
    if (scroll->layout)
        scroll->layout = false;
    else
        return;
    
    if (widget->child_num == 0)
        return;
    
    /* 仅指定布局触发布局更新 */
    /* 粘性布局,与自由布局类似,只需要边界自动对齐即可 */
    if (scroll->dir == scui_event_dir_none) {
        /* 还原布局内容 */
        if (scroll->ofs_cur.x != 0 ||
            scroll->ofs_cur.y != 0) {
            /* 还原布局是计算逆向偏移 */
            scroll->ofs_cur.x = -scroll->ofs_cur.x;
            scroll->ofs_cur.y = -scroll->ofs_cur.y;
            /* 偏移所有子控件 */
            scui_widget_refr_ofs_child_list(widget->myself, &scroll->ofs_cur);
            scroll->ofs_cur.x = 0;
            scroll->ofs_cur.y = 0;
        }
        
        /* 计算布局内容 */
        scui_area_t clip = widget->clip;
        /* 布局内容为所有子控件的全集 */
        scui_widget_child_list_btra(widget, idx) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_area_t clip_inter = {0};
            scui_area_merge(&clip_inter, &child->clip, &clip);
            clip = clip_inter;
        }
        scui_area_t clip_widget = widget->clip;
        scui_area_m_to_s(&clip);
        scui_area_m_to_s(&clip_widget);
        /* 计算自由布局下的ofs_min和ofs_max */
        scroll->ofs_min.x = -scui_dist(clip_widget.x1, clip.x1);
        scroll->ofs_min.y = -scui_dist(clip_widget.y1, clip.y1);
        scroll->ofs_max.x = +scui_dist(clip_widget.x2, clip.x2);
        scroll->ofs_max.y = +scui_dist(clip_widget.y2, clip.y2);
        return;
    }
    
    /* 只支持水平自动布局与垂直自动布局 */
    SCUI_ASSERT(scroll->dir == scui_event_dir_hor ||
                scroll->dir == scui_event_dir_ver);
    
    // 水平自动布局会调整所有子控件的(x,y,h)
    // 垂直自动布局会调整所有子控件的(x,y,w)
    scui_point_t pos = {
        .x = widget->clip.x,
        .y = widget->clip.y,
    };
    
    /* 根据布局方向调整子控件信息 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        /* 被隐藏的控件忽略 */
        if (scui_widget_style_is_hide(handle))
            continue;
        
        /* 更新子控件尺寸 */
        if (scroll->dir == scui_event_dir_hor)
            scui_widget_refr_size(handle, child->clip.w, widget->clip.h);
        if (scroll->dir == scui_event_dir_ver)
            scui_widget_refr_size(handle, widget->clip.w, child->clip.h);
        /* 更新子控件位置 */
        scui_widget_refr_pos(handle, &pos);
        /* 迭代到下一子控件 */
        if (scroll->dir == scui_event_dir_hor)
            pos.x += child->clip.w + scroll->space;
        if (scroll->dir == scui_event_dir_ver)
            pos.y += child->clip.h + scroll->space;
    }
    
    /* 空隙只夹杂在控件中 */
    if (scroll->dir == scui_event_dir_hor)
        pos.x -= scroll->space;
    if (scroll->dir == scui_event_dir_ver)
        pos.y -= scroll->space;
    
    /* 不同模式设置不同参数 */
    if (scroll->loop) {
    } else {
        scroll->dis_cur = 0;
        scroll->dis_lim = 0;
        if (scroll->dir == scui_event_dir_hor)
            scroll->dis_lim = pos.x - widget->clip.x;
        if (scroll->dir == scui_event_dir_ver)
            scroll->dis_lim = pos.y - widget->clip.y;
        SCUI_ASSERT(scroll->dis_lim != 0);
    }
    
    if (scui_widget_style_is_show(handle))
        scui_widget_draw(handle, NULL, false);
}

/*@brief 滚动控件事件处理回调
 *@param event 事件
 */
void scui_scroll_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_show:
        scui_scroll_update_layout(event);
        break;
    case scui_event_focus_get:
        scui_scroll_update_layout(event);
        break;
    case scui_event_ptr_down:
        break;
    case scui_event_ptr_move:
        break;
    case scui_event_ptr_fling:
        break;
    case scui_event_ptr_up:
        break;
    default:
        break;
    }
}
