/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 内部交叉引用<auto>: */
static void scui_scroll_anima_auto(scui_handle_t handle, int32_t value_s, int32_t value_e, uint32_t period);
static void scui_scroll_event_auto(scui_event_t *event,  uint8_t type);
/* 内部交叉引用: */



/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_scroll_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_scroll_t *scroll = widget;
    scui_scroll_maker_t *scroll_maker = widget_maker;
    
    /* 必须标记widget,ptr事件 */
    widget_maker->style.sched_widget = true;
    widget_maker->style.indev_ptr    = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_scroll));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 状态初始化 */
    scroll->pos         = scroll_maker->pos;
    scroll->dir         = scroll_maker->dir;
    scroll->skip        = scroll_maker->skip;
    scroll->edge        = scroll_maker->edge;
    scroll->space       = scroll_maker->space;
    scroll->springback  = scroll_maker->springback;
    scroll->fling_page  = scroll_maker->fling_page;
    scroll->route_enc   = scroll_maker->route_enc;
    scroll->route_key   = scroll_maker->route_key;
    scroll->keyid_fdir  = scroll_maker->keyid_fdir;
    scroll->keyid_bdir  = scroll_maker->keyid_bdir;
    scroll->freedom     = scroll_maker->freedom;
    scroll->loop        = scroll_maker->loop;
    scroll->layout      = true;
    scroll->over_scroll = true;
    
    /* 偏移点记录 */
    scroll->point_rcd = SCUI_MEM_ALLOC(scui_mem_type_mix, widget->child_num * sizeof(scui_point_t));
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        scroll->point_rcd[idx] = (scui_point_t){0};
    
    /* 设置动画轨迹, 动画速度 */
    scui_map_cb_t anima_path[4] = {0};
    scui_coord_t anima_speed[4] = SCUI_WIDGET_SCROLL_SPD_ANIM;
    
    // 怪怪的感觉，有区别又没有区别
    anima_path[0] = scui_map_ease_in_out;
    anima_path[1] = scui_map_ease_in_out;
    anima_path[2] = scui_map_ease_in_out;
    anima_path[3] = scui_map_ease_in_out;
    
    /* 动画轨迹, 动画速度 */
    for (scui_handle_t idx = 0; idx < 4; idx++) {
        scroll->anima_tag[idx]   = false;
        scroll->anima_path[idx]  = scroll_maker->anima_path[idx];
        scroll->anima_speed[idx] = scroll_maker->anima_speed[idx];
        if (scroll->anima_path[idx]  == NULL)
            scroll->anima_path[idx]   = anima_path[idx];
        if (scroll->anima_speed[idx] == 0)
            scroll->anima_speed[idx]  = anima_speed[idx];
    }
    
    /* 默认保持一个翻页 */
    if (scroll->route_enc == 0) {
        if (scroll->dir == scui_opt_dir_hor)
            scroll->route_enc = widget->clip.w;
        if (scroll->dir == scui_opt_dir_ver)
            scroll->route_enc = widget->clip.h;
    }
    
    /* 默认保持一个翻页 */
    if (scroll->route_key == 0) {
        if (scroll->dir == scui_opt_dir_hor)
            scroll->route_key = widget->clip.w;
        if (scroll->dir == scui_opt_dir_ver)
            scroll->route_key = widget->clip.h;
    }
    
    /* 最低保持一个翻页 */
    if (scroll->fling_page <= 0)
        scroll->fling_page  = 1;
    
    /* 自由布局无循环模式 */
    if (scroll->freedom) {
        scroll->loop = false;
        if (scroll->dir == scui_opt_dir_none)
            scroll->dir  = scui_opt_dir_all;
    }
    
    /* 滚动方向目前只支持三种:水平滚动,垂直滚动,全局滚动 */
    SCUI_ASSERT(scroll->dir == scui_opt_dir_all ||
                scroll->dir == scui_opt_dir_hor ||
                scroll->dir == scui_opt_dir_ver);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_scroll_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    // 回收记录点列表
    SCUI_MEM_FREE(scroll->point_rcd);
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 滚动控件动画标记
 *@param handle 滚动控件句柄
 *@param tag    标记索引
 */
static void scui_scroll_anima_tag(scui_handle_t handle, uint8_t tag)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    for (scui_handle_t idx = 0; idx < 4; idx++)
        scroll->anima_tag[idx] = false;
    
    if (tag < 4)
        scroll->anima_tag[tag] = true;
        scroll->anima_tag[3] = true;
}

/*@brief 滚动控件获取偏移量百分比(自动布局)
 *@param handle 滚动控件句柄
 *@param percent 偏移量
 */
void scui_scroll_percent_get(scui_handle_t handle, scui_coord_t *percent)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    SCUI_ASSERT(percent != NULL);
    *percent = 0;
    
    if (scroll->freedom) {
        if (scroll->dir == scui_opt_dir_hor) {
            scui_coord_t ofs_x = -(scroll->ofs_sum.x + scroll->ofs_cur.x);
            if (ofs_x <= scroll->ofs_min.x) {
               *percent = 0;
                return;
            }
            if (ofs_x >= scroll->ofs_max.x) {
               *percent = 100;
                return;
            }
            *percent = scui_map(ofs_x, scroll->ofs_min.x, scroll->ofs_max.x, 0, 100);
        }
        if (scroll->dir == scui_opt_dir_ver) {
            scui_coord_t ofs_y = -(scroll->ofs_sum.y + scroll->ofs_cur.y);
            if (ofs_y <= scroll->ofs_min.y) {
               *percent = 0;
                return;
            }
            if (ofs_y >= scroll->ofs_max.y) {
               *percent = 100;
                return;
            }
            *percent = scui_map(ofs_y, scroll->ofs_min.y, scroll->ofs_max.y, 0, 100);
        }
    } else {
        scui_coord_t ofs = -(scroll->dis_ofs + scroll->dis_sum);
        if (ofs <= 0) {
           *percent = 0;
            return;
        }
        if (ofs >= scroll->dis_lim) {
           *percent = 100;
            return;
        }
       *percent = scui_map(ofs, 0, scroll->dis_lim, 0, 100);
    }
}

/*@brief 滚动控件中心对齐子控件
 *@param handle 滚动控件句柄
 *@param target 中心对齐子控件
 */
void scui_scroll_center_target_get(scui_handle_t handle, scui_handle_t *target)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    SCUI_ASSERT(scroll->pos == scui_opt_pos_c);
    SCUI_ASSERT(target != NULL);
    *target = SCUI_HANDLE_INVALID;
    
    if (scui_widget_global_scroll_flag(0x02, NULL))
        return;
    
    scui_point_t offset = {0};
    scui_widget_align_pos_calc(handle, target, &offset, scui_opt_pos_c);
}

/*@brief 滚动控件中心对齐子控件
 *@param handle 滚动控件句柄
 *@param target 中心对齐子控件
 */
void scui_scroll_center_target(scui_handle_t handle, scui_handle_t target)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    SCUI_ASSERT(scroll->pos == scui_opt_pos_c);
    SCUI_ASSERT(target != SCUI_HANDLE_INVALID);
    
    bool child_not_find = true;
    scui_widget_child_list_btra(widget, idx)
    if (widget->child_list[idx] == target) {
        child_not_find = false;
        break;
    }
    if (child_not_find) {
        SCUI_LOG_WARN("child not find");
        return;
    }
    
    if (scui_widget_global_scroll_flag(0x02, NULL))
        return;
    
    // 如果已经是中心子控件, 跳过目标
    scui_handle_t center_t = SCUI_HANDLE_INVALID;
    scui_scroll_center_target_get(handle, &center_t);
    if (center_t == target)
        return;
    
    scui_widget_t *widget_c = scui_handle_source_check(target);
    scui_coord_t center_x = widget->clip.x + widget->clip.w / 2;
    scui_coord_t center_y = widget->clip.y + widget->clip.h / 2;
    scui_coord_t center_c_x = widget_c->clip.x + widget_c->clip.w / 2;
    scui_coord_t center_c_y = widget_c->clip.y + widget_c->clip.h / 2;
    
    scui_point_t offset = {
        .x = center_x - center_c_x,
        .y = center_y - center_c_y,
    };
    SCUI_LOG_INFO("offset:<%d, %d>", offset.x, offset.y);
    scui_scroll_anima_tag(handle, 0);
    scui_scroll_offset(handle, &offset);
}

/*@brief 滚动控件获取偏移量(自动布局)
 *@param handle 滚动控件句柄
 *@param offset 偏移量
 */
void scui_scroll_offset_get(scui_handle_t handle, scui_coord_t *offset)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    SCUI_ASSERT(offset != NULL);
    *offset = 0;
    
    if (scroll->freedom) {
        if (scroll->dir == scui_opt_dir_hor) {
            scui_coord_t ofs_x = -(scroll->ofs_sum.x + scroll->ofs_cur.x);
            if (ofs_x <= scroll->ofs_min.x) {
               *offset = scroll->ofs_min.x;
                return;
            }
            if (ofs_x >= scroll->ofs_max.x) {
               *offset = scroll->ofs_max.x;
                return;
            }
            *offset = ofs_x;
        }
        if (scroll->dir == scui_opt_dir_ver) {
            scui_coord_t ofs_y = -(scroll->ofs_sum.y + scroll->ofs_cur.y);
            if (ofs_y <= scroll->ofs_min.y) {
               *offset = scroll->ofs_min.y;
                return;
            }
            if (ofs_y >= scroll->ofs_max.y) {
               *offset = scroll->ofs_max.y;
                return;
            }
            *offset = ofs_y;
        }
    } else {
        scui_coord_t ofs = -(scroll->dis_ofs + scroll->dis_sum);
        if (ofs <= 0) {
            *offset = 0;
             return;
        }
        if (ofs >= scroll->dis_lim) {
           *offset = scroll->dis_lim;
            return;
        }
       *offset = ofs;
    }
}

/*@brief 滚动控件设置偏移量
 *@param handle 滚动控件句柄
 *@param offset 偏移量
 */
void scui_scroll_offset(scui_handle_t handle, scui_point_t *offset)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    /* 全局滚动锁定 */
    if (!scui_widget_global_scroll_flag(0x00, &scroll->key))
         return;
    scroll->lock_move = true;
    scroll->hold_move = false;
    scroll->mask_springback = false;
    
    /* 复用动画即可 */
    scui_event_t event = {0};
    event.object = handle;
    event.type = scui_event_ptr_move;
    event.ptr_e.x = offset->x;
    event.ptr_e.y = offset->y;
    
    uint8_t type = scroll->freedom ? 0x10 : 0x00;
    scui_scroll_event_auto(&event, type);
}

/*@brief 滚动控件边距(自由布局)
 *@param handle 滚动控件句柄
 *@param edge   滚动边距
 */
void scui_scroll_edge(scui_handle_t handle, scui_point_t *edge)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    if (scroll->freedom)
        scroll->edge = *edge;
    
    scroll->layout = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 滚动控件边界忽略检查
 *@param handle 滚动控件句柄
 *@param dir    滚动方向
 */
static bool scui_scroll_edge_skip(scui_handle_t handle, scui_opt_dir_t dir)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    scui_area_t clip_w = widget->clip;
    scui_area_m_to_s(&clip_w,  &clip_w);
    
    // 上边界和左边界:计算第一个子控件与控件相对偏移值
    if (scui_opt_bits_check(scroll->skip, scui_opt_pos_u) ||
        scui_opt_bits_check(scroll->skip, scui_opt_pos_l)) {
        scui_handle_t  handle_c = scui_widget_child_offset(handle, 0, false);
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_area_t clip_c = widget_c->clip;
        scui_area_m_to_s(&clip_c, &clip_c);
        
        if (dir == scui_opt_dir_to_d)
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_u))
        if (clip_w.y1 == clip_c.y1)
            return true;
        
        if (dir == scui_opt_dir_to_r)
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_l))
        if (clip_w.x1 == clip_c.x1)
            return true;
    }
    
    // 下边界和右边界:计算最后一个子控件与控件相对偏移值
    if (scui_opt_bits_check(scroll->skip, scui_opt_pos_d) ||
        scui_opt_bits_check(scroll->skip, scui_opt_pos_r)) {
        scui_handle_t  handle_c = scui_widget_child_offset(handle, 0, true);
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_area_t clip_c = widget_c->clip;
        scui_area_m_to_s(&clip_c, &clip_c);
        
        if (dir == scui_opt_dir_to_u)
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_d))
        if (clip_w.y2 == clip_c.y2)
            return true;
        
        if (dir == scui_opt_dir_to_l)
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_r))
        if (clip_w.x2 == clip_c.x2)
            return true;
    }
    
    return false;
}

/*@brief 滚动控件事件处理回调
 *@param handle 滚动控件句柄
 *@param type   事件类型
 *       0x00   滚动开始事件
 *       0x01   滚动结束事件
 *       0x02   滚动进行事件
 *       0xAA   滚动布局更新事件
 */
static void scui_scroll_notify_alone(scui_handle_t handle, uint8_t type)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    switch (type) {
    case 0x00: {
        /* 上一次滚动已经结束才可产生下一次滚动开始事件 */
        /* 否则为滚动被打断,此时应该吸收中间的事件 */
        if (!scroll->over_scroll) break;
        scroll->over_scroll = false;
        
        /* scroll event: */
        scui_event_t event = {
            .object     = widget->myself,
            .style.sync = true,
            .type       = scui_event_widget_scroll_start,
        };
        scui_event_notify(&event);
        break;
    }
    case 0x01: {
        if (scroll->over_scroll) break;
        scroll->over_scroll = true;
        
        /* scroll event: */
        scui_event_t event = {
            .object     = widget->myself,
            .style.sync = true,
            .type       = scui_event_widget_scroll_over,
        };
        scui_event_notify(&event);
        break;
    }
    case 0x02: {
        /* scroll event: */
        scui_event_t event = {
            .object     = widget->myself,
            .style.sync = true,
            .type       = scui_event_widget_scroll_keep,
            .absorb     = scui_event_absorb_none,
        };
        scui_event_notify(&event);
        break;
    }
    case 0xAA: {
        scui_event_t event = {
            .object     = widget->myself,
            .style.sync = true,
            .type       = scui_event_widget_scroll_layout,
        };
        scui_event_notify(&event);
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type: %x", type);
        break;
    }
}

/*@brief 滚动控件子控件坐标记录
 *@param handle 滚动控件句柄
 *@param record true:记录; false:作用
 */
static void scui_scroll_point_record(scui_handle_t handle, bool record)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        
        if (record) {
            // 此处计算相对偏移量
            // 保存的是子控件相对父控件的偏移量
            scroll->point_rcd[idx].x = widget_c->clip.x - widget->clip.x;
            scroll->point_rcd[idx].y = widget_c->clip.y - widget->clip.y;
        } else {
            scui_point_t point_c = {
                .x = widget->clip.x + scroll->point_rcd[idx].x,
                .y = widget->clip.y + scroll->point_rcd[idx].y,
            };
            scui_widget_move_pos(handle_c, &point_c);
        }
    }
}

/*@brief 滚动控件动画回调
 */
static void scui_scroll_anima_prepare(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 滚动控件动画回调
 */
static void scui_scroll_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_widget_t *widget = scui_handle_source_check(anima->handle);
    scui_scroll_t *scroll = (void *)widget;
    
    int32_t value_s = anima->value_s;
    int32_t value_e = anima->value_e;
    int32_t value_c = anima->value_c;
    SCUI_LOG_DEBUG("<s:%d, e:%d> - c:%d", value_s, value_e, value_c);
    
    if (scroll->freedom) {
        /* 自由布局 */
        scroll->point_cur.x = 0;
        scroll->point_cur.y = 0;
        if (value_e - value_s != 0) {
            scroll->point_cur.x = scui_map(value_c, value_s, value_e, 0, scroll->point_ofs.x);
            scroll->point_cur.y = scui_map(value_c, value_s, value_e, 0, scroll->point_ofs.y);
        }
        
        scui_point_t delta = {
            .x = scroll->point_cur.x - scroll->ofs_sum.x,
            .y = scroll->point_cur.y - scroll->ofs_sum.y,
        };
        
        SCUI_LOG_DEBUG("ofs_cur:<%d,%d>", scroll->ofs_cur.x, scroll->ofs_cur.y);
        SCUI_LOG_DEBUG("ofs_sum:<%d,%d>", scroll->ofs_sum.x, scroll->ofs_sum.y);
        SCUI_LOG_DEBUG("delta:<%d,%d>", delta.x, delta.y);
        
        SCUI_ASSERT(!scroll->loop);
        
        scui_point_t springback_min = {
            .x = +(scroll->springback - scroll->ofs_min.x),
            .y = +(scroll->springback - scroll->ofs_min.y),
        };
        scui_point_t springback_max = {
            .x = -(scroll->springback + scroll->ofs_max.x),
            .y = -(scroll->springback + scroll->ofs_max.y),
        };
        
        /* 使用偏移范围限制offset,最多允许偏移到回弹点 */
        scui_point_t dis_ofs_sum = {
            .x = scroll->ofs_cur.x + scroll->ofs_sum.x + delta.x,
            .y = scroll->ofs_cur.y + scroll->ofs_sum.y + delta.y,
        };
        SCUI_LOG_DEBUG("dis_ofs_sum:<%d,%d>", dis_ofs_sum.x, dis_ofs_sum.y);
        
        if (dis_ofs_sum.x > springback_min.x ||
            dis_ofs_sum.x < springback_max.x)
            delta.x = 0;
        
        if (dis_ofs_sum.x > springback_min.x)
            if (scroll->ofs_cur.x + scroll->ofs_sum.x < springback_min.x)
                delta.x = springback_min.x - (scroll->ofs_cur.x + scroll->ofs_sum.x);
        if (dis_ofs_sum.x < springback_max.x)
            if (scroll->ofs_cur.x + scroll->ofs_sum.x > springback_max.x)
                delta.x = springback_max.x - (scroll->ofs_cur.x + scroll->ofs_sum.x);
        
        if (dis_ofs_sum.y > springback_min.y ||
            dis_ofs_sum.y < springback_max.y)
            delta.y = 0;
        
        if (dis_ofs_sum.y > springback_min.y)
            if (scroll->ofs_cur.y + scroll->ofs_sum.y > springback_min.y)
                delta.y = springback_min.y - (scroll->ofs_cur.y + scroll->ofs_sum.y);
        if (dis_ofs_sum.y < springback_max.y)
            if (scroll->ofs_cur.y + scroll->ofs_sum.y > springback_max.y)
                delta.y = springback_max.y - (scroll->ofs_cur.y + scroll->ofs_sum.y);
        
        /* 最后一帧了,可以结束了 */
        if (delta.x == 0 && delta.y == 0 && value_c != 0)
            anima->reduce = anima->period;
        
        scui_point_t offset = delta;
        scroll->ofs_sum.x += delta.x;
        scroll->ofs_sum.y += delta.y;
        
        SCUI_LOG_DEBUG("ofs_sum:<%d,%d>", scroll->ofs_sum.x, scroll->ofs_sum.y);
        SCUI_LOG_DEBUG("delta:<%d,%d>", delta.x, delta.y);
        SCUI_LOG_DEBUG("offset:<x:%d,y:%d>", offset.x, offset.y);
        
        /* 偏移所有子控件 */
        scui_scroll_point_record(widget->myself, false);
        scui_widget_clist_move_ofs(widget->myself, &offset);
        scui_scroll_point_record(widget->myself, true);
        scui_scroll_notify_alone(widget->myself, 0x02);
        
        scui_widget_draw(widget->myself, NULL, false);
    } else {
        /* 自动布局,非循环,循环 */
        scui_coord_t delta = 0;
        
        if (scroll->dir == scui_opt_dir_hor) {
            scroll->point_cur.x = value_c;
            delta = scroll->point_cur.x - scroll->dis_sum;
        }
        if (scroll->dir == scui_opt_dir_ver) {
            scroll->point_cur.y = value_c;
            delta = scroll->point_cur.y - scroll->dis_sum;
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
            
            if (dis_ofs_sum > springback_min ||
                dis_ofs_sum < springback_max)
                delta = 0;
            
            if (dis_ofs_sum > springback_min)
                if (scroll->dis_ofs + scroll->dis_sum < springback_min)
                    delta = springback_min - (scroll->dis_ofs + scroll->dis_sum);
            if (dis_ofs_sum < springback_max)
                if (scroll->dis_ofs + scroll->dis_sum > springback_max)
                    delta = springback_max - (scroll->dis_ofs + scroll->dis_sum);
        }
        
        /* 最后一帧了,可以结束了 */
        if (delta == 0 && value_c != 0)
            anima->reduce = anima->period;
        
        scroll->dis_sum += delta;
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_opt_dir_hor)
            offset.x = delta;
        if (scroll->dir == scui_opt_dir_ver)
            offset.y = delta;
        
        SCUI_LOG_DEBUG("dis_sum:%d", scroll->dis_sum);
        SCUI_LOG_DEBUG("delta:%d", delta);
        SCUI_LOG_DEBUG("offset:<x:%d,y:%d>", offset.x, offset.y);
        
        /* 偏移所有子控件 */
        if (scroll->loop) {
            scui_point_t range = {0};
            if (scroll->dir == scui_opt_dir_hor)
                range.x = scroll->dis_lim;
            if (scroll->dir == scui_opt_dir_ver)
                range.y = scroll->dis_lim;
            
            scui_scroll_point_record(widget->myself, false);
            scui_widget_clist_move_ofs_loop(widget->myself, &offset, &range);
            scui_scroll_point_record(widget->myself, true);
            scui_scroll_notify_alone(widget->myself, 0x02);
        } else {
            scui_scroll_point_record(widget->myself, false);
            scui_widget_clist_move_ofs(widget->myself, &offset);
            scui_scroll_point_record(widget->myself, true);
            scui_scroll_notify_alone(widget->myself, 0x02);
        }
        
        scui_widget_draw(widget->myself, NULL, false);
    }
}

/*@brief 滚动控件动画回调
 */
static void scui_scroll_anima_finish(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_widget_t *widget = scui_handle_source_check(anima->handle);
    scui_scroll_t *scroll = (void *)widget;
    
    scui_event_t event = {.object = anima->handle};
    
    /* 这里需要考虑回弹效果,包括回弹点,边界对齐 */
    
    if (scroll->freedom) {
        /* 自由布局 */
        SCUI_LOG_DEBUG("ofs_cur:<%d,%d>", scroll->ofs_cur.x, scroll->ofs_cur.y);
        SCUI_LOG_DEBUG("ofs_sum:<%d,%d>", scroll->ofs_sum.x, scroll->ofs_sum.y);
        scui_scroll_event_auto(&event, 0x11);
        
        SCUI_ASSERT(!scroll->loop);
        
        if (!scroll->hold_move) {
            
            /* 到达回弹点,则开始回弹 */
            if (scroll->ofs_cur.x > -scroll->ofs_min.x ||
                scroll->ofs_cur.y > -scroll->ofs_min.y ||
                scroll->ofs_cur.x < -scroll->ofs_max.x ||
                scroll->ofs_cur.y < -scroll->ofs_max.y) {
                 scroll->mask_springback = true;
                 
                if (scroll->ofs_cur.x > -scroll->ofs_min.x)
                    scroll->point_ofs.x = -(scroll->ofs_cur.x + scroll->ofs_min.x);
                if (scroll->ofs_cur.y > -scroll->ofs_min.y)
                    scroll->point_ofs.y = -(scroll->ofs_cur.y + scroll->ofs_min.y);
                
                if (scroll->ofs_cur.x < -scroll->ofs_max.x)
                    scroll->point_ofs.x = -(scroll->ofs_cur.x + scroll->ofs_max.x);
                if (scroll->ofs_cur.y < -scroll->ofs_max.y)
                    scroll->point_ofs.y = -(scroll->ofs_cur.y + scroll->ofs_max.y);
                
                scui_multi_t dist = 0;
                scui_point_t offset = {
                    .x = scroll->ofs_cur.x,
                    .y = scroll->ofs_cur.y,
                };
                dist = offset.x * offset.x + offset.y * offset.y;
                
                int32_t sqrt_i = 0, sqrt_f = 0;
                scui_sqrt(dist, &sqrt_i, &sqrt_f, 0x800);
                scui_scroll_anima_auto(widget->myself, 0, sqrt_i, 0);
                goto over;
            }
            
            /* 进行回弹则不再校正 */
            if (scroll->mask_springback)
                goto over;
            
            /* 未回弹则需要校正 */
            scui_scroll_event_auto(&event, 0xAA);
        }
    } else {
        /* 自动布局,非循环,循环 */
        SCUI_LOG_DEBUG("dis_ofs:%d", scroll->dis_ofs);
        SCUI_LOG_DEBUG("dis_sum:%d", scroll->dis_sum);
        scui_scroll_event_auto(&event, 0x01);
        
        if (!scroll->hold_move) {
            
            if (!scroll->loop) {
                
                /* 到达回弹点,则开始回弹 */
                if (scroll->dis_ofs > +0) {
                    scroll->mask_springback = true;
                    scui_scroll_anima_auto(widget->myself, 0, -scroll->dis_ofs + 0, 0);
                    goto over;
                }
                if (scroll->dis_ofs < -scroll->dis_lim) {
                    scroll->mask_springback = true;
                    scui_scroll_anima_auto(widget->myself, 0, -scroll->dis_ofs - scroll->dis_lim, 0);
                    goto over;
                }
                /* 进行回弹则不再校正 */
                if (scroll->mask_springback)
                    goto over;
            }
            
            if (scroll->dis_lim == 0)
                goto over;
            
            /* 未回弹则需要校正 */
            scui_scroll_event_auto(&event, 0xAA);
        }
    }
    over:
    
    if (!scroll->hold_move) {
         scroll->lock_move = false;
         
        // 仅仅当anima完全结束后,解锁scroll标记
        if (scroll->anima == SCUI_HANDLE_INVALID ||
           !scui_anima_running(scroll->anima))
            scui_widget_global_scroll_flag(0x01, &scroll->key);
        
        if (scroll->anima != SCUI_HANDLE_INVALID &&
           !scui_anima_running(scroll->anima)) {
            
            scui_scroll_notify_alone(widget->myself, 0x01);
            
            if (scroll->anima != SCUI_HANDLE_INVALID) {
                scui_anima_stop(scroll->anima);
                scui_anima_destroy(scroll->anima);
                scroll->anima = SCUI_HANDLE_INVALID;
            }
        }
    }
}

/*@brief 滚动控件动画自动化
 *@param handle  滚动控件句柄
 *@param value_s 起始值
 *@param value_e 结束值
 *@param period  周期值
 */
static void scui_scroll_anima_auto(scui_handle_t handle, int32_t value_s, int32_t value_e, uint32_t period)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    scui_anima_t anima = {0};
    anima.prepare = scui_scroll_anima_prepare;
    anima.expired = scui_scroll_anima_expired;
    anima.finish  = scui_scroll_anima_finish;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.period  = period != 0 ? period : scui_dist(anima.value_s, anima.value_e);
    anima.handle  = handle;
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    // 确定当前动画的路径
    // 计算当前动画的周期
    for (scui_handle_t idx = 0; idx < 4; idx++)
        if (scroll->anima_tag[idx]) {
            anima.path = scroll->anima_path[idx];
            anima.period = anima.period * 1000 / scroll->anima_speed[idx];
            break;
        }
    
    if (scroll->anima == SCUI_HANDLE_INVALID)
        scui_scroll_notify_alone(handle, 0x00);
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_scroll_anima_prepare(&anima);
        scui_scroll_anima_expired(&anima);
        scui_scroll_anima_finish(&anima);
        return;
    }
    scui_anima_create(&anima, &scroll->anima);
    scui_anima_start(scroll->anima);
}

/*@brief 滚动控件事件流程合并
 *@param event 事件
 *@param type  自动事件类型
 *             0x00 动画打断事件(自动布局, 非循环, 循环)
 *             0x01 动画重置事件(自动布局, 非循环, 循环)
 *             0x02 动画重置事件(自动布局, 非循环, 循环)
 *             0x10 动画打断事件(自由布局)
 *             0x11 动画重置事件(自由布局)
 *             0x12 动画重置事件(自由布局)
 *             0xAA 动画回弹事件(自动布局, 自由布局, 非循环, 循环)
 */
static void scui_scroll_event_auto(scui_event_t *event, uint8_t type)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_scroll_t *scroll = (void *)widget;
    
    switch (type) {
    case 0x00: {
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
        scroll->dis_sum  = 0;
        
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
        
        if (scroll->dir == scui_opt_dir_hor) {
            track += scroll->point_ofs.x - scroll->point_cur.x;
            scroll->point_cur.x = 0;
            scroll->point_ofs.x = delta_x + track;
            scui_scroll_anima_auto(widget->myself, 0, delta_x + track, 0);
        }
        if (scroll->dir == scui_opt_dir_ver) {
            track += scroll->point_ofs.y - scroll->point_cur.y;
            scroll->point_cur.y = 0;
            scroll->point_ofs.y = delta_y + track;
            scui_scroll_anima_auto(widget->myself, 0, delta_y + track, 0);
        }
        break;
    }
    case 0x01:
    case 0x02: {
        scui_coord_t track = 0;
        
        scroll->dis_ofs += scroll->dis_sum;
        scroll->dis_sum  = 0;
        
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
            if (scroll->dir == scui_opt_dir_hor)
                track += scroll->point_ofs.x - scroll->point_cur.x;
            if (scroll->dir == scui_opt_dir_ver)
                track += scroll->point_ofs.y - scroll->point_cur.y;
            scui_scroll_anima_auto(widget->myself, 0, track, 0);
        }
        scroll->point_cur = (scui_point_t){0};
        scroll->point_ofs = (scui_point_t){0};
        break;
    }
    case 0x10: {
        scui_point_t track = {0};
        scui_coord_t delta_x = event->ptr_e.x - event->ptr_s.x;
        scui_coord_t delta_y = event->ptr_e.y - event->ptr_s.y;
        
        delta_x = scui_opt_bits_check(scroll->dir, scui_opt_dir_hor) ? delta_x : 0;
        delta_y = scui_opt_bits_check(scroll->dir, scui_opt_dir_ver) ? delta_y : 0;
        
        /* 自由布局的翻页退化为移动 */
        if (event->type == scui_event_ptr_fling);
        
        scroll->ofs_cur.x += scroll->ofs_sum.x;
        scroll->ofs_cur.y += scroll->ofs_sum.y;
        scroll->ofs_sum.x = 0;
        scroll->ofs_sum.y = 0;
        
        SCUI_ASSERT(!scroll->loop);
        
        scui_point_t springback_min = {
            .x = +(scroll->springback - scroll->ofs_min.x),
            .y = +(scroll->springback - scroll->ofs_min.y),
        };
        scui_point_t springback_max = {
            .x = -(scroll->springback + scroll->ofs_max.x),
            .y = -(scroll->springback + scroll->ofs_max.y),
        };
        
        if (scroll->ofs_cur.x > springback_min.x) {
            track.x = springback_min.x - scroll->ofs_cur.x;
            scroll->ofs_cur.x = springback_min.x;
            SCUI_LOG_INFO("over limit:%d", track.x);
        }
        if (scroll->ofs_cur.x < springback_max.x) {
            track.x = springback_max.x - scroll->ofs_cur.x;
            scroll->ofs_cur.x = springback_max.x;
            SCUI_LOG_INFO("over limit:%d", track.x);
        }
        if (scroll->ofs_cur.y > springback_min.y) {
            track.y = springback_min.y - scroll->ofs_cur.y;
            scroll->ofs_cur.y = springback_min.y;
            SCUI_LOG_INFO("over limit:%d", track.y);
        }
        if (scroll->ofs_cur.y < springback_max.y) {
            track.y = springback_max.y - scroll->ofs_cur.y;
            scroll->ofs_cur.y = springback_max.y;
            SCUI_LOG_INFO("over limit:%d", track.y);
        }
        
        track.x += scroll->point_ofs.x - scroll->point_cur.x;
        track.y += scroll->point_ofs.y - scroll->point_cur.y;
        scroll->point_cur.x = 0;
        scroll->point_cur.y = 0;
        scroll->point_ofs.x = delta_x + track.x;
        scroll->point_ofs.y = delta_y + track.y;
        
        scui_multi_t dist = 0;
        scui_point_t offset = {
            .x = delta_x + track.x,
            .y = delta_y + track.y,
        };
        dist = offset.x * offset.x + offset.y * offset.y;
        
        int32_t sqrt_i = 0, sqrt_f = 0;
        scui_sqrt(dist, &sqrt_i, &sqrt_f, 0x800);
        scui_scroll_anima_auto(widget->myself, 0, sqrt_i, 0);
        break;
    }
    case 0x11:
    case 0x12: {
        scui_point_t track = {0};
        scroll->ofs_cur.x += scroll->ofs_sum.x;
        scroll->ofs_cur.y += scroll->ofs_sum.y;
        scroll->ofs_sum.x = 0;
        scroll->ofs_sum.y = 0;
        
        SCUI_ASSERT(!scroll->loop);
        
        scui_point_t springback_min = {
            .x = +(scroll->springback - scroll->ofs_min.x),
            .y = +(scroll->springback - scroll->ofs_min.y),
        };
        scui_point_t springback_max = {
            .x = -(scroll->springback + scroll->ofs_max.x),
            .y = -(scroll->springback + scroll->ofs_max.y),
        };
        
        if (scroll->ofs_cur.x > springback_min.x) {
            track.x = springback_min.x - scroll->ofs_cur.x;
            scroll->ofs_cur.x = springback_min.x;
            SCUI_LOG_INFO("over limit:%d", track.x);
        }
        if (scroll->ofs_cur.x < springback_max.x) {
            track.x = springback_max.x - scroll->ofs_cur.x;
            scroll->ofs_cur.x = springback_max.x;
            SCUI_LOG_INFO("over limit:%d", track.x);
        }
        if (scroll->ofs_cur.y > springback_min.y) {
            track.y = springback_min.y - scroll->ofs_cur.y;
            scroll->ofs_cur.y = springback_min.y;
            SCUI_LOG_INFO("over limit:%d", track.y);
        }
        if (scroll->ofs_cur.y < springback_max.y) {
            track.y = springback_max.y - scroll->ofs_cur.y;
            scroll->ofs_cur.y = springback_max.y;
            SCUI_LOG_INFO("over limit:%d", track.y);
        }
        
        if (type == 0x11) {
            scroll->point_cur = (scui_point_t){0};
            scroll->point_ofs = (scui_point_t){0};
        }
        /* 通过一次立即调度触发回弹流程 */
        if (type == 0x12) {
            track.x += scroll->point_ofs.x - scroll->point_cur.x;
            track.y += scroll->point_ofs.y - scroll->point_cur.y;
            
            scui_multi_t dist = 0;
            scui_point_t offset = {
                .x = track.x,
                .y = track.y,
            };
            dist = offset.x * offset.x + offset.y * offset.y;
            
            int32_t sqrt_i = 0, sqrt_f = 0;
            scui_sqrt(dist, &sqrt_i, &sqrt_f, 0x800);
            scui_scroll_anima_auto(widget->myself, 0, sqrt_i, 0);
        }
        break;
    }
    case 0xAA: {
        bool retval = true;
        scui_point_t offset  = {0};
        /* 中心点对齐,开始校正 */
        if (scroll->pos == scui_opt_pos_c) {
            retval = scui_widget_align_pos_calc(widget->myself, NULL, &offset, scui_opt_pos_c);
            if (retval) {
                /* 已经校正完毕,不再校正 */
                if (offset.x == 0 && offset.y == 0)
                    break;
                
                if (scroll->freedom) {
                    scroll->point_ofs.x = offset.x;
                    scroll->point_ofs.y = offset.y;
                    
                    scui_multi_t dist = offset.x * offset.x + offset.y * offset.y;
                    
                    int32_t sqrt_i = 0, sqrt_f = 0;
                    scui_sqrt(dist, &sqrt_i, &sqrt_f, 0x800);
                    scui_scroll_anima_auto(widget->myself, 0, sqrt_i, 0);
                } else {
                    SCUI_ASSERT(!(offset.x != 0 && offset.y != 0));
                    
                    SCUI_ASSERT((scroll->dir == scui_opt_dir_hor && offset.x != 0) ||
                                (scroll->dir == scui_opt_dir_ver && offset.y != 0));
                    
                    if (scroll->dir == scui_opt_dir_hor)
                        scui_scroll_anima_auto(widget->myself, 0, offset.x, 0);
                    if (scroll->dir == scui_opt_dir_ver)
                        scui_scroll_anima_auto(widget->myself, 0, offset.y, 0);
                }
            }
            break;
        }
        
        scui_coord_t offset_num = 0;
        scui_point_t offset4[4] = {0};
        offset.x = scui_coord_max;
        offset.y = scui_coord_max;
        
        if ((scroll->pos & scui_opt_pos_l) != 0) {
            retval = retval && scui_widget_align_pos_calc(widget->myself, NULL, &offset, scui_opt_pos_l);
            offset4[offset_num++] = offset;
        }
        if ((scroll->pos & scui_opt_pos_r) != 0) {
            retval = retval && scui_widget_align_pos_calc(widget->myself, NULL, &offset, scui_opt_pos_r);
            offset4[offset_num++] = offset;
        }
        if ((scroll->pos & scui_opt_pos_u) != 0) {
            retval = retval && scui_widget_align_pos_calc(widget->myself, NULL, &offset, scui_opt_pos_u);
            offset4[offset_num++] = offset;
        }
        if ((scroll->pos & scui_opt_pos_d) != 0) {
            retval = retval && scui_widget_align_pos_calc(widget->myself, NULL, &offset, scui_opt_pos_d);
            offset4[offset_num++] = offset;
        }
        
        /* 边界对齐,开始校正 */
        if (scroll->dir == scui_opt_dir_all && scroll->pos != scui_opt_dir_none) {
            
            if (retval) {
                /* 取最小偏移量 */
                for (scui_coord_t idx = 0; idx < offset_num; idx++)
                    if (offset.x * offset.x + offset.y * offset.y >
                        offset4[idx].x * offset4[idx].x + offset4[idx].y * offset4[idx].y)
                        offset = offset4[idx];
                
                /* 已经校正完毕,不再校正 */
                if (offset.x == 0 && offset.y == 0)
                    break;
                
                scroll->point_ofs.x = offset.x;
                scroll->point_ofs.y = offset.y;
                
                scui_multi_t dist = offset.x * offset.x + offset.y * offset.y;
                
                int32_t sqrt_i = 0, sqrt_f = 0;
                scui_sqrt(dist, &sqrt_i, &sqrt_f, 0x800);
                scui_scroll_anima_auto(widget->myself, 0, sqrt_i, 0);
            }
        }
        
        /* 边界对齐,开始校正 */
        if ((scroll->dir == scui_opt_dir_hor && scui_opt_bits_check(scroll->pos, scui_opt_dir_hor)) ||
            (scroll->dir == scui_opt_dir_ver && scui_opt_bits_check(scroll->pos, scui_opt_dir_ver))) {
            
            /* 不可以同时为多个方向的目标 */
            SCUI_ASSERT(!(scui_opt_bits_check(scroll->pos, scui_opt_dir_hor) &&
                          scui_opt_bits_check(scroll->pos, scui_opt_dir_ver)));
            
            if (retval) {
                /* 取最小偏移量 */
                for (scui_coord_t idx = 0; idx < offset_num; idx++)
                    if (offset.x * offset.x + offset.y * offset.y >
                        offset4[idx].x * offset4[idx].x + offset4[idx].y * offset4[idx].y)
                        offset = offset4[idx];
                
                SCUI_ASSERT(!(offset.x != 0 && offset.y != 0));
                
                /* 已经校正完毕,不再校正 */
                if (offset.x == 0 && offset.y == 0)
                    break;
                
                SCUI_ASSERT((scroll->dir == scui_opt_dir_hor && offset.x != 0) ||
                            (scroll->dir == scui_opt_dir_ver && offset.y != 0));
                
                if (scroll->dir == scui_opt_dir_hor)
                    scui_scroll_anima_auto(widget->myself, 0, offset.x, 0);
                if (scroll->dir == scui_opt_dir_ver)
                    scui_scroll_anima_auto(widget->myself, 0, offset.y, 0);
            }
            break;
        }
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type: %x", type);
        break;
    }
}

/*@brief 滚动控件更新布局回调
 *@param event 事件
 */
static void scui_scroll_event_layout(scui_event_t *event)
{
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_scroll_t *scroll = (void *)widget;
    
    /* 仅标记布局触发布局更新 */
    if (scroll->layout)
        scroll->layout = false;
    else
        return;
    
    if (widget->child_num == 0)
        return;
    
    SCUI_LOG_INFO("widget: %u", widget->myself);
    
    // 状态量还原
    scroll->point_cur = (scui_point_t){0};
    scroll->point_ofs = (scui_point_t){0};
    
    /* 仅指定布局触发布局更新 */
    /* 粘性布局,与自由布局类似,只需要边界自动对齐即可 */
    if (scroll->freedom) {
        scroll->loop = false;
        
        /* 计算布局内容 */
        scui_area_t clip = widget->clip;
        /* 布局内容为所有子控件的全集 */
        scui_widget_child_list_btra(widget, idx) {
            scui_handle_t  handle_c = widget->child_list[idx];
            scui_widget_t *widget_c = scui_handle_source_check(handle_c);
            scui_area_merge2(&clip, &widget_c->clip);
        }
        scui_area_t clip_w = widget->clip;
        scui_area_m_to_s(&clip, &clip);
        scui_area_m_to_s(&clip_w, &clip_w);
        /* 计算自由布局下的ofs_min和ofs_max */
        scroll->ofs_min.x = -scui_dist(clip_w.x1, clip.x1) - scroll->edge.x;
        scroll->ofs_min.y = -scui_dist(clip_w.y1, clip.y1) - scroll->edge.y;
        scroll->ofs_max.x = +scui_dist(clip_w.x2, clip.x2) + scroll->edge.x;
        scroll->ofs_max.y = +scui_dist(clip_w.y2, clip.y2) + scroll->edge.y;
        SCUI_LOG_DEBUG("ofs_min:<0, %d>", scroll->ofs_min.x, scroll->ofs_min.y);
        SCUI_LOG_DEBUG("ofs_max:<0, %d>", scroll->ofs_max.x, scroll->ofs_max.y);
        
        // 保存子控件的坐标记录
        scui_scroll_point_record(event->object, true);
        scui_scroll_notify_alone(event->object, 0xAA);
        
        // 状态量还原
        scroll->ofs_cur = (scui_point_t){0};
        scroll->ofs_sum = (scui_point_t){0};
        scui_widget_draw(widget->myself, NULL, false);
        return;
    }
    
    // 水平自动布局会调整所有子控件的(x,y,h)
    // 垂直自动布局会调整所有子控件的(x,y,w)
    scui_point_t pos = {
        .x = widget->clip.x,
        .y = widget->clip.y,
    };
    
    /* 根据布局方向调整子控件信息 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        /* 被隐藏的控件忽略 */
        if (scui_widget_is_hide(handle_c))
            continue;
        
        /* 更新子控件尺寸 */
        if (scroll->dir == scui_opt_dir_hor)
            scui_widget_adjust_size(handle_c, widget_c->clip.w, widget->clip.h);
        if (scroll->dir == scui_opt_dir_ver)
            scui_widget_adjust_size(handle_c, widget->clip.w, widget_c->clip.h);
        /* 更新子控件位置 */
        scui_widget_move_pos(handle_c, &pos);
        /* 迭代到下一子控件 */
        if (scroll->dir == scui_opt_dir_hor)
            pos.x += widget_c->clip.w + scroll->space;
        if (scroll->dir == scui_opt_dir_ver)
            pos.y += widget_c->clip.h + scroll->space;
    }
    // 保存子控件的坐标记录
    scui_scroll_point_record(event->object, true);
    scui_scroll_notify_alone(event->object, 0xAA);
    
    // 状态量还原
    scroll->dis_sum = 0;
    scroll->dis_ofs = 0;
    scroll->dis_lim = 0;
    
    if (!scroll->loop) {
    
        /* 空隙只夹杂在控件中 */
        if (scroll->dir == scui_opt_dir_hor)
            pos.x -= scroll->space;
        if (scroll->dir == scui_opt_dir_ver)
            pos.y -= scroll->space;
    }
    
    /* 不同模式设置不同参数 */
    if (scroll->dir == scui_opt_dir_hor) {
        scroll->dis_lim = pos.x - widget->clip.x;
        
        if (!scroll->loop) {
            
            if (scroll->dis_lim >  widget->clip.w)
                scroll->dis_lim -= widget->clip.w;
            else
                scroll->dis_lim = 0;
        }
    }
    if (scroll->dir == scui_opt_dir_ver) {
        scroll->dis_lim = pos.y - widget->clip.y;
        
        if (!scroll->loop) {
            
            if (scroll->dis_lim >  widget->clip.h)
                scroll->dis_lim -= widget->clip.h;
            else
                scroll->dis_lim = 0;
        }
    }
    SCUI_LOG_DEBUG("range:[0, %d]", scroll->dis_lim);
    
    scui_widget_draw(widget->myself, NULL, false);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_scroll_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_scroll_t *scroll = (void *)widget;
    
    switch (event->type) {
    case scui_event_show:
    case scui_event_hide:
    case scui_event_focus_get:
    case scui_event_focus_lost:
    case scui_event_child_nums:
    case scui_event_child_size: {
        scroll->layout = true;
        /* 额外补充一个布局事件 */
        scui_event_t event = {
            .object = widget->myself,
            .type   = scui_event_layout,
            .absorb = scui_event_absorb_none,
        };
        scui_event_notify(&event);
        break;
    }
    case scui_event_layout:
        scui_scroll_event_layout(event);
        break;
    case scui_event_ptr_down:
        break;
    case scui_event_ptr_move:
    case scui_event_ptr_fling: {
        
        scui_opt_dir_t event_dir = scui_indev_ptr_dir(event);
        SCUI_LOG_INFO("dir:%u", event_dir);
        
        // 不匹配的方向不支持
        if (!scui_opt_bits_check(event_dir, scroll->dir))
            break;
        
        // 忽略的方向不支持
        if (scui_scroll_edge_skip(event->object, event_dir))
            break;
        
        if (!scroll->lock_move) {
            /* 全局滚动锁定 */
            if (!scui_widget_global_scroll_flag(0x00, &scroll->key))
                 break;
            
            scroll->lock_move = true;
            scroll->hold_move = true;
            scui_scroll_anima_tag(event->object, 0);
        }
        
        uint8_t type = scroll->freedom ? 0x10 : 0x00;
        scui_scroll_event_auto(event, type);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_ptr_up:
        
        if (scroll->hold_move) {
            scroll->hold_move = false;
            scroll->mask_springback = false;
            
            uint8_t type = scroll->freedom ? 0x12 : 0x02;
            scui_scroll_event_auto(event, type);
            
            scui_scroll_anima_tag(event->object, -1);
        }
        break;
    case scui_event_enc_clockwise:
    case scui_event_enc_clockwise_anti: {
        
        if (scroll->dir != scui_opt_dir_hor &&
            scroll->dir != scui_opt_dir_ver) {
            SCUI_LOG_ERROR("scroll way is unsupport");
            break;
        }
        if (scroll->route_enc == 0) {
            SCUI_LOG_ERROR("route encode is zero");
            break;
        }
        
        scui_coord_t way = 0;
        if (event->type == scui_event_enc_clockwise)
            way = +1;
        if (event->type == scui_event_enc_clockwise_anti)
            way = -1;
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_opt_dir_hor)
            offset.x = way * scroll->route_enc * event->enc_diff;
        if (scroll->dir == scui_opt_dir_ver)
            offset.y = way * scroll->route_enc * event->enc_diff;
        
        scui_scroll_anima_tag(event->object, 1);
        scui_scroll_offset(event->object, &offset);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_key_click: {
        
        if (scroll->dir != scui_opt_dir_hor &&
            scroll->dir != scui_opt_dir_ver) {
            SCUI_LOG_ERROR("scroll way is unsupport");
            break;
        }
        if (scroll->route_key == 0) {
            SCUI_LOG_ERROR("route key is zero");
            break;
        }
        
        if (event->key_id != scroll->keyid_fdir &&
            event->key_id != scroll->keyid_bdir)
            break;
        
        scui_coord_t way = 0;
        if (event->key_id == scroll->keyid_fdir)
            way = +1;
        if (event->key_id == scroll->keyid_bdir)
            way = -1;
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_opt_dir_hor)
            offset.x = way * scroll->route_key;
        if (scroll->dir == scui_opt_dir_ver)
            offset.y = way * scroll->route_key;
        
        scui_scroll_anima_tag(event->object, 2);
        scui_scroll_offset(event->object, &offset);
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
