/*实现目标:
 *    控件操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->parent != SCUI_HANDLE_INVALID)
        return scui_widget_root(widget->parent);
    return handle;
}

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->parent;
}

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->child_num;
}

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件位置(映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_index(scui_handle_t handle, scui_handle_t index)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 其他范围映射到取值范围[0:num) */
    index = scui_mabs(index, widget->child_num);
    
    if (index < widget->child_num)
        return widget->child_list[index];
    return SCUI_HANDLE_INVALID;
}

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_attr_type(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->type;
}

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_attr_clip(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->clip;
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 如果它的父容器隐藏则它也隐藏(这是递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID)
    if (!scui_widget_style_is_show(widget->parent))
         return false;
    
    /* 它自己的显示状态 */
    if (widget->style.state)
        return true;
    
    return false;
}

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_style_is_hide(scui_handle_t handle)
{
    return !scui_widget_style_is_show(handle);
}

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@retval 控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->surface.alpha;
}

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->image;
}

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_gradient_t scui_widget_color_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->color;
}

/*@brief 控件透明度设置
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha)
{
    SCUI_LOG_INFO("widget %u alpha %u set", handle, alpha);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->surface.alpha = alpha;
    
    /* 待定中 */
    return;
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
        scui_widget_alpha_set(widget->child_list[idx], alpha);
}

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image)
{
    SCUI_LOG_INFO("widget %u image %u set", handle, image);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->image = image;
}

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_gradient_t color)
{
    SCUI_LOG_INFO("widget %u color %x set", handle, color.color.full);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->color = color;
}

/*@brief 控件对齐子控件计算
 *       中心对齐:则控件中心点与子控件中心点偏移量(最小)
 *       边界对齐:则边界中心点与子控件中心点偏移量(最小)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param pos    对齐目标
 */
bool scui_widget_align_pos_calc(scui_handle_t handle, scui_point_t *offset, scui_event_pos_t pos)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (widget->child_num == 0)
        return false;
    
    scui_point_t point_c = {0};
    scui_point_t point_child_c = {0};
    /* 中心对齐:则控件中心点与子控件中心点偏移量(最小) */
    if (pos == scui_event_pos_c) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    
    if (pos == scui_event_pos_u) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y;
    }
    if (pos == scui_event_pos_d) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y + widget->clip.h;
    }
    
    if (pos == scui_event_pos_l) {
        point_c.x = widget->clip.x;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    if (pos == scui_event_pos_r) {
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
        
        if (pos == scui_event_pos_c) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        
        if (pos == scui_event_pos_u) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y;
        }
        if (pos == scui_event_pos_d) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y + child->clip.h;
        }
        
        if (pos == scui_event_pos_l) {
            point_child_c.x = child->clip.x;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        if (pos == scui_event_pos_r) {
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
    
    if (handle_tar == SCUI_HANDLE_INVALID)
        return false;
    
    offset->x = -dist_tar_x;
    offset->y = -dist_tar_y;
    return true;
}
