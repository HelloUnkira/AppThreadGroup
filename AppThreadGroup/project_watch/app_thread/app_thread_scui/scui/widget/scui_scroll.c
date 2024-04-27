/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

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
    scroll->fling_page  = maker->fling_page;
    scroll->loop        = maker->loop;
    
    if (scroll->fling_page <= 0)
        scroll->fling_page  = 1;
    
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
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    
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
    
    if (widget->type != scui_widget_type_scroll)
        return;
    
    scroll->layout = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 滚动控件翻页数更新
 *@param handle 可滚动控件句柄
 *@param fling_page 翻页数
 */
void scui_scroll_fling_page(scui_handle_t handle, scui_coord_t fling_page)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (widget->type != scui_widget_type_scroll)
        return;
    
    scroll->fling_page = fling_page;
    
    if (scroll->fling_page <= 0)
        scroll->fling_page  = 1;
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
    scui_anima_t  *anima  = instance;
    scui_handle_t  handle = anima->handle;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    /* 这里需要考虑回弹效果,包括回弹点,边界对齐 */
    
    switch (scroll->dir) {
    case scui_event_dir_none: {
        /* 自由布局 */
        break;
        }
    case scui_event_dir_hor:
    case scui_event_dir_ver: {
        scui_event_t event = {.object = handle};
        
        /* 自动布局,非循环,循环 */
        SCUI_LOG_DEBUG("dis_ofs:%d", scroll->dis_ofs);
        SCUI_LOG_DEBUG("dis_sum:%d", scroll->dis_sum);
        scui_scroll_event_auto_merge(&event, 0x01);
        
        if (!scroll->hold_move) {
            
            if (!scroll->loop) {
                
                /* 到达回弹点,则开始回弹 */
                if (scroll->dis_ofs > +0) {
                    scroll->mask_springback = true;
                    scui_scroll_anima_auto(widget->myself, 0, -scroll->dis_ofs + 0, 0);
                    break;
                }
                if (scroll->dis_ofs < -scroll->dis_lim) {
                    scroll->mask_springback = true;
                    scui_scroll_anima_auto(widget->myself, 0, -scroll->dis_ofs - scroll->dis_lim, 0);
                    break;
                }
                /* 进行回弹则不再校正 */
                if (scroll->mask_springback)
                    break;
            }
            
            if (scroll->dis_lim == 0)
                break;
            
            /* 未回弹则需要校正 */
            scui_scroll_event_auto_merge(&event, 0xAA);
        }
        break;
    }
    default:
        break;
    }
    
    if (!scroll->hold_move) {
         scroll->lock_move = false;
         scui_widget_event_scroll_flag(0x01, &scroll->key);
    }
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
        /* 自由布局 */
        #if 0
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
        #endif
        break;
    }
    case scui_event_dir_hor:
    case scui_event_dir_ver: {
        int32_t value_s = anima->value_s;
        int32_t value_e = anima->value_e;
        int32_t value_c = anima->value_c;
        SCUI_LOG_DEBUG("<s:%d, e:%d> - c:%d", value_s, value_e, value_c);
        
        /* 自动布局,非循环,循环 */
        scui_coord_t delta = 0;
        
        if (scroll->dir == scui_event_dir_hor) {
            scroll->point_c.x = value_c;
            delta = scroll->point_c.x - scroll->point_s.x - scroll->dis_sum;
        }
        if (scroll->dir == scui_event_dir_ver) {
            scroll->point_c.y = value_c;
            delta = scroll->point_c.y - scroll->point_s.y - scroll->dis_sum;
        }
        
        SCUI_LOG_DEBUG("dis_ofs:%d", scroll->dis_ofs);
        SCUI_LOG_DEBUG("dis_sum:%d", scroll->dis_sum);
        SCUI_LOG_DEBUG("delta:%d", delta);
        
        if (!scroll->loop) {
            
            scui_coord_t springback_min = +(scroll->springback);
            scui_coord_t springback_max = -(scroll->springback + scroll->dis_lim);
            
            /* 使用偏移范围限制offset,最多允许偏移到回弹点 */
            scui_coord_t dis_ofs_sum = scroll->dis_ofs + scroll->dis_sum + delta;
            SCUI_LOG_DEBUG("dis_ofs_sum:%d", dis_ofs_sum);
            if (dis_ofs_sum > springback_min) {
                if (scroll->dis_ofs + scroll->dis_sum < springback_min)
                    delta = springback_min - (scroll->dis_ofs + scroll->dis_sum);
                else {
                    delta = 0;
                    /* 最后一帧了,可以结束了 */
                    anima->reduce = anima->peroid;
                }
            }
            if (dis_ofs_sum < springback_max) {
                if (scroll->dis_ofs + scroll->dis_sum > springback_max)
                    delta = springback_max - (scroll->dis_ofs + scroll->dis_sum);
                else {
                    delta = 0;
                    /* 最后一帧了,可以结束了 */
                    anima->reduce = anima->peroid;
                }
            }
        }
        
        scroll->dis_sum += delta;
        SCUI_LOG_DEBUG("dis_sum:%d", scroll->dis_sum);
        SCUI_LOG_DEBUG("delta:%d", delta);
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_event_dir_hor)
            offset.x = delta;
        if (scroll->dir == scui_event_dir_ver)
            offset.y = delta;
        SCUI_LOG_DEBUG("<x:%d,y:%d>", offset.x, offset.y);
        
        /* 偏移所有子控件 */
        if (!scroll->loop) {
            
            scui_widget_refr_ofs_child_list(widget->myself, &offset);
        }
        if (scroll->loop) {
            scui_point_t range = {0};
            if (scroll->dir == scui_event_dir_hor)
                range.x = scroll->dis_lim;
            if (scroll->dir == scui_event_dir_ver)
                range.y = scroll->dis_lim;
            scui_widget_refr_ofs_child_list_loop(widget->myself, &offset, &range);
        }
        
        scui_widget_draw(widget->myself, NULL, false);
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
    anima.path    = scui_anima_path_ease_in_out;
    anima.start   = scui_scroll_anima_start;
    anima.ready   = scui_scroll_anima_ready;
    anima.expired = scui_scroll_anima_expired;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.peroid  = peroid != 0 ? peroid : scui_abs(anima.value_e - anima.value_s);
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
    
    SCUI_LOG_WARN("widget: %u", widget->myself);
    
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
    
    scroll->dis_ofs = 0;
    scroll->dis_lim = 0;
    
    if (!scroll->loop) {
    
        /* 空隙只夹杂在控件中 */
        if (scroll->dir == scui_event_dir_hor)
            pos.x -= scroll->space;
        if (scroll->dir == scui_event_dir_ver)
            pos.y -= scroll->space;
    }
    
    /* 不同模式设置不同参数 */
    if (scroll->dir == scui_event_dir_hor) {
        scroll->dis_lim = pos.x - widget->clip.x;
        
        if (!scroll->loop) {
            
            if (scroll->dis_lim >  widget->clip.w)
                scroll->dis_lim -= widget->clip.w;
            else
                scroll->dis_lim = 0;
        }
    }
    if (scroll->dir == scui_event_dir_ver) {
        scroll->dis_lim = pos.y - widget->clip.y;
        
        if (!scroll->loop) {
            
            if (scroll->dis_lim >  widget->clip.h)
                scroll->dis_lim -= widget->clip.h;
            else
                scroll->dis_lim = 0;
        }
    }
    SCUI_LOG_DEBUG("range:[0, %d]", scroll->dis_lim);
    
    if (scui_widget_style_is_show(handle))
        scui_widget_draw(handle, NULL, false);
}

/*@brief 滚动控件事件流程合并
 *@param event 事件
 *@param type  自动事件类型
 *             0x00 动画打断事件(自动布局, 非循环, 循环)
 *             0x01 动画重置事件(自动布局, 非循环, 循环)
 *             0x02 动画重置事件(自动布局, 非循环, 循环)
 *             0xAA 动画回弹事件(自动布局, 非循环, 循环)
 */
void scui_scroll_event_auto_merge(scui_event_t *event, uint8_t type)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (type) {
    case 0x00: {
        SCUI_ASSERT(scroll->dir == scui_event_dir_hor ||
                    scroll->dir == scui_event_dir_ver);
        
        scui_coord_t track = 0;
        scui_coord_t delta_x = event->ptr_e.x - event->ptr_s.x;
        scui_coord_t delta_y = event->ptr_e.y - event->ptr_s.y;
        
        /* 翻页一次以一页为目标 */
        if (event->type == scui_event_ptr_fling) {
            delta_x = delta_x > 0 ? widget->clip.w : -widget->clip.w;
            delta_y = delta_y > 0 ? widget->clip.h : -widget->clip.h;
            delta_x *= scroll->fling_page;
            delta_y *= scroll->fling_page;
        }
        
        scroll->dis_ofs += scroll->dis_sum;
        scroll->dis_sum = 0;
        
        if (!scroll->loop) {
        
            scui_coord_t springback_min = +(scroll->springback);
            scui_coord_t springback_max = -(scroll->springback + scroll->dis_lim);
            
            if (scroll->dis_ofs > springback_min) {
                track = springback_min - scroll->dis_ofs;
                scroll->dis_ofs = springback_min;
                SCUI_LOG_INFO("over limit:%d", track);
            }
            if (scroll->dis_ofs < springback_max) {
                track = springback_max - scroll->dis_ofs;
                scroll->dis_ofs = springback_max;
                SCUI_LOG_INFO("over limit:%d", track);
            }
        }
        
        if (scroll->dir == scui_event_dir_hor) {
            track += scroll->point_e.x - scroll->point_c.x;
            scroll->point_c.x = scroll->point_s.x;
            scroll->point_e.x = delta_x + track;
            scui_scroll_anima_auto(widget->myself, 0, delta_x + track, 0);
        }
        if (scroll->dir == scui_event_dir_ver) {
            track += scroll->point_e.y - scroll->point_c.y;
            scroll->point_c.y = scroll->point_s.y;
            scroll->point_e.y = delta_y + track;
            scui_scroll_anima_auto(widget->myself, 0, delta_y + track, 0);
        }
        break;
    }
    case 0x01:
    case 0x02: {
        SCUI_ASSERT(scroll->dir == scui_event_dir_hor ||
                    scroll->dir == scui_event_dir_ver);
        
        scui_coord_t track = 0;
        
        scroll->dis_ofs += scroll->dis_sum;
        scroll->dis_sum =  0;
        
        if (!scroll->loop) {
        
            scui_coord_t springback_min = +(scroll->springback);
            scui_coord_t springback_max = -(scroll->springback + scroll->dis_lim);
            
            if (scroll->dis_ofs > springback_min) {
                track = springback_min - scroll->dis_ofs;
                scroll->dis_ofs = springback_min;
                SCUI_LOG_INFO("over limit:%d", track);
            }
            if (scroll->dis_ofs < springback_max) {
                track = springback_max - scroll->dis_ofs;
                scroll->dis_ofs = springback_max;
                SCUI_LOG_INFO("over limit:%d", track);
            }
        }
        
        /* 通过一次立即调度触发回弹流程 */
        if (type == 0x02) {
            if (scroll->dir == scui_event_dir_hor)
                track += scroll->point_e.x - scroll->point_c.x;
            if (scroll->dir == scui_event_dir_ver)
                track += scroll->point_e.y - scroll->point_c.y;
            scui_scroll_anima_auto(widget->myself, 0, track, 0);
        }
        scroll->point_s = (scui_point_t){0};
        scroll->point_c = (scui_point_t){0};
        scroll->point_e = (scui_point_t){0};
        break;
    }
    case 0xAA: {
        bool retval = true;
        scui_point_t offset  = {0};
        scui_point_t offset1 = {0};
        scui_point_t offset2 = {0};
        /* 中心点对齐,开始校正 */
        if (scroll->pos == scui_event_pos_c) {
            retval = scui_widget_align_pos_calc(handle, &offset, scui_event_pos_c);
            if (retval) {
                SCUI_ASSERT(!(offset.x != 0 && offset.y != 0));
                
                /* 已经校正完毕,不再校正 */
                if (offset.x == 0 && offset.y == 0)
                    break;
                
                SCUI_ASSERT((scroll->dir == scui_event_dir_hor && offset.x != 0) ||
                            (scroll->dir == scui_event_dir_ver && offset.y != 0));
                
                if (scroll->dir == scui_event_dir_hor)
                    scui_scroll_anima_auto(widget->myself, 0, offset.x, 0);
                if (scroll->dir == scui_event_dir_ver)
                    scui_scroll_anima_auto(widget->myself, 0, offset.y, 0);
            }
            break;
        }
        
        /* 边界对齐,开始校正 */
        if ((scroll->dir == scui_event_dir_hor && (scroll->pos & scui_event_dir_hor) != 0) ||
            (scroll->dir == scui_event_dir_ver && (scroll->pos & scui_event_dir_ver) != 0)) {
            
            /* 不可以同时为多个方向的目标 */
            SCUI_ASSERT(!((scroll->pos & scui_event_dir_hor) != 0 &&
                          (scroll->pos & scui_event_dir_ver) != 0));
            
            if ((scroll->pos & scui_event_pos_l) != 0)
                retval = retval && scui_widget_align_pos_calc(handle, &offset1, scui_event_pos_l);
            if ((scroll->pos & scui_event_pos_r) != 0)
                retval = retval && scui_widget_align_pos_calc(handle, &offset2, scui_event_pos_r);
            
            if ((scroll->pos & scui_event_pos_u) != 0)
                retval = retval && scui_widget_align_pos_calc(handle, &offset1, scui_event_pos_u);
            if ((scroll->pos & scui_event_pos_d) != 0)
                retval = retval && scui_widget_align_pos_calc(handle, &offset2, scui_event_pos_d);
            
            if (retval) {
                /* 取最小偏移量 */
                if ((((scroll->pos & scui_event_pos_l) != 0) || ((scroll->pos & scui_event_pos_u) != 0)))
                      offset = offset1;
                if ((((scroll->pos & scui_event_pos_r) != 0) || ((scroll->pos & scui_event_pos_d) != 0)))
                      offset = offset2;
                
                if ((((scroll->pos & scui_event_pos_l) != 0) && ((scroll->pos & scui_event_pos_r) != 0)) ||
                     ((scroll->pos & scui_event_pos_u) != 0) && ((scroll->pos & scui_event_pos_d) != 0))
                       offset = (offset1.x * offset1.x + offset1.y * offset1.y <
                                 offset2.x * offset2.x + offset2.y * offset2.y) ? offset1 : offset2;
                
                SCUI_ASSERT(!(offset.x != 0 && offset.y != 0));
                
                /* 已经校正完毕,不再校正 */
                if (offset.x == 0 && offset.y == 0)
                    break;
                
                SCUI_ASSERT((scroll->dir == scui_event_dir_hor && offset.x != 0) ||
                            (scroll->dir == scui_event_dir_ver && offset.y != 0));
                
                if (scroll->dir == scui_event_dir_hor)
                    scui_scroll_anima_auto(widget->myself, 0, offset.x, 0);
                if (scroll->dir == scui_event_dir_ver)
                    scui_scroll_anima_auto(widget->myself, 0, offset.y, 0);
            }
            break;
        }
    }
    default:
        SCUI_LOG_ERROR("unknown type: %x", type);
        break;
    }
}

/*@brief 滚动控件事件处理回调
 *@param event 事件
 */
void scui_scroll_event(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_show:
    case scui_event_hide:
    case scui_event_focus_get:
    case scui_event_focus_lost:
        scui_scroll_layout(handle);
        break;
    case scui_event_draw:
        scui_scroll_update_layout(event);
        break;
    case scui_event_ptr_down:
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_ptr_move:
    case scui_event_ptr_fling: {
        
        scui_event_dir_t event_dir = scui_indev_ptr_dir(event);
        SCUI_LOG_INFO("dir:%u", event_dir);
        
        /* 更新切换目标 */
        if (scroll->lock_move) {
            /* 自由布局 */
            if (scroll->dir == scui_event_dir_none) {
                /* ... */
                scui_widget_event_mask_over(event);
                break;
            }
            
            /* 自动布局,非循环,循环 */
            scui_scroll_event_auto_merge(event, 0x00);
            scui_widget_event_mask_over(event);
        } else {
            /* 自由布局 */
            if (scroll->dir == scui_event_dir_none) {
                scroll->lock_move = true;
                scroll->hold_move = true;
                /* ... */
                scui_widget_event_mask_over(event);
                break;
            }
            
            /* 自动布局,非循环,循环 */
            if ((event_dir & scroll->dir) != 0) {
                /* 全局滚动锁定 */
                if (!scui_widget_event_scroll_flag(0x00, &scroll->key))
                     break;
                scroll->lock_move = true;
                scroll->hold_move = true;
                
                scui_scroll_event_auto_merge(event, 0x00);
                scui_widget_event_mask_over(event);
            }
        }
        break;
    }
    case scui_event_ptr_up:
        scui_widget_event_mask_keep(event);
        if (scroll->hold_move) {
            scroll->hold_move = false;
            scroll->mask_springback = false;
            
            /* 自由布局 */
            if (scroll->dir == scui_event_dir_none) {
                /* ... */
                break;
            }
            
            /* 自动布局,非循环,循环 */
            scui_scroll_event_auto_merge(event, 0x02);
        }
        break;
    default:
        break;
    }
}
