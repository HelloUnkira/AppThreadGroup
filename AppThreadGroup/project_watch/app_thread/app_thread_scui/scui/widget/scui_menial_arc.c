/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if SCUI_DRAW_USE_THORVG
/* 矢量绘图引擎(thorvg): */
#include "thorvg_capi.h"

static void scui_menial_tvg_cb(scui_draw_dsc_t *draw_dsc)
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
    /* draw dsc args<e> */
    
    if (src_radius <= 0)
        return;
    
    Tvg_Canvas   *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t  offset = draw_dsc->graph.src_tvg_offset;
    scui_sbitfd_t round  = draw_dsc->graph.src_tvg_round;
    scui_sbitfd_t gradw  = draw_dsc->graph.src_tvg_gradw;
    scui_sbitfd_t grad   = draw_dsc->graph.src_tvg_grad;
    
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
    if (round) tvg_shape_set_stroke_cap(paint, TVG_STROKE_CAP_ROUND);
    
    uint8_t f_a = 0, f_r = 0, f_g = 0, f_b = 0;
    uint8_t s_a = 0, s_r = 0, s_g = 0, s_b = 0;
    
    if (grad) {
        uint8_t g_a[2] = {0}, g_r[2] = {0}, g_g[2] = {0}, g_b[2] = {0};
        g_a[0] = src_color.color_s.ch.a; g_a[1] = src_color.color_e.ch.a;
        g_r[0] = src_color.color_s.ch.r; g_r[1] = src_color.color_e.ch.r;
        g_g[0] = src_color.color_s.ch.g; g_g[1] = src_color.color_e.ch.g;
        g_b[0] = src_color.color_s.ch.b; g_b[1] = src_color.color_e.ch.b;
        
        Tvg_Color_Stop g_cs[2] = {0};
        g_cs[0] = (Tvg_Color_Stop){0.0, .r = g_r[0], .g = g_g[0], .b = g_b[0], .a = g_a[0],};
        g_cs[1] = (Tvg_Color_Stop){1.0, .r = g_r[1], .g = g_g[1], .b = g_b[1], .a = g_a[1],};
        
        Tvg_Gradient *p_grad = tvg_linear_gradient_new();
        if (gradw) tvg_linear_gradient_set(p_grad, c_x, c_y - src_radius, c_x, c_y + src_radius);
        else tvg_linear_gradient_set(p_grad, c_x - src_radius, c_y, c_x + src_radius, c_y);
        tvg_gradient_set_color_stops(p_grad, g_cs, 2);
        if (full) tvg_shape_set_linear_gradient(paint, p_grad);
        else tvg_shape_set_stroke_linear_gradient(paint, p_grad);
        tvg_shape_set_stroke_width(paint, s_width);
    } else {
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
#endif

/*@brief 控件构造器初始化(子类型)
 *@param menial_maker 控件构造器实例
 */
void scui_menial_arc_maker(scui_menial_maker_t *menial_maker)
{
    /* 必须标记anima事件 */
    menial_maker->widget.style.sched_anima = true;
}

/*@brief 控件初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_arc_config(scui_menial_t *menial)
{
    /* 未配置使用默认值 */
    if (SCUI_IS_ZERO_VAL_F(menial->data.arc.angle_s) &&
        SCUI_IS_ZERO_VAL_F(menial->data.arc.angle_e)) {
        menial->data.arc.angle_s = 0.0f;
        menial->data.arc.angle_e = 360.0f;
    }
    
    /* 默认从零点开始 */
    scui_coord3_t angle_s = menial->data.arc.angle_s;
    scui_coord3_t angle_e = menial->data.arc.angle_e;
    menial->data.arc.angle_dist = scui_dist(angle_s, angle_e);
    menial->data.arc.angle_cur  = menial->data.arc.anti ? angle_e : angle_s;
    menial->data.arc.angle_way  = 0;
    
    scui_coord3_t angle_d = menial->data.arc.angle_dist;
    menial->data.arc.time = SCUI_WIDGET_MENIAL_ARC_TIME * angle_d / 360.0f;
    menial->data.arc.tick = 0;
}

/*@brief 控件反初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_arc_recycle(scui_menial_t *menial)
{
}

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 *@param anim   动画更新
 */
void scui_menial_arc_update_angle(scui_handle_t handle, scui_coord3_t angle, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    if (menial->type != scui_menial_type_arc) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    /* 动画更新 */
    if (anim) {
        scui_coord3_t angle_s = menial->data.arc.angle_s;
        scui_coord3_t angle_e = menial->data.arc.angle_e;
        scui_coord3_t angle_d = menial->data.arc.angle_dist;
        /* 让angle_cur到达angle即可 */
        menial->data.arc.angle_way = menial->data.arc.angle_cur <= angle ? +1.0f : -1.0f;
        scui_coord3_t angle_c = scui_dist(menial->data.arc.angle_cur, angle);
        menial->data.arc.tick = scui_map(angle_c, 0.0f, angle_d, 0, menial->data.arc.time);
        return;
    }
    
    /* 直接更新 */
    menial->data.arc.angle_cur = angle;
    menial->data.arc.angle_way = 0;
    menial->data.arc.tick = 0;
    scui_widget_draw(widget->myself, NULL, false);
}

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, 100.0f]
 *@param anim   动画更新
 */
void scui_menial_arc_update_value(scui_handle_t handle, scui_coord3_t value, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    if (menial->type != scui_menial_type_arc) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    /*  */
    scui_coord3_t angle_s = menial->data.arc.angle_s;
    scui_coord3_t angle_e = menial->data.arc.angle_e;
    scui_menial_arc_update_angle(handle, menial->data.arc.anti ?
        scui_map(value, 100.0f, 0.0f, angle_s, angle_e) :
        scui_map(value, 0.0f, 100.0f, angle_s, angle_e), anim);
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_arc_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        if (menial->data.arc.tick == 0)
            break;
        if (menial->data.arc.tick < 0) {
            menial->data.arc.tick = 0;
            break;
        }
        
        scui_coord_t tick = event->tick;
        menial->data.arc.tick -= tick;
        
        scui_coord3_t angle_d = menial->data.arc.angle_dist;
        scui_coord3_t angle_c = scui_map(tick, 0, menial->data.arc.time, 0.0f, angle_d);
        menial->data.arc.angle_cur += menial->data.arc.angle_way * angle_c;
        scui_widget_draw(widget->myself, NULL, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_draw_dsc_t draw_dsc = {0};
        draw_dsc.type = scui_draw_type_pixel_tvg;
        draw_dsc.graph.src_center = menial->data.arc.center;
        draw_dsc.graph.src_width  = menial->data.arc.width;
        draw_dsc.graph.src_radius = menial->data.arc.radius;
        
        draw_dsc.graph.src_tvg_cb    = scui_menial_tvg_cb;
        draw_dsc.graph.src_tvg_round = menial->data.arc.round;
        draw_dsc.graph.src_tvg_gradw = menial->data.arc.gradw;
        draw_dsc.graph.src_tvg_grad  = menial->data.arc.grad;
        
        draw_dsc.graph.src_center.x += widget->clip.x;
        draw_dsc.graph.src_center.y += widget->clip.y;
        draw_dsc.graph.src_radius   -= menial->data.arc.width / 2 + 1;
        
        /* 绘制背景: */
        draw_dsc.graph.src_angle_s = menial->data.arc.angle_s;
        draw_dsc.graph.src_angle_e = menial->data.arc.angle_e;
        scui_widget_draw_graph(widget->myself, NULL,
            widget->alpha, menial->data.arc.color[0], &draw_dsc);
        
        /* 绘制前景: */
        if (menial->data.arc.anti) {
            draw_dsc.graph.src_angle_s = menial->data.arc.angle_cur;
            draw_dsc.graph.src_angle_e = menial->data.arc.angle_e;
        } else {
            draw_dsc.graph.src_angle_s = menial->data.arc.angle_s;
            draw_dsc.graph.src_angle_e = menial->data.arc.angle_cur;
        }
        
        scui_widget_draw_graph(widget->myself, NULL,
            widget->alpha, menial->data.arc.color[1], &draw_dsc);
        
        break;
    }
    default:
        break;
    }
}
