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

static void scui_object_tvg_arc_cb(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_coord_t    src_radius  =  draw_dsc->graph.src_radius;
    scui_point_t    src_center  =  draw_dsc->graph.src_center;
    scui_coord_t    src_angle_s =  draw_dsc->graph.src_angle_s;
    scui_coord_t    src_angle_e =  draw_dsc->graph.src_angle_e;
    scui_sbitfd_t   src_round   =  draw_dsc->graph.src_round;
    scui_sbitfd_t   src_grad_w  =  draw_dsc->graph.src_grad_w;
    scui_sbitfd_t   src_grad    =  draw_dsc->graph.src_grad;
    /* draw dsc args<e> */
    
    if (src_radius <= 0)
        return;
    
    Tvg_Canvas   *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t  offset = draw_dsc->graph.src_tvg_offset;
    
    /* 固定:取用画笔并配置信息 */
    Tvg_Paint *paint = tvg_shape_new();
    tvg_paint_set_opacity(paint, src_alpha);
    
    scui_coord_t full = (src_width == 0 || src_width >= src_radius) ? 1 : 0;
    scui_coord_t s_width = (src_width == 0 || src_width >= src_radius) ? 0 : src_width;
    
    /* 一个弧形扇形 */
    scui_coord_t a_s  = src_angle_s;
    scui_coord_t a_l  = src_angle_e  - src_angle_s;
    scui_coord_t c_x  = src_center.x - offset.x;
    scui_coord_t c_y  = src_center.y - offset.y;
    tvg_shape_append_arc(paint, c_x, c_y, src_radius, a_s, a_l, full);
    if (src_round) tvg_shape_set_stroke_cap(paint, TVG_STROKE_CAP_ROUND);
    
    if (src_grad) {
        uint8_t g_a[2] = {0}, g_r[2] = {0}, g_g[2] = {0}, g_b[2] = {0};
        g_a[0] = src_color.color_s.ch.a; g_a[1] = src_color.color_e.ch.a;
        g_r[0] = src_color.color_s.ch.r; g_r[1] = src_color.color_e.ch.r;
        g_g[0] = src_color.color_s.ch.g; g_g[1] = src_color.color_e.ch.g;
        g_b[0] = src_color.color_s.ch.b; g_b[1] = src_color.color_e.ch.b;
        
        Tvg_Color_Stop g_cs[2] = {0};
        g_cs[0] = (Tvg_Color_Stop){0.0, .r = g_r[0], .g = g_g[0], .b = g_b[0], .a = g_a[0],};
        g_cs[1] = (Tvg_Color_Stop){1.0, .r = g_r[1], .g = g_g[1], .b = g_b[1], .a = g_a[1],};
        
        Tvg_Gradient *p_grad = tvg_linear_gradient_new();
        if (src_grad_w) tvg_linear_gradient_set(p_grad, c_x, c_y - src_radius, c_x, c_y + src_radius);
        else tvg_linear_gradient_set(p_grad, c_x - src_radius, c_y, c_x + src_radius, c_y);
        tvg_gradient_set_color_stops(p_grad, g_cs, 2);
        if (full) tvg_shape_set_linear_gradient(paint, p_grad);
        else tvg_shape_set_stroke_linear_gradient(paint, p_grad);
        tvg_shape_set_stroke_width(paint, s_width);
    } else {
        uint8_t f_a = 0, f_r = 0, f_g = 0, f_b = 0;
        uint8_t s_a = 0, s_r = 0, s_g = 0, s_b = 0;
        
        if (src_width == 0 || src_width >= src_radius) {
            f_a = src_color.color.ch.a;
            f_r = src_color.color.ch.r;
            f_g = src_color.color.ch.g;
            f_b = src_color.color.ch.b;
        } else {
            s_a = src_color.color.ch.a;
            s_r = src_color.color.ch.r;
            s_g = src_color.color.ch.g;
            s_b = src_color.color.ch.b;
            s_width = src_width;
        }
        
        tvg_shape_set_fill_color(paint, f_r, f_g, f_b, f_a);
        tvg_shape_set_stroke_color(paint, s_r, s_g, s_b, s_a);
        tvg_shape_set_stroke_width(paint, s_width);
    }
    
    tvg_canvas_push(canvas, paint);
}

static void scui_object_tvg_line_cb(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_point_t   *src_vpos    =  draw_dsc->graph.src_vpos;
    scui_coord_t    src_vpos_c  =  draw_dsc->graph.src_vpos_c;
    bool            src_round   =  draw_dsc->graph.src_round;
    /* draw dsc args<e> */
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_width <= 0)
        src_width  = 1;
    
    if (src_vpos_c == 0)
        return;
    
    Tvg_Canvas   *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t  offset = draw_dsc->graph.src_tvg_offset;
    
    /* 固定:取用画笔并配置信息 */
    Tvg_Paint *paint = tvg_shape_new();
    tvg_paint_set_opacity(paint, src_alpha);
    
    uint8_t s_a = src_color.color.ch.a;
    uint8_t s_r = src_color.color.ch.r;
    uint8_t s_g = src_color.color.ch.g;
    uint8_t s_b = src_color.color.ch.b;
    
    /* 线条序列 */
    tvg_shape_move_to(paint, src_vpos[0].x - dst_clip->x, src_vpos[0].y - dst_clip->y);
    
    for (scui_coord_t idx = 1; idx < src_vpos_c; idx++)
        tvg_shape_line_to(paint, src_vpos[idx].x - dst_clip->x, src_vpos[idx].y - dst_clip->y);
    
    if (src_round) tvg_shape_set_stroke_cap(paint, TVG_STROKE_CAP_ROUND);
    tvg_shape_set_stroke_width(paint, src_width);
    tvg_shape_set_stroke_color(paint, s_r, s_g, s_b, s_a);
    tvg_canvas_push(canvas, paint);
}

#endif

/*@brief 对象控件绘制属性同步
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@param offset 样式偏移
 *@param number 样式数量
 *@param data   属性数据
 *@retval 成功失败
 */
static bool scui_object_draw_prop_sync(scui_handle_t handle, scui_object_prop_t *prop,
    scui_object_type_t offset, scui_object_type_t number, scui_object_data_t *data)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_data_t local_data_zero = {0};
    scui_object_prop_t local_prop = {
        .part  = prop->part,
        .state = prop->state,
    };
    for (scui_coord_t idx = 0; idx < number; idx++) {
        local_prop.data  = local_data_zero;
        local_prop.style = offset + idx;
        if (!scui_object_prop_sync(handle, &local_prop)) {
             SCUI_LOG_ERROR("lose rect style %d", idx);
             return false;
        }
        
        data[idx] = local_prop.data;
    }
    
    return true;
}

/*@brief 对象控件绘制矩形
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_rect(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 从属性列表取属性 */
    const scui_multi_t src_style_ofs = scui_object_style_rect_s + 1;
    const scui_coord_t src_style_num = scui_object_style_rect_e - scui_object_style_rect_s - 1;
    scui_object_data_t src_data[scui_object_style_rect_e - scui_object_style_rect_s] = {0};
    if (!scui_object_draw_prop_sync(handle, prop, src_style_ofs, src_style_num, src_data))
         return false;
    
    scui_alpha_t alpha = src_data[scui_object_style_idx(rect_alpha)].alpha;
    alpha = scui_alpha_mix(alpha, widget->alpha);
    scui_color_t color = {
        .color_s = src_data[scui_object_style_idx(rect_color)].color32,
        .color_e = src_data[scui_object_style_idx(rect_color_grad)].color32,
    };
    
    scui_area_t dst_area = widget->clip;
    dst_area.x += src_data[scui_object_style_idx(rect_point)].point.x;
    dst_area.y += src_data[scui_object_style_idx(rect_point)].point.y;
    
    scui_opt_pos_t align = src_data[scui_object_style_idx(rect_align)].align;
    scui_coord_t   width = src_data[scui_object_style_idx(rect_width)].number;
    scui_coord_t  height = src_data[scui_object_style_idx(rect_height)].number;
    scui_coord_t s_width = src_data[scui_object_style_idx(rect_side_width)].number;
    
    if (scui_opt_bits_equal(align, scui_opt_pos_hor))
        dst_area.x += (dst_area.w - (width  - s_width * 2)) / 2;
    else if (scui_opt_bits_equal(align, scui_opt_pos_l));
    else if (scui_opt_bits_equal(align, scui_opt_pos_r))
        dst_area.x += (dst_area.w - (width  - s_width * 2));
    
    if (scui_opt_bits_equal(align, scui_opt_pos_ver))
        dst_area.y += (dst_area.h - (height - s_width * 2)) / 2;
    else if (scui_opt_bits_equal(align, scui_opt_pos_u));
    else if (scui_opt_bits_equal(align, scui_opt_pos_d))
        dst_area.y += (dst_area.h - (height - s_width * 2));
    
    dst_area.w = (width  - s_width * 2);
    dst_area.h = (height - s_width * 2);
    
    /* 检查信息 */
    if (dst_area.w == 0 || width  == 0 ||
        dst_area.h == 0 || height == 0)
        return true;
    
    /* tvg draw cb */
    scui_draw_dsc_t draw_dsc = {0};
    draw_dsc.type = scui_draw_type_pixel_tvg;
    draw_dsc.graph.dst_part   = dst_area;
    draw_dsc.graph.src_width  = s_width;
    draw_dsc.graph.src_radius = src_data[scui_object_style_idx(rect_radius)].number;
    draw_dsc.graph.src_shadow = src_data[scui_object_style_idx(rect_multi)].multi.shadow;
    draw_dsc.graph.src_grad_w = src_data[scui_object_style_idx(rect_multi)].multi.grad_w;
    draw_dsc.graph.src_grad   = src_data[scui_object_style_idx(rect_multi)].multi.grad;
    #if SCUI_DRAW_USE_THORVG
    draw_dsc.graph.src_tvg_cb = scui_object_tvg_rect_cb;
    #endif
    
    scui_widget_draw_graph(widget->myself, NULL,
        alpha, color, &draw_dsc);
    
    /* 添加抖动模糊效果 */
    if (draw_dsc.graph.src_grad)
        scui_widget_draw_dither(widget->myself, NULL);
    
    return true;
}

/*@brief 对象控件绘制矩形
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
    
    const scui_object_type_t src_part[] = {
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

/*@brief 对象控件绘制圆弧
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_arc(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 从属性列表取属性 */
    const scui_multi_t src_style_ofs = scui_object_style_arc_s + 1;
    const scui_coord_t src_style_num = scui_object_style_arc_e - scui_object_style_arc_s - 1;
    scui_object_data_t src_data[scui_object_style_arc_e - scui_object_style_arc_s] = {0};
    if (!scui_object_draw_prop_sync(handle, prop, src_style_ofs, src_style_num, src_data))
         return false;
    
    scui_alpha_t alpha = src_data[scui_object_style_idx(arc_alpha)].alpha;
    alpha = scui_alpha_mix(alpha, widget->alpha);
    scui_color_t color = {
        .color_s = src_data[scui_object_style_idx(arc_color)].color32,
        .color_e = src_data[scui_object_style_idx(arc_color_grad)].color32,
    };
    
    scui_draw_dsc_t draw_dsc = {0};
    draw_dsc.type = scui_draw_type_pixel_tvg;
    draw_dsc.graph.src_center  = src_data[scui_object_style_idx(arc_center)].point;
    draw_dsc.graph.src_width   = src_data[scui_object_style_idx(arc_side_width)].number;
    draw_dsc.graph.src_radius  = src_data[scui_object_style_idx(arc_radius)].number;
    draw_dsc.graph.src_angle_s = src_data[scui_object_style_idx(arc_angle_s)].number;
    draw_dsc.graph.src_angle_e = src_data[scui_object_style_idx(arc_angle_e)].number;
    draw_dsc.graph.src_round   = src_data[scui_object_style_idx(arc_multi)].multi.round;
    draw_dsc.graph.src_grad_w  = src_data[scui_object_style_idx(arc_multi)].multi.grad_w;
    draw_dsc.graph.src_grad    = src_data[scui_object_style_idx(arc_multi)].multi.grad;
    #if SCUI_DRAW_USE_THORVG
    draw_dsc.graph.src_tvg_cb  = scui_object_tvg_arc_cb;
    #endif
    
    draw_dsc.graph.src_center.x += widget->clip.x;
    draw_dsc.graph.src_center.y += widget->clip.y;
    draw_dsc.graph.src_radius   -= draw_dsc.graph.src_width / 2  + 1;
    
    scui_widget_draw_graph(widget->myself, NULL,
        alpha, color, &draw_dsc);
    
    /* 添加抖动模糊效果 */
    if (draw_dsc.graph.src_grad)
        scui_widget_draw_dither(widget->myself, NULL);
    
    return true;
}

/*@brief 对象控件绘制线条
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_line(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 从属性列表取属性 */
    const scui_multi_t src_style_ofs = scui_object_style_line_s + 1;
    const scui_coord_t src_style_num = scui_object_style_line_e - scui_object_style_line_s - 1;
    scui_object_data_t src_data[scui_object_style_line_e - scui_object_style_line_s] = {0};
    if (!scui_object_draw_prop_sync(handle, prop, src_style_ofs, src_style_num, src_data))
         return false;
    
    scui_alpha_t alpha = src_data[scui_object_style_idx(line_alpha)].alpha;
    alpha = scui_alpha_mix(alpha, widget->alpha);
    scui_color_t color = {
        .color = src_data[scui_object_style_idx(line_color)].color32,
    };
    
    scui_area_t area = src_data[scui_object_style_idx(line_area)].area;
    area.x += widget->clip.x; area.y += widget->clip.y;
    
    scui_draw_dsc_t draw_dsc = {0};
    draw_dsc.type = scui_draw_type_pixel_tvg;
    draw_dsc.graph.src_vpos   = src_data[scui_object_style_idx(line_vpos)].pointer;
    draw_dsc.graph.src_vpos_c = src_data[scui_object_style_idx(line_vpos_num)].number;
    draw_dsc.graph.src_width  = src_data[scui_object_style_idx(line_side_width)].number;
    draw_dsc.graph.src_round  = src_data[scui_object_style_idx(line_multi)].multi.round;
    #if SCUI_DRAW_USE_THORVG
    draw_dsc.graph.src_tvg_cb = scui_object_tvg_line_cb;
    #endif
    
    for (scui_coord_t idx = 0; idx < draw_dsc.graph.src_vpos_c; idx++) {
        draw_dsc.graph.src_vpos[idx].x += area.x;
        draw_dsc.graph.src_vpos[idx].y += area.y;
    }
    
    scui_widget_draw_graph(widget->myself, &area,
        alpha, color, &draw_dsc);
    
    return true;
}
