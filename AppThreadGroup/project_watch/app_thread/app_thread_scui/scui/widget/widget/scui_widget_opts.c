/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件坐标剥离
 *@param handle 控件句柄
 *@param point  点坐标
 *@retval 成功失败
 */
bool scui_widget_switch_point(scui_handle_t handle, scui_point_t *point)
{
    scui_handle_t  handle_t = scui_widget_tree(handle);
    scui_widget_t *widget_t = scui_handle_source_check(handle_t);
    scui_widget_t *widget   = scui_handle_source_check(handle);
    
    scui_area_t clip_t = scui_widget_clip_tree(handle);
    if (widget   != widget_t) {
        clip_t.x += widget_t->clip.x;
        clip_t.y += widget_t->clip.y;
    }
    
    if (scui_area_point(&clip_t, point)) {
        point->x -= clip_t.x;
        point->y -= clip_t.y;
        return true;
    }
    
    return false;
}

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 *@param abs    坐标基准(0:默认; 1:原始点)
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point, bool abs)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_point_t  point_c = *point;
    
    /* 父相对坐标换算为独立画布绝对坐标 */
    /* 根控件(无父)或父即本控件的独立画布(root)时, 偏置为0 */
    if (!abs && widget->parent != SCUI_HANDLE_INVALID &&
        scui_widget_root(handle) != widget->parent) {
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        point_c.x += widget_p->clip.x;
        point_c.y += widget_p->clip.y;
    }
    point = &point_c;
    
    if (widget->clip.x == point->x &&
        widget->clip.y == point->y)
        return;
    
    /* 控件悬浮, 不响应移动 */
    if (widget->style.fixed)
        return;
    
    /* 如果移动的是独立画布 */
    /* 只需要改变独立画布位置 */
    if (widget->style.buffer) {
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        
        if (widget->parent == SCUI_HANDLE_INVALID)
            scui_widget_refr(widget->myself, false);
        if (widget->parent != SCUI_HANDLE_INVALID) {
            scui_widget_surface_refr(widget, false);
            scui_widget_draw(widget->parent, NULL, false, 1);
        }
        
        SCUI_LOG_INFO("<x:%d, y:%d>", point->x, point->y);
    } else {
        /* 计算移动偏移量 */
        scui_point_t offset = {
            .x = point->x - widget->clip.x,
            .y = point->y - widget->clip.y,
        };
        
        /* 移动自己 */
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        scui_widget_surface_refr(widget, false);
        
        /* 移动孩子,迭代它的孩子列表 */
        scui_widget_child_list_btra(widget, idx) {
            scui_handle_t  handle_c = widget->child_list[idx];
            scui_widget_t *widget_c = scui_handle_source_check(handle_c);
            scui_point_t    point_c = {0};
            point_c.x = offset.x + widget_c->clip.x;
            point_c.y = offset.y + widget_c->clip.y;
            scui_widget_move_pos(handle_c, &point_c, true);
        }
        
        scui_widget_draw(widget->parent, NULL, false, 0);
    }
    
    /* 子控件更新, 父控件更新 */
    scui_event_define(event_c, widget->myself, true, scui_event_self_pos,  scui_event_absorb_none);
    scui_event_define(event_p, widget->parent, true, scui_event_child_pos, scui_event_absorb_none);
    scui_event_notify(&event_c); if (widget->parent == SCUI_HANDLE_INVALID) return;
    scui_event_notify(&event_p);
}

/*@brief 控件移动
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_move_ofs(scui_handle_t handle, scui_point_t *offset)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_point_t point = {0};
    point.x = widget->clip.x + offset->x;
    point.y = widget->clip.y + offset->y;
    scui_widget_move_pos(handle, &point, true);
}

/*@brief 子控件坐标对齐
 *@param handle  控件句柄
 *@param handle  控件句柄(目标控件,不存在则相对父控件)
 *@param align   对齐方向
 *@param offset  偏移量
 */
void scui_widget_align_pos(scui_handle_t handle, scui_handle_t target, scui_align_t align, scui_point_t *offset)
{
    scui_widget_t *widget   = scui_handle_source_check(handle);
    scui_handle_t  handle_t = target != SCUI_HANDLE_INVALID ? target : widget->parent;
    scui_widget_t *widget_t = scui_handle_source_check(handle_t);
    if (handle_t == SCUI_HANDLE_INVALID) return;
    /* 需要找到有效的对齐目标 */
    
    /* 取区域对齐偏移(下沉层双轴), 再叠加外部偏移修正 */
    scui_point_t ofs = scui_area_align(&widget_t->clip, &widget->clip, align);
    if (offset != NULL) {
        ofs.x += offset->x;
        ofs.y += offset->y;
    }
    
    scui_point_t point = {
        .x = widget_t->clip.x + ofs.x,
        .y = widget_t->clip.y + ofs.y,
    };
    scui_widget_move_pos(handle, &point, true);
}

/*@brief 子控件坐标镜像
 *@param handle  控件句柄
 *@param child   控件子控件句柄(为空则镜像所有子控件)
 *@param dir     镜像方向(水平镜像或垂直镜像)
 *@param recurse 递归处理(全局镜像有效)
 */
void scui_widget_mirror_pos(scui_handle_t handle, scui_handle_t child, scui_opt_dir_t dir, bool recurse)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 移动孩子,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_point_t    point_c = {0};
        point_c.x = widget_c->clip.x;
        point_c.y = widget_c->clip.y;
        
        /* 存在指定子控件时, 只镜像子控件 */
        if (child != SCUI_HANDLE_INVALID && child != handle_c)
            continue;
        
        if (scui_opt_bits_equal(dir, scui_opt_dir_hor))
            point_c.x = widget->clip.w - widget_c->clip.w - widget_c->clip.x;
        if (scui_opt_bits_equal(dir, scui_opt_dir_ver))
            point_c.y = widget->clip.h - widget_c->clip.h - widget_c->clip.y;
        
        scui_widget_move_pos(handle_c, &point_c, true);
        
        if (!recurse)
             continue;
        
        /* 递归镜像 */
        scui_widget_mirror_pos(handle_c, SCUI_HANDLE_INVALID, dir, recurse);
    }
}

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_adjust_size(scui_handle_t handle, scui_coord_t width, scui_coord_t height)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (widget->clip.w == width &&
        widget->clip.h == height)
        return;
    
    /* 因为会影响到画布资源, 暂定于, 禁止独立画布控件修改自己尺寸 */
    if (widget->style.buffer || widget->parent == SCUI_HANDLE_INVALID) {
        SCUI_LOG_ERROR("unsupport");
        return;
    }
    
    if (width <= 0 || height <= 0) {
        scui_widget_layout_refr(widget->myself);
        return;
    }
    
    widget->clip.w = width;
    widget->clip.h = height;
    scui_widget_surface_refr(widget, false);
    scui_widget_draw(widget->myself, NULL, false, 0);
    
    /* 子控件更新, 父控件更新 */
    scui_event_define(event_c, widget->myself, true, scui_event_self_size,  scui_event_absorb_none);
    scui_event_define(event_p, widget->parent, true, scui_event_child_size, scui_event_absorb_none);
    scui_event_notify(&event_c);
    scui_event_notify(&event_p);
}

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_clist_move_ofs(scui_handle_t handle, scui_point_t *offset)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_point_t    point_c = {0};
        point_c.x = widget_c->clip.x + offset->x;
        point_c.y = widget_c->clip.y + offset->y;
        scui_widget_move_pos(handle_c, &point_c, true);
    }
}

/*@brief 控件移动子控件(循环模式)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param range  偏移量限制
 */
void scui_widget_clist_move_ofs_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        
        scui_area_t clip_inter = {0};
        scui_area_t clip_c = widget_c->clip;
        clip_c.x += offset->x;
        clip_c.y += offset->y;
        
        /* 计算是否与父控件存在交集 */
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_c)) {
            scui_widget_move_pos(handle_c, &clip_c.pos, true);
            continue;
        }
        
        /* 偏回一个range,计算是否与父控件存在交集 */
        clip_c.x -= range->x;
        clip_c.y -= range->y;
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_c)) {
            scui_widget_move_pos(handle_c, &clip_c.pos, true);
            continue;
        }
        clip_c.x += range->x;
        clip_c.y += range->y;
        
        /* 偏去一个range,计算是否与父控件存在交集 */
        clip_c.x += range->x;
        clip_c.y += range->y;
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_c)) {
            scui_widget_move_pos(handle_c, &clip_c.pos, true);
            continue;
        }
        clip_c.x -= range->x;
        clip_c.y -= range->y;
        
        /* 正常继续偏转 */
        scui_widget_move_pos(handle_c, &clip_c.pos, true);
    }
}

/*@brief 控件对齐子控件计算
 *       中心对齐:则控件中心点与子控件中心点偏移量(最小)
 *       边界对齐:则边界中心点与子控件中心点偏移量(最小)
 *@param handle 控件句柄
 *@param target 控件句柄(目标控件)
 *@param offset 偏移量
 *@param pos    对齐目标
 */
bool scui_widget_align_pos_calc(scui_handle_t handle, scui_handle_t *target, scui_point_t *offset, scui_opt_pos_t pos)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (widget->child_num == 0)
        return false;
    
    scui_point_t point_w = {0};
    /* 中心对齐:则控件中心点与子控件中心点偏移量(最小) */
    switch (pos) {
    case scui_opt_pos_c:
        point_w.x = widget->clip.x + widget->clip.w / 2;
        point_w.y = widget->clip.y + widget->clip.h / 2;
        break;
    case scui_opt_pos_u:
        point_w.x = widget->clip.x + widget->clip.w / 2;
        point_w.y = widget->clip.y;
        break;
    case scui_opt_pos_d:
        point_w.x = widget->clip.x + widget->clip.w / 2;
        point_w.y = widget->clip.y + widget->clip.h;
        break;
    case scui_opt_pos_l:
        point_w.x = widget->clip.x;
        point_w.y = widget->clip.y + widget->clip.h / 2;
        break;
    case scui_opt_pos_r:
        point_w.x = widget->clip.x + widget->clip.w;
        point_w.y = widget->clip.y + widget->clip.h / 2;
        break;
    default:
        SCUI_ASSERT(false);
        break;
    }
    
    if (point_w.x == 0 && point_w.y == 0)
        return false;
    
    scui_handle_t handle_t = SCUI_HANDLE_INVALID;
    scui_multi_t  dist_t_a = scui_multi_max;
    scui_coord_t  dist_t_x = 0;
    scui_coord_t  dist_t_y = 0;
    
    /* 迭代计算每一个子控件,找到最小偏移量 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_point_t    point_c = {0};
        
        switch (pos) {
        case scui_opt_pos_c:
            point_c.x = widget_c->clip.x + widget_c->clip.w / 2;
            point_c.y = widget_c->clip.y + widget_c->clip.h / 2;
            break;
        case scui_opt_pos_u:
            point_c.x = widget_c->clip.x + widget_c->clip.w / 2;
            point_c.y = widget_c->clip.y;
            break;
        case scui_opt_pos_d:
            point_c.x = widget_c->clip.x + widget_c->clip.w / 2;
            point_c.y = widget_c->clip.y + widget_c->clip.h;
            break;
        case scui_opt_pos_l:
            point_c.x = widget_c->clip.x;
            point_c.y = widget_c->clip.y + widget_c->clip.h / 2;
            break;
        case scui_opt_pos_r:
            point_c.x = widget_c->clip.x + widget_c->clip.w;
            point_c.y = widget_c->clip.y + widget_c->clip.h / 2;
            break;
        default:
            SCUI_ASSERT(false);
            break;
        }
        
        scui_coord_t dist_x = point_c.x - point_w.x;
        scui_coord_t dist_y = point_c.y - point_w.y;
        scui_multi_t dist_a = dist_x * dist_x + dist_y * dist_y;
        
        if (dist_t_a > dist_a) {
            dist_t_a = dist_a;
            dist_t_x = dist_x;
            dist_t_y = dist_y;
            handle_t = widget_c->myself;
        }
    }
    
    if (target != NULL)
       *target  = handle_t;
    if (handle_t == SCUI_HANDLE_INVALID)
        return false;
    
    offset->x = -dist_t_x;
    offset->y = -dist_t_y;
    return true;
}