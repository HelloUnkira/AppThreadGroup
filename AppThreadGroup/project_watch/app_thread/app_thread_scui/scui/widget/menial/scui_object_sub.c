/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if SCUI_DRAW_USE_THORVG
/* 矢量绘图引擎(thorvg): */
#include "thorvg_capi.h"

static void scui_object_tvg_rect_cb(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_area_t    *dst_part    = &draw_dsc->graph.dst_part;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_coord_t    src_radius  =  draw_dsc->graph.src_radius;
    scui_coord_t    src_shadow  =  draw_dsc->graph.src_shadow;
    scui_sbitfd_t   src_grad_w  =  draw_dsc->graph.src_grad_w;
    scui_sbitfd_t   src_grad    =  draw_dsc->graph.src_grad;
    /* draw dsc args<e> */
    
    scui_coord_t src_radius_max = scui_min(dst_part->w, dst_part->h) / 2;
    if (src_radius > src_radius_max || src_radius < 0)
        src_radius = src_radius_max;
    
    Tvg_Canvas  *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t offset = draw_dsc->graph.src_tvg_offset;
    
    /* 绘制要求:仅绘制在src_area中 */
    /* 所以描边部分需要动态调整一下 */
    
    scui_color32_t c_s = src_color.color_s;
    scui_color32_t c_e = src_color.color_e;
    Tvg_Color_Stop color2[] = {
        {0.0, .r = c_s.ch.r, .g = c_s.ch.g, .b = c_s.ch.b, .a = c_s.ch.a,},
        {1.0, .r = c_e.ch.r, .g = c_e.ch.g, .b = c_e.ch.b, .a = c_e.ch.a,},
    };
    
    scui_coord_t sw = src_width;
    scui_coord_t x = dst_part->x - offset.x;
    scui_coord_t y = dst_part->y - offset.y;
    scui_coord_t w = dst_part->w;
    scui_coord_t h = dst_part->h;
    scui_coord_t r = src_radius;
    
    if (src_shadow) {
        if (src_width == 0 || src_width >= src_radius) {
            SCUI_LOG_ERROR("unsupport fill grad");
            return;
        }
        
        struct {
            scui_point_t move_to;
            scui_point_t line_to[4];
            scui_point_t grad_to[2];
        } line_4[4] = {0};
        
        line_4[0].move_to    = (scui_point_t){x + r, y};
        line_4[0].line_to[0] = (scui_point_t){x + w - r, y};
        line_4[0].line_to[1] = (scui_point_t){x + w - r, y + sw};
        line_4[0].line_to[2] = (scui_point_t){x + r, y + sw};
        line_4[0].line_to[3] = (scui_point_t){x + r, y};
        line_4[0].grad_to[0] = line_4[0].line_to[2];
        line_4[0].grad_to[1] = line_4[0].line_to[3];
        
        line_4[1].move_to    = (scui_point_t){x + r, y + h - sw};
        line_4[1].line_to[0] = (scui_point_t){x + w - r, y + h - sw};
        line_4[1].line_to[1] = (scui_point_t){x + w - r, y + h};
        line_4[1].line_to[2] = (scui_point_t){x + r, y + h};
        line_4[1].line_to[3] = (scui_point_t){x + r, y + h - sw};
        line_4[1].grad_to[0] = line_4[0].line_to[3];
        line_4[1].grad_to[1] = line_4[0].line_to[2];
        
        line_4[2].move_to    = (scui_point_t){x, y + r};
        line_4[2].line_to[0] = (scui_point_t){x + sw, y + r};
        line_4[2].line_to[1] = (scui_point_t){x + sw, y + h - r};
        line_4[2].line_to[2] = (scui_point_t){x, y + h - r};
        line_4[2].line_to[3] = (scui_point_t){x, y + r};
        line_4[2].grad_to[0] = line_4[0].line_to[0];
        line_4[2].grad_to[1] = line_4[0].line_to[3];
        
        line_4[3].move_to    = (scui_point_t){x + w, y + r};
        line_4[3].line_to[0] = (scui_point_t){x + w - sw, y + r};
        line_4[3].line_to[1] = (scui_point_t){x + w - sw, y + h - r};
        line_4[3].line_to[2] = (scui_point_t){x + w, y + h - r};
        line_4[3].line_to[3] = (scui_point_t){x + w, y + r};
        line_4[3].grad_to[0] = line_4[0].line_to[0];
        line_4[3].grad_to[1] = line_4[0].line_to[3];
        
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_coord_t x = line_4[idx].move_to.x;
            scui_coord_t y = line_4[idx].move_to.y;
            
            Tvg_Paint *paint = tvg_shape_new();
            tvg_paint_set_opacity(paint, src_alpha);
            tvg_shape_move_to(paint, x, y);
            for (scui_coord_t idx_i = 0; idx_i < 4; idx_i++) {
                x = line_4[idx].line_to[idx_i].x;
                y = line_4[idx].line_to[idx_i].y;
                tvg_shape_line_to(paint,x , y);
            }
            
            scui_coord_t x1 = line_4[3].grad_to[0].x;
            scui_coord_t y1 = line_4[3].grad_to[0].y;
            scui_coord_t x2 = line_4[3].grad_to[1].x;
            scui_coord_t y2 = line_4[3].grad_to[1].y;
            Tvg_Gradient* grad = tvg_linear_gradient_new();
            tvg_linear_gradient_set(grad, x1, y1, x2, y2);
            tvg_gradient_set_color_stops(grad, color2, 2);
            tvg_shape_set_linear_gradient(paint, grad);
            tvg_canvas_push(canvas, paint);
        }
        
        struct {
            scui_point_t center;
            scui_point_t angle;
        } arc_4[4] = {0};
        
        arc_4[0].center = (scui_point_t){x + w - r, y + r};
        arc_4[0].angle  = (scui_point_t){270, 90};
        arc_4[1].center = (scui_point_t){x + w - r, y + h - r};
        arc_4[1].angle  = (scui_point_t){0, 90};
        arc_4[2].center = (scui_point_t){x + r, y + h - r};
        arc_4[2].angle  = (scui_point_t){90, 90};
        arc_4[3].center = (scui_point_t){x + r, y + r};
        arc_4[3].angle  = (scui_point_t){180, 90};
        
        
        color2[0].offset = (r - sw) * 1.0f / r;
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_coord_t c_x = arc_4[idx].center.x;
            scui_coord_t c_y = arc_4[idx].center.y;
            scui_coord_t a_s = arc_4[idx].angle.x;
            scui_coord_t a_l = arc_4[idx].angle.y;
            
            Tvg_Paint *paint = tvg_shape_new();
            tvg_paint_set_opacity(paint, src_alpha);
            tvg_shape_append_arc(paint, c_x, c_y, r, a_s, a_l, 1);
            Tvg_Gradient* grad = tvg_radial_gradient_new();
            tvg_radial_gradient_set(grad, c_x, c_y, r);
            tvg_gradient_set_color_stops(grad, color2, 2);
            tvg_shape_set_radial_gradient(paint, grad);
            tvg_canvas_push(canvas, paint);
        }
        
    } else {
        
        /* 固定:取用画笔并配置信息 */
        Tvg_Paint *paint = tvg_shape_new();
        tvg_paint_set_opacity(paint, src_alpha);
        
        uint8_t x_a = src_color.color.ch.a;
        uint8_t x_r = src_color.color.ch.r;
        uint8_t x_g = src_color.color.ch.g;
        uint8_t x_b = src_color.color.ch.b;
        
        if (src_width == 0 || src_width >= src_radius) {
            
            tvg_shape_move_to(paint, x + r, y);
            tvg_shape_line_to(paint, x + w - r, y);
            tvg_shape_line_to(paint, x + w - r, y + r);
            tvg_shape_line_to(paint, x + r, y + r);
            tvg_shape_line_to(paint, x + r, y);
            
            tvg_shape_move_to(paint, x + r, y + h - r);
            tvg_shape_line_to(paint, x + w - r, y + h - r);
            tvg_shape_line_to(paint, x + w - r, y + h);
            tvg_shape_line_to(paint, x + r, y + h);
            tvg_shape_line_to(paint, x + r, y + h - r);
            
            tvg_shape_move_to(paint, x, y + r);
            tvg_shape_line_to(paint, x + w, y + r);
            tvg_shape_line_to(paint, x + w, y + h - r);
            tvg_shape_line_to(paint, x, y + h - r);
            tvg_shape_line_to(paint, x, y + r);
            
            tvg_shape_append_arc(paint, x + w - r, y + r, r, 270, 90, 1);
            tvg_shape_append_arc(paint, x + w - r, y + h - r, r, 0, 90, 1);
            tvg_shape_append_arc(paint, x + r, y + h - r, r, 90, 90, 1);
            tvg_shape_append_arc(paint, x + r, y + r, r, 180, 90, 1);
            
            tvg_shape_set_fill_color(paint, x_r, x_g, x_b, x_a);
        } else {
            /* 调整一下边界 */
            r -= sw / 2;
            x += sw / 2;
            y += sw / 2;
            w -= sw;
            h -= sw;
            
            tvg_shape_move_to(paint, x + r, y);
            tvg_shape_line_to(paint, x + w - r, y);
            tvg_shape_append_arc(paint, x + w - r, y + r, r, 270, 90, 0);
            tvg_shape_line_to(paint, x + w, y + h - r);
            tvg_shape_append_arc(paint, x + w - r, y + h - r, r, 0, 90, 0);
            tvg_shape_line_to(paint, x + r, y + h);
            tvg_shape_append_arc(paint, x + r, y + h - r, r, 90, 90, 0);
            tvg_shape_line_to(paint, x, y + r);
            tvg_shape_append_arc(paint, x + r, y + r, r, 180, 90, 0);
            
            tvg_shape_set_stroke_color(paint, x_r, x_g, x_b, x_a);
            tvg_shape_set_stroke_width(paint, src_width);
        }
        
        if (src_grad) {
            Tvg_Gradient *grad = tvg_linear_gradient_new();
            if (src_grad_w) tvg_linear_gradient_set(grad, x, y, x, y + h);
            else tvg_linear_gradient_set(grad, x, y, x + w, y);
            tvg_gradient_set_color_stops(grad, color2, 2);
            
            if (src_width == 0 || src_width >= src_radius)
                tvg_shape_set_linear_gradient(paint, grad);
            else tvg_shape_set_stroke_linear_gradient(paint, grad);
        }
        
        tvg_canvas_push(canvas, paint);
    }
}
#endif

/*@brief 对象控件绘制圆角矩形
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_rect(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    const struct {bool must;scui_object_type_t style;} src_style[] = {
        
        {true,  scui_object_style_rect_alpha,},
        {true,  scui_object_style_rect_color,},
        {false, scui_object_style_rect_align,},
        {true,  scui_object_style_rect_width,},
        {true,  scui_object_style_rect_height,},
        {true,  scui_object_style_rect_radius,},
        {true,  scui_object_style_rect_side_width,},
        
        {false, scui_object_style_rect_color_grad,},
        {false, scui_object_style_rect_multi,},
    };
    
    /* 从属性列表取属性 */
    scui_object_data_t src_data[scui_arr_len(src_style)] = {0};
    scui_object_data_t local_data_zero = {0};
    scui_object_prop_t local_prop = {
        .part  = prop->part,
        .state = prop->state,
    };
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_style); idx++) {
        local_prop.data  = local_data_zero;
        local_prop.style = src_style[idx].style;
        bool retval = scui_object_prop_sync(handle, &local_prop);
        if (src_style[idx].must && !retval) {
            SCUI_LOG_ERROR("lose rect style %d", idx);
            return false;
        }
        
        src_data[idx] = local_prop.data;
    }
    
    scui_alpha_t   alpha        = src_data[0].alpha;
    scui_color32_t color_s      = src_data[1].color32;
    scui_opt_pos_t align        = src_data[2].align;
    scui_coord_t   width        = src_data[3].number;
    scui_coord_t   height       = src_data[4].number;
    scui_coord_t   radius       = src_data[5].number;
    scui_coord_t   side_width   = src_data[6].number;
    scui_color32_t color_e      = src_data[7].color32;
    bool shadow = src_data[8].multi.shadow;
    bool grad_w = src_data[8].multi.grad_w;
    bool grad   = src_data[8].multi.grad;
    
    alpha = scui_alpha_mix(alpha, widget->alpha);
    scui_color_t color = {
        .color   = color_s,
        .color_s = color_s,
        .color_e = color_e,
    };
    
    /* 默认为居中 */
    if (!scui_opt_bits_check(align, scui_opt_pos_hor)) align |= scui_opt_pos_hor;
    if (!scui_opt_bits_check(align, scui_opt_pos_ver)) align |= scui_opt_pos_ver;
    
    scui_area_t dst_area = widget->clip;
    if (scui_opt_bits_equal(align, scui_opt_pos_hor))
        dst_area.x += (dst_area.w - (width  - side_width * 2)) / 2;
    else if (scui_opt_bits_equal(align, scui_opt_pos_l));
    else if (scui_opt_bits_equal(align, scui_opt_pos_r))
        dst_area.x += (dst_area.w - (width  - side_width * 2));
    
    if (scui_opt_bits_equal(align, scui_opt_pos_ver))
        dst_area.y += (dst_area.h - (height - side_width * 2)) / 2;
    else if (scui_opt_bits_equal(align, scui_opt_pos_u));
    else if (scui_opt_bits_equal(align, scui_opt_pos_d))
        dst_area.y += (dst_area.h - (height - side_width * 2));
    
    dst_area.w = (width  - side_width * 2);
    dst_area.h = (height - side_width * 2);
    
    /* 检查信息 */
    if (dst_area.w == 0 || width  == 0 ||
        dst_area.h == 0 || height == 0)
        return true;
    
    /* tvg draw cb */
    scui_draw_dsc_t draw_dsc = {0};
    draw_dsc.type = scui_draw_type_pixel_tvg;
    draw_dsc.graph.dst_part   = dst_area;
    draw_dsc.graph.src_width  = side_width;
    draw_dsc.graph.src_radius = radius;
    draw_dsc.graph.src_shadow = shadow;
    draw_dsc.graph.src_grad_w = grad_w;
    draw_dsc.graph.src_grad   = grad;
    #if SCUI_DRAW_USE_THORVG
    draw_dsc.graph.src_tvg_cb = scui_object_tvg_rect_cb;
    #endif
    
    scui_widget_draw_graph(widget->myself, NULL,
        alpha, color, &draw_dsc);
    
    /* 添加抖动模糊效果 */
    if (grad) scui_widget_draw_dither(widget->myself, &dst_area);
    
    return true;
}

/*@brief 对象控件添加经典圆角矩形属性
 *@param handle 对象控件句柄
 *@param rect   圆角矩形属性
 */
void scui_object_prop_rect(scui_handle_t handle, scui_object_rect_t *rect)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_type_t src_part[] = {
        scui_object_part_rect_bg,
        scui_object_part_rect_fg,
        scui_object_part_rect_edge,
        scui_object_part_rect_box,
        scui_object_part_rect_sha,
    };
    
    scui_object_type_t src_style[] = {
        scui_object_style_rect_alpha,
        scui_object_style_rect_color,
        scui_object_style_rect_align,
        scui_object_style_rect_width,
        scui_object_style_rect_height,
        scui_object_style_rect_radius,
        scui_object_style_rect_side_width,
        
        scui_object_style_rect_color_grad,
        scui_object_style_rect_multi,
    };
    
    SCUI_ASSERT(rect->index < scui_arr_len(src_part));
    scui_coord_t src_radius[scui_arr_len(src_part)] = {
        [0] = rect->radius,[1] = rect->radius,};
    
    for (scui_coord_t idx = 1; idx + 1 < scui_arr_len(src_part); idx++) {
        src_radius[idx + 1]  = src_radius[idx];
        src_radius[idx + 1] += rect->width[idx + 1];
    }
    
    scui_object_data_t src_data[scui_arr_len(src_style)] = {
        [0].alpha   = rect->alpha[rect->index],
        [1].color32 = rect->color[rect->index].color_s,
        [2].align   = rect->align,
        [3].number  = rect->area.w,
        [4].number  = rect->area.h,
        [5].number  = src_radius[rect->index],
        [6].number  = rect->width[rect->index],
        /*  */
        [7].color32 = rect->color[rect->index].color_e,
        [8].multi.shadow = rect->index == 3,
        [8].multi.grad_w = rect->grad_w,
        [8].multi.grad   = rect->grad,
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

/*@brief 对象控件绘制圆角矩形
 *@param handle 对象控件句柄
 *@param clip   剪切域
 *@param prop   属性(state)
 *@retval 成功失败
 */
bool scui_object_draw_rect_x(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_type_t src_part[] = {
        scui_object_part_rect_bg,
        scui_object_part_rect_fg,
        scui_object_part_rect_edge,
        scui_object_part_rect_box,
        scui_object_part_rect_sha,
    };
    
    bool retval = true;
    scui_object_prop_t local_prop = *prop;
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_part); idx++) {
        local_prop.part  = src_part[idx];
        
        scui_object_state_get(widget->myself, &local_prop.state);
        if (scui_object_draw_rect(widget->myself, &local_prop))
            continue;
        
        retval = false;
    }
    
    return retval;
}

/*@brief 对象控件添加经典圆角矩形属性
 *@param handle 对象控件句柄
 *@param rect   圆角矩形属性
 */
void scui_object_prop_rect_x(scui_handle_t handle, scui_object_rect_t *rect)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_type_t src_part[] = {
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
