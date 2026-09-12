/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 内部交叉引用<something>: */
static void scui_scroll_notify_alone(scui_handle_t handle, uint8_t type);
static void scui_scroll_anima_auto(scui_handle_t handle, scui_point_t *point_ofs, bool anima);
static void scui_scroll_event_auto(scui_event_t *event,  uint8_t type);
/* 内部交叉引用: */



/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_scroll_make(void *inst, void *inst_maker, scui_handle_t *handle)
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
    scui_widget_make(widget, widget_maker, handle);
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
    scroll->route_bar   = scroll_maker->route_bar;
    scroll->route_key   = scroll_maker->route_key;
    scroll->keyid_fdir  = scroll_maker->keyid_fdir;
    scroll->keyid_bdir  = scroll_maker->keyid_bdir;
    scroll->freedom     = scroll_maker->freedom;
    scroll->loop        = scroll_maker->loop;
    
    scroll->point_rcd   = SCUI_MEM_ZALLOC(scui_mem_type_mix, widget->child_num * sizeof(scui_point_t));
    scroll->layout      = true;
    scroll->over_scroll = true;
    
    /* 偏移点记录 */
    
    /* 动画类型初始化 */
    scroll->anima_type = 0;
    
    /* 默认保持一个翻页 */
    if (scroll->route_enc == 0) {
        if (scroll->dir == scui_opt_dir_hor)
            scroll->route_enc = widget->clip.w;
        if (scroll->dir == scui_opt_dir_ver)
            scroll->route_enc = widget->clip.h;
    }
    
    /* 默认保持一个翻页 */
    if (scroll->route_bar == 0) {
        if (scroll->dir == scui_opt_dir_hor)
            scroll->route_bar = widget->clip.w;
        if (scroll->dir == scui_opt_dir_ver)
            scroll->route_bar = widget->clip.h;
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
    
    /* 回收记录点列表 */
    SCUI_MEM_FREE(scroll->point_rcd);
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
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
    
    /* 显示位置(统一自由布局与方向布局) */
    scui_point_t ofs = {
        .x = -(scroll->ofs_cur.x + scroll->ofs_sum.x),
        .y = -(scroll->ofs_cur.y + scroll->ofs_sum.y),
    };
    
    if (scroll->dir == scui_opt_dir_hor) {
        if (ofs.x <= scroll->ofs_min.x) {
           *percent = 0;
            return;
        }
        if (ofs.x >= scroll->ofs_max.x) {
           *percent = 100;
            return;
        }
        *percent = scui_map(ofs.x, scroll->ofs_min.x, scroll->ofs_max.x, 0, 100);
    }
    if (scroll->dir == scui_opt_dir_ver) {
        if (ofs.y <= scroll->ofs_min.y) {
           *percent = 0;
            return;
        }
        if (ofs.y >= scroll->ofs_max.y) {
           *percent = 100;
            return;
        }
        *percent = scui_map(ofs.y, scroll->ofs_min.y, scroll->ofs_max.y, 0, 100);
    }
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
    
    /* 显示位置(统一自由布局与方向布局) */
    scui_point_t ofs = {
        .x = -(scroll->ofs_cur.x + scroll->ofs_sum.x),
        .y = -(scroll->ofs_cur.y + scroll->ofs_sum.y),
    };
    
    if (scroll->dir == scui_opt_dir_hor) {
        if (ofs.x <= scroll->ofs_min.x) {
           *offset = scroll->ofs_min.x;
            return;
        }
        if (ofs.x >= scroll->ofs_max.x) {
           *offset = scroll->ofs_max.x;
            return;
        }
        *offset = ofs.x;
    }
    if (scroll->dir == scui_opt_dir_ver) {
        if (ofs.y <= scroll->ofs_min.y) {
           *offset = scroll->ofs_min.y;
            return;
        }
        if (ofs.y >= scroll->ofs_max.y) {
           *offset = scroll->ofs_max.y;
            return;
        }
        *offset = ofs.y;
    }
}

/*@brief 滚动控件设置偏移量
 *@param handle 滚动控件句柄
 *@param offset 偏移量
 *@param anima  动画模式
 */
void scui_scroll_offset(scui_handle_t handle, scui_point_t *offset, bool anima)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scroll->lock_move = false;
    scroll->mask_springback = false;
    
    /* 进行一次布局更新 */
    scui_event_define(event, handle, true, scui_event_layout, NULL);
    scui_event_notify(&event);
    
    /* 复用动画即可 */
    scui_event_t event_t = {0};
    event_t.object  = handle;
    event_t.type    = scui_event_ptr_move;
    event_t.ptr_e.x = offset->x;
    event_t.ptr_e.y = offset->y;
    
    if (scroll->anima == SCUI_HANDLE_INVALID) {
        scui_scroll_notify_alone(handle, 0x00);
    }
    
    uint8_t type = 0;
    if (anima) type = 0x00;
    else type = 0x0A;
    scui_scroll_event_auto(&event_t, type);
}

/*@brief 滚动控件中心对齐子控件
 *@param handle 滚动控件句柄
 *@param target 中心对齐子控件
 */
void scui_scroll_center_get(scui_handle_t handle, scui_handle_t *target)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_scroll));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    SCUI_ASSERT(target != NULL);
    *target = SCUI_HANDLE_INVALID;
    
    scui_point_t offset = {0};
    scui_widget_align_pos_calc(handle, target, &offset, scui_opt_pos_c);
}

/*@brief 滚动控件中心对齐子控件
 *@param handle 滚动控件句柄
 *@param target 中心对齐子控件
 *@param anima  动画模式
 */
void scui_scroll_center(scui_handle_t handle, scui_handle_t target, bool anima)
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
    
    /* 如果已经是中心子控件, 跳过目标 */
    scui_handle_t handle_c = SCUI_HANDLE_INVALID;
    scui_scroll_center_get(handle, &handle_c);
    if (handle_c == target)
        return;
    
    /* 主动布局一次 */
    scui_event_define(event, handle, true, scui_event_layout, NULL);
    scui_event_notify(&event);
    
    scui_area_t  clip_t   = scui_widget_clip(target);
    scui_point_t center_w = scui_area_center(&widget->clip);
    scui_point_t center_t = scui_area_center(&clip_t);
    
    scui_point_t offset = {
        .x = center_w.x - center_t.x,
        .y = center_w.y - center_t.y,
    };
    SCUI_LOG_INFO("offset:<%d, %d>", offset.x, offset.y);
    ((scui_scroll_t *)widget)->anima_type = 0;
    scui_scroll_offset(handle, &offset, anima);
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
    scui_widget_draw(handle, NULL, false, 0);
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
    scui_area_m_to_s(&clip_w, &clip_w);
    
    /* 不处理循环模式 */
    if (scroll->loop)
        return false;
    
    /* 上边界和左边界:计算第一个子控件与控件相对偏移值 */
    if (scui_opt_bits_check(scroll->skip, scui_opt_pos_u) ||
        scui_opt_bits_check(scroll->skip, scui_opt_pos_l)) {
        scui_handle_t  index_h = 0;
        scui_handle_t handle_h = scui_widget_child_by_index(handle, index_h);
        scui_area_t clip_c = scui_widget_clip(handle_h);
        clip_c.x = scroll->point_rcd[index_h].x;
        clip_c.y = scroll->point_rcd[index_h].y;
        scui_area_m_to_s(&clip_c, &clip_c);
        
        if (scui_opt_bits_check(dir, scui_opt_dir_utd))
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_u))
        if (clip_w.y1 == clip_c.y1)
            return true;
        
        if (scui_opt_bits_check(dir, scui_opt_dir_ltr))
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_l))
        if (clip_w.x1 == clip_c.x1)
            return true;
    }
    
    /* 下边界和右边界:计算最后一个子控件与控件相对偏移值 */
    if (scui_opt_bits_check(scroll->skip, scui_opt_pos_d) ||
        scui_opt_bits_check(scroll->skip, scui_opt_pos_r)) {
        scui_handle_t  index_t = scui_widget_child_now(handle) - 1;
        scui_handle_t handle_t = scui_widget_child_by_index(handle, index_t);
        scui_area_t clip_c = scui_widget_clip(handle_t);
        clip_c.x = scroll->point_rcd[index_t].x;
        clip_c.y = scroll->point_rcd[index_t].y;
        scui_area_m_to_s(&clip_c, &clip_c);
        
        if (scui_opt_bits_check(dir, scui_opt_dir_dtu))
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_d))
        if (clip_w.y2 == clip_c.y2)
            return true;
        
        if (scui_opt_bits_check(dir, scui_opt_dir_rtl))
        if (scui_opt_bits_check(scroll->skip, scui_opt_pos_r))
        if (clip_w.x2 == clip_c.x2)
            return true;
    }
    
    return false;
}

/*@brief 滚动控件事件处理回调
 *@param handle 滚动控件句柄
 *@param type   滚动事件类型(0x00   :开始;0x01:结束;0x02:进行;0xAA:布局更新;)
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
        scui_event_define(event, widget->myself, true, scui_event_scroll_start, NULL);
        scui_event_notify(&event);
        break;
    }
    case 0x01: {
        if (scroll->over_scroll) break;
        scroll->over_scroll = true;
        
        /* scroll event: */
        scui_event_define(event, widget->myself, true, scui_event_scroll_over, NULL);
        scui_event_notify(&event);
        break;
    }
    case 0x02: {
        /* scroll event: */
        scui_event_define(event, widget->myself, true, scui_event_scroll_keep, NULL);
        scui_event_notify(&event);
        break;
    }
    case 0xAA: {
        scui_event_define(event, widget->myself, true, scui_event_scroll_layout, NULL);
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
            /* 此处计算相对偏移量 */
            /* 保存的是子控件相对父控件的偏移量 */
            scroll->point_rcd[idx].x = widget_c->clip.x - widget->clip.x;
            scroll->point_rcd[idx].y = widget_c->clip.y - widget->clip.y;
        } else {
            scui_point_t point_c = {
                .x = widget->clip.x + scroll->point_rcd[idx].x,
                .y = widget->clip.y + scroll->point_rcd[idx].y,
            };
            scui_widget_move_pos(handle_c, &point_c, true);
        }
    }
}

/*@brief 滚动控件动画回调
 */
static void scui_scroll_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 滚动控件动画回调
 */
static void scui_scroll_anima_expire(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_widget_t *widget = scui_handle_source_check(anima->object);
    scui_scroll_t *scroll = (void *)widget;
    
    int32_t value_s = anima->value_s;
    int32_t value_e = anima->value_e;
    int32_t value_c = anima->value_c;
    SCUI_LOG_DEBUG("<s:%d, e:%d> - c:%d", value_s, value_e, value_c);
    
    /* 步骤1: 动画值(模长)映射到偏移向量(统一自由布局与方向布局) */
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
    
    /* 步骤2: 边界限制(非循环,最多允许偏移到回弹点) */
    if (!scroll->loop) {
        
        scui_point_t springback_min = {
            .x = scroll->ofs_min.x - scroll->springback,
            .y = scroll->ofs_min.y - scroll->springback,
        };
        scui_point_t springback_max = {
            .x = scroll->ofs_max.x + scroll->springback,
            .y = scroll->ofs_max.y + scroll->springback,
        };
        
        /* 使用偏移范围限制offset,最多允许偏移到回弹点 */
        scui_point_t dis_ofs_sum = {
            .x = scroll->ofs_cur.x + scroll->ofs_sum.x + delta.x,
            .y = scroll->ofs_cur.y + scroll->ofs_sum.y + delta.y,
        };
        scui_point_t ofs = {
            .x = -dis_ofs_sum.x,
            .y = -dis_ofs_sum.y,
        };
        SCUI_LOG_DEBUG("dis_ofs_sum:<%d,%d>", dis_ofs_sum.x, dis_ofs_sum.y);
        
        if (ofs.x < springback_min.x ||
            ofs.x > springback_max.x)
            delta.x = 0;
        
        if (ofs.x < springback_min.x)
            if (scroll->ofs_cur.x + scroll->ofs_sum.x < -springback_min.x)
                delta.x = -springback_min.x - (scroll->ofs_cur.x + scroll->ofs_sum.x);
        if (ofs.x > springback_max.x)
            if (scroll->ofs_cur.x + scroll->ofs_sum.x > -springback_max.x)
                delta.x = -springback_max.x - (scroll->ofs_cur.x + scroll->ofs_sum.x);
        
        if (ofs.y < springback_min.y ||
            ofs.y > springback_max.y)
            delta.y = 0;
        
        if (ofs.y < springback_min.y)
            if (scroll->ofs_cur.y + scroll->ofs_sum.y < -springback_min.y)
                delta.y = -springback_min.y - (scroll->ofs_cur.y + scroll->ofs_sum.y);
        if (ofs.y > springback_max.y)
            if (scroll->ofs_cur.y + scroll->ofs_sum.y > -springback_max.y)
                delta.y = -springback_max.y - (scroll->ofs_cur.y + scroll->ofs_sum.y);
    }
    
    /* 最后一帧了,可以结束了 */
    if (delta.x == 0 && delta.y == 0 && value_c != 0)
        anima->reduce = anima->period;
    
    scroll->ofs_sum.x += delta.x;
    scroll->ofs_sum.y += delta.y;
    
    SCUI_LOG_DEBUG("ofs_sum:<%d,%d>", scroll->ofs_sum.x, scroll->ofs_sum.y);
    SCUI_LOG_DEBUG("delta:<%d,%d>", delta.x, delta.y);
    
    /* 偏移所有子控件 */
    if (scroll->loop) {
        scui_point_t range = {
            .x = scroll->ofs_max.x,
            .y = scroll->ofs_max.y,
        };
        scui_scroll_point_record(widget->myself, false);
        scui_widget_clist_move_ofs_loop(widget->myself, &delta, &range);
        scui_scroll_point_record(widget->myself, true);
        scui_scroll_notify_alone(widget->myself, 0x02);
    } else {
        scui_scroll_point_record(widget->myself, false);
        scui_widget_clist_move_ofs(widget->myself, &delta);
        scui_scroll_point_record(widget->myself, true);
        scui_scroll_notify_alone(widget->myself, 0x02);
    }
    
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 滚动控件动画回调
 */
static void scui_scroll_anima_finish(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_widget_t *widget = scui_handle_source_check(anima->object);
    scui_scroll_t *scroll = (void *)widget;
    
    scui_event_t event = {.object = anima->object};
    
    /* 这里需要考虑回弹效果,包括回弹点,边界对齐 */
    
    /* 步骤1: 提交动画偏移(统一自由布局与方向布局) */
    scui_scroll_event_auto(&event, 0x01);
    
    if (scroll->lock_move)
        goto over;
    
    if (!scroll->loop) {
        
        /* 显示位置(滚动体偏移取负,统一边界域) */
        scui_point_t ofs = {
            .x = -(scroll->ofs_cur.x),
            .y = -(scroll->ofs_cur.y),
        };
        
        /* 到达回弹点,则开始回弹 */
        if (ofs.x < scroll->ofs_min.x || ofs.y < scroll->ofs_min.y) {
            scroll->mask_springback = true;
            
            scui_point_t point_ofs = {
                .x = ofs.x < scroll->ofs_min.x ? ofs.x - scroll->ofs_min.x : 0,
                .y = ofs.y < scroll->ofs_min.y ? ofs.y - scroll->ofs_min.y : 0,
            };
            scui_scroll_anima_auto(widget->myself, &point_ofs, true);
            goto over;
        }
        if (ofs.x > scroll->ofs_max.x || ofs.y > scroll->ofs_max.y) {
            scroll->mask_springback = true;
            
            scui_point_t point_ofs = {
                .x = ofs.x > scroll->ofs_max.x ? ofs.x - scroll->ofs_max.x : 0,
                .y = ofs.y > scroll->ofs_max.y ? ofs.y - scroll->ofs_max.y : 0,
            };
            scui_scroll_anima_auto(widget->myself, &point_ofs, true);
            goto over;
        }
        /* 进行回弹则不再校正 */
        if (scroll->mask_springback)
            goto over;
        
        /* 无内容可滚(方向布局) */
        if (!scroll->freedom && scroll->ofs_max.x == 0 && scroll->ofs_max.y == 0)
            goto over;
    }
    
    /* 未回弹则需要校正(循环模式也校正) */
    scui_scroll_event_auto(&event, 0xAA);
    over:
    
    if (!scroll->lock_move) {
        
        if (scroll->anima != SCUI_HANDLE_INVALID && !scui_anima_running(scroll->anima)) {
            
            scui_anima_stop(scroll->anima);
            scui_anima_destroy(scroll->anima);
            scroll->anima = SCUI_HANDLE_INVALID;
        }
        if (scroll->anima == SCUI_HANDLE_INVALID) {
            scui_scroll_notify_alone(widget->myself, 0x01);
        }
    }
}

/*@brief 滚动控件动画自动化
 *@param handle    滚动控件句柄
 *@param point_ofs 目标偏移向量(滚动体增量)
 *@param anima     动画模式
 */
static void scui_scroll_anima_auto(scui_handle_t handle, scui_point_t *point_ofs, bool anima)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_scroll_t *scroll = (void *)widget;
    
    scui_anima_t anima_t = {0};
    anima_t.ready   = scui_scroll_anima_ready;
    anima_t.expire  = scui_scroll_anima_expire;
    anima_t.finish  = scui_scroll_anima_finish;
    anima_t.object  = handle;
    
    /* 目标偏移向量(滚动体增量) */
    scroll->point_ofs.x = point_ofs->x;
    scroll->point_ofs.y = point_ofs->y;
    
    /* 动画值使用偏移模长(统一自由布局双轴与方向布局单轴) */
    scui_multi_t dist = 0;
    dist = point_ofs->x * point_ofs->x + point_ofs->y * point_ofs->y;
    int32_t sqrt_i = 0, sqrt_f = 0;
    scui_sqrt(dist, &sqrt_i, &sqrt_f, 0x800);
    anima_t.value_s = 0;
    anima_t.value_e = sqrt_i;
    anima_t.period  = sqrt_i;
    
    SCUI_LOG_INFO("<%d, %d>", point_ofs->x, point_ofs->y);
    
    /* 确定当前动画的路径 */
    /* 计算当前动画的周期 */
    scui_coord_t anima_speed[] = SCUI_WIDGET_SCROLL_SPD_ANIM;
    anima_t.period = anima_t.period * 1000 / anima_speed[scroll->anima_type];
    anima_t.path = scui_map_linear;
    
    switch (scroll->anima_type){
    case 0: anima_t.path = scui_map_linear;   break;
    case 1: anima_t.path = scui_map_linear;   break;
    case 2: anima_t.path = scui_map_linear;   break;
    case 3: anima_t.path = scui_map_linear;   break;
    case 4: anima_t.path = scui_map_ease_out; break;
    }
    
    if (scroll->anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scroll->anima);
        scui_anima_destroy(scroll->anima);
        scroll->anima = SCUI_HANDLE_INVALID;
    }
    if (!anima || (point_ofs->x == 0 && point_ofs->y == 0)) {
        /* 直接到达,立即完成 */
        anima_t.value_c = anima_t.value_e;
        scui_scroll_anima_ready(&anima_t);
        scui_scroll_anima_expire(&anima_t);
        scui_scroll_anima_finish(&anima_t);
        return;
    }
    scui_anima_create(&anima_t, &scroll->anima);
    scui_anima_start(scroll->anima);
}

/*@brief 滚动控件事件流程合并
 *@param event 事件
 *@param type  自动事件类型(自动布局与自由布局统一)
 *             0x00 动画打断事件(非循环, 循环)
 *             0x01 动画重置事件(非循环, 循环)
 *             0x02 动画重置事件(非循环, 循环)
 *             0x0A 直接到达事件(非循环, 循环)
 *             0xAA 动画回弹事件(非循环, 循环)
 */
static void scui_scroll_event_auto(scui_event_t *event, uint8_t type)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_scroll_t *scroll = (void *)widget;
    
    switch (type) {
    case 0x0A:
    case 0x00: {
        /* 动画打断(0x00)/直接到达(0x0A): 显示位置夹取到回弹点 */
        scui_point_t track = {0};
        scui_coord_t delta_x = event->ptr_e.x - event->ptr_s.x;
        scui_coord_t delta_y = event->ptr_e.y - event->ptr_s.y;
        
        /* 方向屏蔽(方向布局仅单轴) */
        if (scroll->dir == scui_opt_dir_hor) delta_y = 0;
        if (scroll->dir == scui_opt_dir_ver) delta_x = 0;
        
        /* 翻页一次以一页为目标(仅自动布局,自由布局翻页退化为移动) */
        if (event->type == scui_event_ptr_fling && !scroll->freedom) {
            delta_x = delta_x > 0 ? widget->clip.w : -widget->clip.w;
            delta_y = delta_y > 0 ? widget->clip.h : -widget->clip.h;
            delta_x *= scroll->fling_page;
            delta_y *= scroll->fling_page;
        }
        
        scroll->ofs_cur.x += scroll->ofs_sum.x;
        scroll->ofs_cur.y += scroll->ofs_sum.y;
        scroll->ofs_sum.x = 0;
        scroll->ofs_sum.y = 0;
        
        if (!scroll->loop) {
            
            scui_point_t springback_min = {
                .x = scroll->ofs_min.x - scroll->springback,
                .y = scroll->ofs_min.y - scroll->springback,
            };
            scui_point_t springback_max = {
                .x = scroll->ofs_max.x + scroll->springback,
                .y = scroll->ofs_max.y + scroll->springback,
            };
            
            /* 显示位置夹取到回弹点 */
            scui_point_t ofs = {
                .x = -(scroll->ofs_cur.x),
                .y = -(scroll->ofs_cur.y),
            };
            if (ofs.x < springback_min.x) {
                track.x = ofs.x - springback_min.x;
                ofs.x = springback_min.x;
                SCUI_LOG_INFO("over limit:%d", track.x);
            }
            if (ofs.x > springback_max.x) {
                track.x = ofs.x - springback_max.x;
                ofs.x = springback_max.x;
                SCUI_LOG_INFO("over limit:%d", track.x);
            }
            if (ofs.y < springback_min.y) {
                track.y = ofs.y - springback_min.y;
                ofs.y = springback_min.y;
                SCUI_LOG_INFO("over limit:%d", track.y);
            }
            if (ofs.y > springback_max.y) {
                track.y = ofs.y - springback_max.y;
                ofs.y = springback_max.y;
                SCUI_LOG_INFO("over limit:%d", track.y);
            }
            scroll->ofs_cur.x = -ofs.x;
            scroll->ofs_cur.y = -ofs.y;
        }
        
        /* 动画剩余 + 本次位移 */
        track.x += scroll->point_ofs.x - scroll->point_cur.x + delta_x;
        track.y += scroll->point_ofs.y - scroll->point_cur.y + delta_y;
        
        scui_scroll_anima_auto(widget->myself, &track, type == 0x00);
        scroll->point_cur = (scui_point_t){0};
        break;
    }
    case 0x01:
    case 0x02: {
        /* 动画重置(0x01)/回弹重置(0x02): 显示位置夹取到回弹点 */
        scui_point_t track = {0};
        
        scroll->ofs_cur.x += scroll->ofs_sum.x;
        scroll->ofs_cur.y += scroll->ofs_sum.y;
        scroll->ofs_sum.x = 0;
        scroll->ofs_sum.y = 0;
        
        if (!scroll->loop) {
            
            scui_point_t springback_min = {
                .x = scroll->ofs_min.x - scroll->springback,
                .y = scroll->ofs_min.y - scroll->springback,
            };
            scui_point_t springback_max = {
                .x = scroll->ofs_max.x + scroll->springback,
                .y = scroll->ofs_max.y + scroll->springback,
            };
            
            /* 显示位置夹取到回弹点 */
            scui_point_t ofs = {
                .x = -(scroll->ofs_cur.x),
                .y = -(scroll->ofs_cur.y),
            };
            if (ofs.x < springback_min.x) {
                track.x = ofs.x - springback_min.x;
                ofs.x = springback_min.x;
                SCUI_LOG_INFO("over limit:%d", track.x);
            }
            if (ofs.x > springback_max.x) {
                track.x = ofs.x - springback_max.x;
                ofs.x = springback_max.x;
                SCUI_LOG_INFO("over limit:%d", track.x);
            }
            if (ofs.y < springback_min.y) {
                track.y = ofs.y - springback_min.y;
                ofs.y = springback_min.y;
                SCUI_LOG_INFO("over limit:%d", track.y);
            }
            if (ofs.y > springback_max.y) {
                track.y = ofs.y - springback_max.y;
                ofs.y = springback_max.y;
                SCUI_LOG_INFO("over limit:%d", track.y);
            }
            scroll->ofs_cur.x = -ofs.x;
            scroll->ofs_cur.y = -ofs.y;
        }
        
        /* 通过一次立即调度触发回弹流程 */
        if (type == 0x02) {
            track.x += scroll->point_ofs.x - scroll->point_cur.x;
            track.y += scroll->point_ofs.y - scroll->point_cur.y;
            scui_scroll_anima_auto(widget->myself, &track, true);
        }
        scroll->point_cur = (scui_point_t){0};
        scroll->point_ofs = (scui_point_t){0};
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
                    scui_scroll_anima_auto(widget->myself, &offset, true);
                } else {
                    SCUI_ASSERT(!(offset.x != 0 && offset.y != 0));
                    
                    SCUI_ASSERT((scroll->dir == scui_opt_dir_hor && offset.x != 0) ||
                                (scroll->dir == scui_opt_dir_ver && offset.y != 0));
                    
                    scui_scroll_anima_auto(widget->myself, &offset, true);
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
                scui_scroll_anima_auto(widget->myself, &offset, true);
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
                
                scui_scroll_anima_auto(widget->myself, &offset, true);
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
    
    SCUI_LOG_INFO("widget: %u", widget->myself);
    
    /* 状态量还原 */
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
        
        /* 保存子控件的坐标记录 */
        scui_scroll_point_record(event->object, true);
        
        /* 状态量还原 */
        scroll->ofs_cur = (scui_point_t){0};
        scroll->ofs_sum = (scui_point_t){0};
        scui_widget_draw(widget->myself, NULL, false, 0);
        return;
    }
    
    /* 水平自动布局会调整所有子控件的(x,y,h) */
    /* 垂直自动布局会调整所有子控件的(x,y,w) */
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
        scui_widget_move_pos(handle_c, &pos, true);
        /* 迭代到下一子控件 */
        if (scroll->dir == scui_opt_dir_hor)
            pos.x += widget_c->clip.w + scroll->space;
        if (scroll->dir == scui_opt_dir_ver)
            pos.y += widget_c->clip.h + scroll->space;
    }
    /* 保存子控件的坐标记录 */
    scui_scroll_point_record(event->object, true);
    
    /* 状态量还原 */
    scroll->ofs_cur = (scui_point_t){0};
    scroll->ofs_sum = (scui_point_t){0};
    
    if (!scroll->loop) {
    
        /* 空隙只夹杂在控件中 */
        if (scroll->dir == scui_opt_dir_hor)
            pos.x -= scroll->space;
        if (scroll->dir == scui_opt_dir_ver)
            pos.y -= scroll->space;
    }
    
    /* 统一边界(显示位置域,方向布局起点为0) */
    scroll->ofs_min = (scui_point_t){0};
    if (scroll->dir == scui_opt_dir_hor) {
        scroll->ofs_max.x = pos.x - widget->clip.x;
        scroll->ofs_max.y = 0;
        
        if (!scroll->loop) {
            
            if (scroll->ofs_max.x >  widget->clip.w)
                scroll->ofs_max.x -= widget->clip.w;
            else
                scroll->ofs_max.x = 0;
        }
    }
    if (scroll->dir == scui_opt_dir_ver) {
        scroll->ofs_max.x = 0;
        scroll->ofs_max.y = pos.y - widget->clip.y;
        
        if (!scroll->loop) {
            
            if (scroll->ofs_max.y >  widget->clip.h)
                scroll->ofs_max.y -= widget->clip.h;
            else
                scroll->ofs_max.y = 0;
        }
    }
    SCUI_LOG_DEBUG("range:[0, %d]", scroll->ofs_max.x + scroll->ofs_max.y);
    
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_scroll_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_scroll_t *scroll = (void *)widget;
    
    /* 无子控件不响应事件 */
    if (widget->child_now == 0)
        return;
    
    switch (event->type) {
    case scui_event_child_num:
    case scui_event_child_size:
        scroll->layout = true;
        break;
    case scui_event_layout: {
        /* 特殊控件:禁止自动宽高 */
        SCUI_ASSERT(!widget->state.layout_w);
        SCUI_ASSERT(!widget->state.layout_h);
        
        bool layout = scroll->layout;
        scui_coord_t offset_t = 0; scui_point_t offset = {0};
        if (layout) scui_scroll_offset_get(event->object, &offset_t);
        
        scui_scroll_event_layout(event);
        if (layout) scui_scroll_notify_alone(event->object, 0xAA);
        
        if (scroll->dir == scui_opt_dir_hor) offset.x = -offset_t;
        if (scroll->dir == scui_opt_dir_ver) offset.y = -offset_t;
        if (layout) scui_scroll_offset(event->object, &offset, false);
        /* if (layout) SCUI_LOG_WARN("<%04d>", offset_t); */
        break;
    }
    case scui_event_ptr_down:
        break;
    case scui_event_ptr_hold:
        if (widget->state.indev_hold)
            scui_event_mask_over(event);
        break;
    case scui_event_ptr_move:
    case scui_event_ptr_fling: {
        
        /* 忽略的方向不支持 */
        SCUI_LOG_INFO("dir:%u", event->ptr_dir);
        if (scui_scroll_edge_skip(event->object, event->ptr_dir))
            break;
        
        if (!scroll->lock_move) {
            
            /* 不匹配的方向不支持 */
            if (!scui_opt_bits_check(event->ptr_dir, scroll->dir))
                break;
            
            scroll->lock_move = true;
            scroll->anima_type = 0;
            widget->state.indev_hold = true;
            
            if (scroll->anima == SCUI_HANDLE_INVALID) {
                scui_scroll_notify_alone(event->object, 0x00);
            }
        }
        
        uint8_t type = 0x00;
        
        scroll->speed_move = 0;
        if (event->type == scui_event_ptr_move) {
            if (scroll->dir == scui_opt_dir_hor) scroll->speed_move = scui_abs(event->ptr_v);
            if (scroll->dir == scui_opt_dir_ver) scroll->speed_move = scui_abs(event->ptr_v);
        }
        
        scui_scroll_event_auto(event, type);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_ptr_up: {
        
        if (scroll->lock_move) {
            scroll->lock_move = false;
            scroll->mask_springback = false;
            
            uint8_t type = 0x02;
            
            /* 惯性滑行: 速度×惯性系数/SCALE_COF=惯性距离 */
            if (SCUI_INDEV_PTR_INERTIA != 0 && scroll->speed_move != 0) {
                scui_multi_t dist = scroll->speed_move * SCUI_INDEV_PTR_INERTIA / SCUI_SCALE_COF;
                if (scroll->dir == scui_opt_dir_hor) scroll->point_ofs.x += dist;
                if (scroll->dir == scui_opt_dir_ver) scroll->point_ofs.y += dist;
            }
            
            scui_scroll_event_auto(event, type);
            
            scroll->speed_move = 0;
            scroll->anima_type = 4;
            widget->state.indev_hold = false;
        }
        break;
    }
    case scui_event_enc_tick: {
        if (widget->state.indev_hold)
            break;
        
        scui_coord_t way = 0;
        scui_opt_dir_t dir = scui_opt_dir_none;
        if (event->enc_way == 0) {
            if (scroll->dir == scui_opt_dir_hor) dir = scui_opt_dir_ltr;
            if (scroll->dir == scui_opt_dir_ver) dir = scui_opt_dir_utd;
            way = +1;
        }
        if (event->enc_way == 1) {
            if (scroll->dir == scui_opt_dir_hor) dir = scui_opt_dir_rtl;
            if (scroll->dir == scui_opt_dir_ver) dir = scui_opt_dir_dtu;
            way = -1;
        }
        
        /* 忽略的方向不支持 */
        if (scui_scroll_edge_skip(event->object, dir))
            break;
        
        if (scroll->dir != scui_opt_dir_hor &&
            scroll->dir != scui_opt_dir_ver) {
            // SCUI_LOG_ERROR("scroll way is unsupport");
            break;
        }
        if (scroll->route_enc == 0) {
            SCUI_LOG_ERROR("route encode is zero");
            break;
        }
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_opt_dir_hor)
            offset.x = way * scroll->route_enc * event->enc_diff;
        if (scroll->dir == scui_opt_dir_ver)
            offset.y = way * scroll->route_enc * event->enc_diff;
        
        if (scroll->anima != SCUI_HANDLE_INVALID) {
            scui_anima_stop(scroll->anima);
            scui_anima_destroy(scroll->anima);
            scroll->anima = SCUI_HANDLE_INVALID;
        }
        scroll->anima_type = 1;
        scui_scroll_offset(event->object, &offset, true);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_bar_move:
    case scui_event_bar_fling: {
        if (widget->state.indev_hold)
            break;
        
        scui_coord_t way = 0;
        scui_opt_dir_t dir = scui_opt_dir_none;
        if (event->bar_way == 0) {
            if (scroll->dir == scui_opt_dir_hor) dir = scui_opt_dir_ltr;
            if (scroll->dir == scui_opt_dir_ver) dir = scui_opt_dir_utd;
            way = +1;
        }
        if (event->bar_way == 1) {
            if (scroll->dir == scui_opt_dir_hor) dir = scui_opt_dir_rtl;
            if (scroll->dir == scui_opt_dir_ver) dir = scui_opt_dir_dtu;
            way = -1;
        }
        
        /* 忽略的方向不支持 */
        if (scui_scroll_edge_skip(event->object, dir))
            break;
        
        if (scroll->dir != scui_opt_dir_hor &&
            scroll->dir != scui_opt_dir_ver) {
            break;
        }
        if (scroll->route_bar == 0) {
            SCUI_LOG_ERROR("route bar is zero");
            break;
        }
        
        scui_coord_t diff = event->bar_diff;
        /* fling是带倍率的move */
        if (event->type == scui_event_bar_fling)
            diff *= scroll->fling_page;
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_opt_dir_hor)
            offset.x = way * scroll->route_bar * diff;
        if (scroll->dir == scui_opt_dir_ver)
            offset.y = way * scroll->route_bar * diff;
        
        if (scroll->anima != SCUI_HANDLE_INVALID) {
            scui_anima_stop(scroll->anima);
            scui_anima_destroy(scroll->anima);
            scroll->anima = SCUI_HANDLE_INVALID;
        }
        scroll->anima_type = 2;
        scui_scroll_offset(event->object, &offset, true);
        scui_event_mask_over(event);
        break;
    }
    case scui_event_key_hold:
        if (widget->state.indev_hold)
            scui_event_mask_over(event);
        break;
    case scui_event_key_click: {
        if (widget->state.indev_hold)
            break;
        
        if (event->key_id != scroll->keyid_fdir &&
            event->key_id != scroll->keyid_bdir)
            break;
        
        scui_coord_t way = 0;
        scui_opt_dir_t dir = scui_opt_dir_none;
        if (event->key_id == scroll->keyid_fdir) {
            if (scroll->dir == scui_opt_dir_hor) dir = scui_opt_dir_ltr;
            if (scroll->dir == scui_opt_dir_ver) dir = scui_opt_dir_utd;
            way = +1;
        }
        if (event->key_id == scroll->keyid_bdir) {
            if (scroll->dir == scui_opt_dir_hor) dir = scui_opt_dir_rtl;
            if (scroll->dir == scui_opt_dir_ver) dir = scui_opt_dir_dtu;
            way = -1;
        }
        
        /* 忽略的方向不支持 */
        if (scui_scroll_edge_skip(event->object, dir))
            break;
        
        if (scroll->dir != scui_opt_dir_hor &&
            scroll->dir != scui_opt_dir_ver) {
            SCUI_LOG_ERROR("scroll way is unsupport");
            break;
        }
        if (scroll->route_key == 0) {
            SCUI_LOG_ERROR("route key is zero");
            break;
        }
        
        scui_point_t offset = {0};
        if (scroll->dir == scui_opt_dir_hor)
            offset.x = way * scroll->route_key;
        if (scroll->dir == scui_opt_dir_ver)
            offset.y = way * scroll->route_key;
        
        if (scroll->anima != SCUI_HANDLE_INVALID) {
            scui_anima_stop(scroll->anima);
            scui_anima_destroy(scroll->anima);
            scroll->anima = SCUI_HANDLE_INVALID;
        }
        scroll->anima_type = 3;
        scui_scroll_offset(event->object, &offset, true);
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
