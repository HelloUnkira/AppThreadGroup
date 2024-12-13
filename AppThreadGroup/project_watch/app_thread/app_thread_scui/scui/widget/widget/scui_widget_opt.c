/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 *@param dirty  脏标记
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point, bool dirty)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->clip.x == point->x &&
        widget->clip.y == point->y)
        return;
    
    SCUI_LOG_DEBUG("");
    /* 存在独立画布下,如果移动的是窗口,只需要改变窗口位置 */
    if (widget->parent == SCUI_HANDLE_INVALID &&
        scui_widget_surface_only(widget)) {
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        scui_widget_refr(widget->myself, false);
        SCUI_LOG_DEBUG("<x:%d, y:%d>", point->x, point->y);
        return;
    }
    
    SCUI_LOG_DEBUG("");
    /* 计算移动偏移量 */
    scui_point_t offset = {
        .x = point->x - widget->clip.x,
        .y = point->y - widget->clip.y,
    };
    
    SCUI_LOG_DEBUG("");
    /* 移动自己 */
    widget->clip.x = point->x;
    widget->clip.y = point->y;
    
    SCUI_LOG_DEBUG("");
    /* 更新画布剪切域(更新自己的) */
    if (dirty) {
        scui_widget_surface_refr(widget->myself, false);
        scui_widget_clip_clear(widget, false);
    }
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_point_t point = {
            .x = offset.x + child->clip.x,
            .y = offset.y + child->clip.y,
        };
        scui_widget_move_pos(handle, &point, dirty);
    }
    
    SCUI_LOG_DEBUG("");
    /* 更新画布剪切域(更新自己和父亲的) */
    if (dirty)
    if (widget->parent != SCUI_HANDLE_INVALID) {
        widget = scui_handle_get(widget->parent);
        SCUI_ASSERT(widget != NULL);
        
        scui_widget_surface_refr(widget->myself, false);
        scui_widget_clip_clear(widget, false);
        
        // 全部更新后需要从父控件开始重绘
        scui_widget_draw(widget->parent, NULL, false);
    }
}

/*@brief 子控件坐标对齐
 *@param handle  控件句柄
 *@param handle  控件句柄(目标控件,不存在则相对父控件)
 *@param pos     对齐方向
 *@param offset  偏移量
 */
void scui_widget_align_pos(scui_handle_t handle, scui_handle_t target, scui_opt_pos_t pos, scui_point_t *offset)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_point_t point = {
        .x = widget->clip.x,
        .y = widget->clip.y,
    };
    
    if (target != SCUI_HANDLE_INVALID || widget->parent != SCUI_HANDLE_INVALID) {
        scui_handle_t  handle_target = target != SCUI_HANDLE_INVALID ? target : widget->parent;
        scui_widget_t *widget_target = scui_handle_get(handle_target);
        SCUI_ASSERT(widget_target != NULL);
        
        point.x = widget->clip.x;
        // 水平左对齐
        if ((pos & scui_opt_pos_l) != 0)
            point.x = (widget_target->clip.x);
        // 水平右对齐
        if ((pos & scui_opt_pos_r) != 0)
            point.x = (widget_target->clip.x + widget_target->clip.w - widget->clip.w);
        // 水平中心对齐
        if ((pos & scui_opt_dir_hor) != 0)
            point.x /= 2;
        
        point.y = widget->clip.y;
        // 垂直上对齐
        if ((pos & scui_opt_pos_u) != 0)
            point.y = (widget_target->clip.y);
        // 垂直下对齐
        if ((pos & scui_opt_pos_d) != 0)
            point.y = (widget_target->clip.y + widget_target->clip.h - widget->clip.h);
        // 垂直中心对齐
        if ((pos & scui_opt_dir_ver) != 0)
            point.y /= 2;
    }
    
    point.x += offset->x;
    point.y += offset->y;
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
    SCUI_LOG_DEBUG("");
    scui_handle_t  handle_child = child;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_ASSERT(dir == scui_opt_dir_hor || dir == scui_opt_dir_ver);
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_point_t   point = {.x = child->clip.x, .y = child->clip.y,};
        
        /* 存在指定子控件时, 只镜像子控件 */
        if (handle_child != SCUI_HANDLE_INVALID && handle_child != handle)
            continue;
        
        if (dir == scui_opt_dir_hor)
            point.x = widget->clip.w - child->clip.w - child->clip.x;
        if (dir == scui_opt_dir_ver)
            point.y = widget->clip.h - child->clip.h - child->clip.y;
        
        scui_widget_move_pos(handle, &point, true);
        
        if (!recurse)
             continue;
        
        /* 递归镜像 */
        scui_widget_mirror_pos(handle, SCUI_HANDLE_INVALID, dir, recurse);
    }
    
    if (handle_child != SCUI_HANDLE_INVALID)
        SCUI_LOG_ERROR("widget %u have not child widget %u", handle, child);
}

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_adjust_size(scui_handle_t handle, scui_coord_t width, scui_coord_t height)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(width  >= 0);
    SCUI_ASSERT(height >= 0);
    
    if (widget->parent == SCUI_HANDLE_INVALID) {
        SCUI_LOG_ERROR("unsupport");
        return;
    }
    
    if (widget->clip.w == width &&
        widget->clip.h == height)
        return;
    
    widget->clip.w = width;
    widget->clip.h = height;
    /* 更新画布剪切域(从父控件开始递归) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        widget = scui_handle_get(widget->parent);
        SCUI_ASSERT(widget != NULL);
    }
    scui_widget_clip_clear(widget, true);
    scui_widget_surface_refr(widget->myself, true);
    
    scui_event_t event = {
        .object = widget->myself,
        .type   = scui_event_adjust_size,
        .absorb = scui_event_absorb_none,
    };
    scui_event_notify(&event);
}

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param dirty  脏标记
 */
void scui_widget_move_ofs_child_list(scui_handle_t handle, scui_point_t *offset, bool dirty)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_area_t clip_child = child->clip;
        clip_child.x += offset->x;
        clip_child.y += offset->y;
        scui_widget_move_pos(handle, &clip_child, dirty);
    }
}

/*@brief 控件移动子控件(循环模式)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param range  偏移量限制
 *@param dirty  脏标记
 */
void scui_widget_move_ofs_child_list_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range, bool dirty)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_area_t clip_child = child->clip;
        scui_area_t clip_inter = {0};
        clip_child.x += offset->x;
        clip_child.y += offset->y;
        
        /* 计算是否与父控件存在交集 */
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_child)) {
            scui_widget_move_pos(handle, &clip_child, dirty);
            continue;
        }
        
        /* 偏回一个range,计算是否与父控件存在交集 */
        clip_child.x -= range->x;
        clip_child.y -= range->y;
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_child)) {
            scui_widget_move_pos(handle, &clip_child, dirty);
            continue;
        }
        clip_child.x += range->x;
        clip_child.y += range->y;
        
        /* 偏去一个range,计算是否与父控件存在交集 */
        clip_child.x += range->x;
        clip_child.y += range->y;
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_child)) {
            scui_widget_move_pos(handle, &clip_child, dirty);
            continue;
        }
        clip_child.x -= range->x;
        clip_child.y -= range->y;
        
        /* 正常继续偏转 */
        scui_widget_move_pos(handle, &clip_child, dirty);
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
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (widget->child_num == 0)
        return false;
    
    scui_point_t point_c = {0};
    scui_point_t point_child_c = {0};
    /* 中心对齐:则控件中心点与子控件中心点偏移量(最小) */
    if (pos == scui_opt_pos_c) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    
    if (pos == scui_opt_pos_u) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y;
    }
    if (pos == scui_opt_pos_d) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y + widget->clip.h;
    }
    
    if (pos == scui_opt_pos_l) {
        point_c.x = widget->clip.x;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    if (pos == scui_opt_pos_r) {
        point_c.x = widget->clip.x + widget->clip.w;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    
    if (point_c.x == 0 && point_c.y == 0)
        return false;
    
    scui_handle_t handle_tar = SCUI_HANDLE_INVALID;
    scui_multi_t  dist_tar_a = scui_coord_max;
    scui_coord_t  dist_tar_x = 0;
    scui_coord_t  dist_tar_y = 0;
    
    /* 迭代计算每一个子控件,找到最小偏移量 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        
        if (pos == scui_opt_pos_c) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        
        if (pos == scui_opt_pos_u) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y;
        }
        if (pos == scui_opt_pos_d) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y + child->clip.h;
        }
        
        if (pos == scui_opt_pos_l) {
            point_child_c.x = child->clip.x;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        if (pos == scui_opt_pos_r) {
            point_child_c.x = child->clip.x + child->clip.w;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        
        scui_coord_t dist_x = point_child_c.x - point_c.x;
        scui_coord_t dist_y = point_child_c.y - point_c.y;
        scui_multi_t dist_a = dist_x * dist_x + dist_y * dist_y;
        
        if (dist_tar_a > dist_a) {
            dist_tar_a = dist_a;
            dist_tar_x = dist_x;
            dist_tar_y = dist_y;
            handle_tar = child->myself;
        }
    }
    
    if (target != NULL)
       *target  = handle_tar;
    if (handle_tar == SCUI_HANDLE_INVALID)
        return false;
    
    offset->x = -dist_tar_x;
    offset->y = -dist_tar_y;
    return true;
}
