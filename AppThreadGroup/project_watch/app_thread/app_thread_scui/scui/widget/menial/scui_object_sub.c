/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 对象控件添加经典矩形属性
 *@param handle 对象控件句柄
 *@param rect   矩形属性
 */
void scui_object_prop_rect(scui_handle_t handle, scui_object_rect_t *rect)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    const scui_object_type_t src_part[] = {
        scui_object_part_rect_bg,
        scui_object_part_rect_fg,
        scui_object_part_rect_edge,
        scui_object_part_rect_box,
        scui_object_part_rect_sha,
    };
    
    const scui_object_type_t src_style[] = {
        scui_object_style_rect_alpha,
        scui_object_style_rect_color,
        scui_object_style_rect_point,
        scui_object_style_rect_align,
        scui_object_style_rect_width,
        scui_object_style_rect_height,
        scui_object_style_rect_radius,
        scui_object_style_rect_side_width,
        scui_object_style_rect_color_grad,
        scui_object_style_rect_multi,
    };
    
    scui_coord_t src_radius[scui_arr_len(src_part)] = {
        [0] = rect->radius,[1] = rect->radius,};
    
    for (scui_coord_t idx = 1; idx + 1 < scui_arr_len(src_part); idx++) {
        src_radius[idx + 1]  = src_radius[idx];
        src_radius[idx + 1] += rect->width[idx + 1];
    }
    
    SCUI_ASSERT(rect->index < scui_arr_len(src_part));
    scui_object_data_t src_data[scui_arr_len(src_style)] = {
        [0].alpha        = rect->alpha[rect->index],
        [1].color32      = rect->color[rect->index].color_s,
        [2].point        = rect->area.pos,
        [3].align        = rect->align,
        [4].number       = rect->area.w,
        [5].number       = rect->area.h,
        [6].number       = src_radius[rect->index],
        [7].number       = rect->width[rect->index],
        /*  */
        [8].color32      = rect->color[rect->index].color_e,
        [9].multi.shadow = rect->index == 3,
        [9].multi.grad_w = rect->grad_w,
        [9].multi.grad   = rect->grad,
    };
    
    scui_object_prop_t local_prop = {
        .part  = src_part[rect->index],
        .state = rect->state,
    };
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_style); idx++) {
        local_prop.style = src_style[idx];
        local_prop.data  = src_data[idx];
        scui_object_prop_add(widget->myself, &local_prop);
    }
}

/*@brief 对象控件添加经典矩形属性
 *@param handle 对象控件句柄
 *@param rect   矩形属性
 */
void scui_object_prop_rect_x(scui_handle_t handle, scui_object_rect_t *rect)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    const scui_object_type_t src_part[] = {
        scui_object_part_rect_bg,
        scui_object_part_rect_fg,
        scui_object_part_rect_edge,
        scui_object_part_rect_box,
        scui_object_part_rect_sha,
    };
    
    scui_object_rect_t local_crect = *rect;
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_part); idx++) {
        local_crect.part  = src_part[idx];
        local_crect.index = idx;
        
        scui_object_prop_rect(handle, &local_crect);
    }
}

/*@brief 对象控件添加经典圆弧属性
 *@param handle 对象控件句柄
 *@param arc    圆弧属性
 */
void scui_object_prop_arc(scui_handle_t handle, scui_object_arc_t *arc)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    const scui_object_type_t src_part[] = {
        scui_object_part_arc_bg,
        scui_object_part_arc_fg,
    };
    
    const scui_object_type_t src_style[] = {
        scui_object_style_arc_alpha,
        scui_object_style_arc_color,
        scui_object_style_arc_angle_s,
        scui_object_style_arc_angle_e,
        scui_object_style_arc_center,
        scui_object_style_arc_radius,
        scui_object_style_arc_side_width,
        scui_object_style_arc_color_grad,
        scui_object_style_arc_multi,
    };
    
    SCUI_ASSERT(arc->index < scui_arr_len(src_part));
    scui_object_data_t src_data[scui_arr_len(src_style)] = {
        [0].alpha        = arc->alpha[arc->index],
        [1].color32      = arc->color[arc->index].color_s,
        [2].number       = arc->angle_s,
        [3].number       = arc->angle_e,
        [4].point        = arc->center,
        [5].number       = arc->radius,
        [6].number       = arc->width,
        /*  */
        [7].color32      = arc->color[arc->index].color_e,
        [8].multi.round  = arc->round,
        [8].multi.grad_w = arc->grad_w,
        [8].multi.grad   = arc->grad,
    };
    
    scui_object_prop_t local_prop = {
        .part  = src_part[arc->index],
        .state = arc->state,
    };
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_style); idx++) {
        local_prop.style = src_style[idx];
        local_prop.data  = src_data[idx];
        scui_object_prop_add(widget->myself, &local_prop);
    }
}

/*@brief 对象控件添加经典线条属性
 *@param handle 对象控件句柄
 *@param line    线条属性
 */
void scui_object_prop_line(scui_handle_t handle, scui_object_line_t *line)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    const scui_object_type_t src_part[] = {
        scui_object_part_line,
    };
    
    const scui_object_type_t src_style[] = {
        scui_object_style_line_alpha,
        scui_object_style_line_color,
        scui_object_style_line_area,
        scui_object_style_line_vpos,
        scui_object_style_line_vpos_num,
        scui_object_style_line_side_width,
        scui_object_style_line_multi,
    };
    
    SCUI_ASSERT(line->index < scui_arr_len(src_part));
    scui_object_data_t src_data[scui_arr_len(src_style)] = {
        [0].alpha        = line->alpha,
        [1].color32      = line->color.color,
        [2].area         = line->area,
        [3].pointer      = line->vpos,
        [4].number       = line->vpos_num,
        [5].number       = line->width,
        /*  */
        [6].multi.round  = line->round,
    };
    
    scui_object_prop_t local_prop = {
        .part  = src_part[0],
        .state = line->state,
    };
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_style); idx++) {
        local_prop.style = src_style[idx];
        local_prop.data  = src_data[idx];
        scui_object_prop_add(widget->myself, &local_prop);
    }
}
