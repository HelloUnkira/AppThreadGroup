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
    scui_area_t    *src_area    = &draw_dsc->graph.src_area;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_coord_t    src_radius  =  draw_dsc->graph.src_radius;
    /* draw dsc args<e> */
    
    scui_coord_t src_radius_max = scui_min(src_area->w, src_area->h) / 2;
    if (src_radius < 0) src_radius = src_radius_max;
    src_radius = scui_clamp(src_radius, 0, src_radius_max);
    
    Tvg_Canvas   *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t  offset = draw_dsc->graph.src_tvg_offset;
    scui_sbitfd_t gradw  = draw_dsc->graph.src_tvg_gradw;
    scui_sbitfd_t grad   = draw_dsc->graph.src_tvg_grad;
    
    /* 固定:取用画笔并配置信息 */
    Tvg_Paint *paint = tvg_shape_new();
    tvg_paint_set_opacity(paint, src_alpha);
    
    scui_coord_t x = src_area->x - offset.x;
    scui_coord_t y = src_area->y - offset.y;
    scui_coord_t w = src_area->w;
    scui_coord_t h = src_area->h;
    scui_coord_t r = src_radius;
    
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
        if (gradw) tvg_linear_gradient_set(p_grad, x, y, x, y + h);
        else tvg_linear_gradient_set(p_grad, x, y, x + w, y);
        tvg_gradient_set_color_stops(p_grad, g_cs, 2);
        tvg_shape_set_linear_gradient(paint, p_grad);
    } else {
        uint8_t f_a = 0, f_r = 0, f_g = 0, f_b = 0;
        f_a = src_color.color.ch.a;
        f_r = src_color.color.ch.r;
        f_g = src_color.color.ch.g;
        f_b = src_color.color.ch.b;
        
        tvg_shape_set_fill_color(paint, f_r, f_g, f_b, f_a);
    }
    
    tvg_canvas_push(canvas, paint);
}
#endif

/*@brief 控件构造器初始化(子类型)
 *@param menial_maker 控件构造器实例
 */
void scui_menial_bar_maker(scui_menial_maker_t *menial_maker)
{
    /* 必须标记anima事件 */
    /* 条件标记ptr事件 */
    menial_maker->widget.style.sched_anima = true;
    menial_maker->widget.style.indev_ptr   = menial_maker->data.bar.slider;
}

/*@brief 控件初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_bar_config(scui_menial_t *menial)
{
    /* 未配置使用默认值 */
    if (SCUI_IS_ZERO_VAL_F(menial->data.bar.value_lim))
        menial->data.bar.value_lim = 100.0f;
    
    scui_coord3_t value_d = menial->data.bar.value_lim;
    menial->data.bar.time = SCUI_WIDGET_MENIAL_BAR_TIME * value_d / 100.0f;
    menial->data.bar.tick = 0;
}

/*@brief 控件反初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_bar_recycle(scui_menial_t *menial)
{
}

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, value_lim]
 *@param anim   动画更新
 */
void scui_menial_bar_update_value(scui_handle_t handle, scui_coord3_t value, bool anim)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    if (menial->type != scui_menial_type_bar) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    value = scui_clamp(value, 0.0f, menial->data.bar.value_lim);
    
    /* 这可以实现丝滑到分段效果 */
    if (menial->data.bar.value_int)
        value = (scui_coord_t)value;
    
    /* 动画更新 */
    if (anim) {
        menial->data.bar.value_way = menial->data.bar.value_cur <= value ? +1.0f : -1.0f;
        /* 让vaule_cur到达value即可 */
        scui_coord3_t value_d = menial->data.bar.value_lim;
        scui_coord3_t value_c = scui_dist(menial->data.bar.value_cur, value);
        menial->data.bar.tick = scui_map(value_c, 0.0f, value_d, 0, menial->data.bar.time);
        return;
    }
    
    /* 直接更新 */
    menial->data.bar.value_cur = value;
    menial->data.bar.value_way = 0;
    menial->data.bar.tick = 0;
    scui_widget_draw(widget->myself, NULL, false);
}

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度
 *@param anim   动画更新
 */
void scui_menial_bar_current_value(scui_handle_t handle, scui_coord3_t *value)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    if (menial->type != scui_menial_type_bar) {
        SCUI_LOG_ERROR("error invoke");
        return;
    }
    
    *value = menial->data.bar.value_cur;
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_bar_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        if (menial->data.bar.tick == 0)
            break;
        if (menial->data.bar.tick < 0) {
            menial->data.bar.tick = 0;
            break;
        }
        
        scui_coord_t tick = event->tick;
        menial->data.bar.tick -= tick;
        
        scui_coord3_t value_d = menial->data.bar.value_lim;
        scui_coord3_t value_c = scui_map(tick, 0, menial->data.bar.time, 0.0f, value_d);
        menial->data.bar.value_cur += menial->data.bar.value_way * value_c;
        scui_widget_draw(widget->myself, NULL, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_draw_dsc_t draw_dsc = {0};
        draw_dsc.type = scui_draw_type_pixel_tvg;
        draw_dsc.graph.src_radius = menial->data.bar.radius;
        draw_dsc.graph.src_tvg_cb = scui_menial_tvg_cb;
        
        /* 渐变方向与绘制方向是一致的 */
        draw_dsc.graph.src_tvg_gradw = menial->data.bar.way;
        draw_dsc.graph.src_tvg_grad  = menial->data.bar.grad;
        
        /* 绘制背景: */
        scui_area_t src_area = widget->clip;
        draw_dsc.graph.src_area = src_area;
        
        scui_widget_draw_graph(widget->myself, NULL,
            widget->alpha, menial->data.bar.color[0], &draw_dsc);
        
        /* 绘制前景: */
        src_area = widget->clip;
        scui_coord3_t value_d = menial->data.bar.value_lim;
        scui_coord3_t value_c = menial->data.bar.value_cur;
        scui_coord3_t value_m = menial->data.bar.radius * 2;
        scui_coord3_t value_l = scui_min(src_area.w, src_area.h);
        if (value_m < 0) value_m = value_l;
        value_m = scui_clamp(value_m, 0, value_l);
        
        if (menial->data.bar.way) {
            src_area.h = value_m + scui_map(value_c, 0.0f, value_d, 0, src_area.h - value_m);
            src_area.h = scui_clamp(src_area.h, value_m, widget->clip.h);
        } else {
            src_area.w = value_m + scui_map(value_c, 0.0f, value_d, 0, src_area.w - value_m);
            src_area.w = scui_clamp(src_area.w, value_m, widget->clip.w);
        }
        draw_dsc.graph.src_area = src_area;
        
        scui_widget_draw_graph(widget->myself, NULL,
            widget->alpha, menial->data.bar.color[1], &draw_dsc);
        
        break;
    }
    case scui_event_ptr_move: {
        if (!scui_widget_event_inside(event) &&
            !widget->state.indev_ptr_hold)
             break;
        if (!menial->data.bar.slider)
             break;
        
        scui_event_mask_over(event);
        widget->state.indev_ptr_hold = true;
        
        scui_point_t ptr_c = event->ptr_e;
        scui_area_t  src_area = widget->clip;
        scui_area_m_to_s(&src_area, &src_area);
        
        scui_coord3_t value_c = 0.0f;
        scui_coord3_t value_d = menial->data.bar.value_lim;
        /* 取最后的落点计算百分比值就地更新 */
        if (menial->data.bar.way) {
            value_c = scui_map(ptr_c.y, src_area.y1, src_area.y2, 0.0f, value_d);
            value_c = scui_clamp(value_c, 0.0f, value_d);
        } else {
            value_c = scui_map(ptr_c.x, src_area.x1, src_area.x2, 0.0f, value_d);
            value_c = scui_clamp(value_c, 0.0f, value_d);
        }
        
        scui_menial_bar_update_value(widget->myself, value_c, false);
        break;
    }
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        widget->state.indev_ptr_hold = false;
        break;
    default:
        break;
    }
}
