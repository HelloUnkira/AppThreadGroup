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
    
    Tvg_Canvas  *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t offset = draw_dsc->graph.src_tvg_offset;
    
    /* 固定:取用画笔并配置信息 */
    Tvg_Paint *paint = tvg_shape_new();
    tvg_paint_set_opacity(paint, src_alpha);
    
    uint8_t f_a = 0, f_r = 0, f_g = 0, f_b = 0;
    uint8_t s_a = 0, s_r = 0, s_g = 0, s_b = 0;
    scui_coord_t s_width = 0;
    
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
    
    scui_coord_t x = src_area->x - offset.x;
    scui_coord_t y = src_area->y - offset.y;
    scui_coord_t w = src_area->w;
    scui_coord_t h = src_area->h;
    scui_coord_t r = src_radius;
    
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
    } else {
        
        tvg_shape_move_to(paint, x + r, y);
        tvg_shape_line_to(paint, x + w - r, y);
        tvg_shape_append_arc(paint, x + w - r, y + r, r, 270, 90, 0);
        tvg_shape_line_to(paint, x + w, y + h - r);
        tvg_shape_append_arc(paint, x + w - r, y + h - r, r, 0, 90, 0);
        tvg_shape_line_to(paint, x + r, y + h);
        tvg_shape_append_arc(paint, x + r, y + h - r, r, 90, 90, 0);
        tvg_shape_line_to(paint, x, y + r);
        tvg_shape_append_arc(paint, x + r, y + r, r, 180, 90, 0);
    }
    
    tvg_shape_set_fill_color(paint, f_r, f_g, f_b, f_a);
    tvg_shape_set_stroke_color(paint, s_r, s_g, s_b, s_a);
    tvg_shape_set_stroke_width(paint, s_width);
    tvg_canvas_push(canvas, paint);
}
#endif

/*@brief 控件构造器初始化(子类型)
 *@param menial_maker 控件构造器实例
 */
void scui_menial_btn_maker(scui_menial_maker_t *menial_maker)
{
    /* 必须标记anima,widget,ptr事件 */
    menial_maker->widget.style.sched_anima  = true;
    menial_maker->widget.style.sched_widget = true;
    menial_maker->widget.style.indev_ptr    = true;
}

/*@brief 控件初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_config(scui_menial_t *menial)
{
    menial->data.btn.time = SCUI_WIDGET_MENIAL_BTN_TIME;
    menial->data.btn.lim  = SCUI_WIDGET_MENIAL_BTN_PCT;
    menial->data.btn.pct  = SCUI_WIDGET_MENIAL_BTN_PCT;
    menial->data.btn.way  = -1;
}

/*@brief 控件反初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_recycle(scui_menial_t *menial)
{
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_btn_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        if (menial->data.btn.fixed)
            break;
        
        /* 按下抬起动画结束, 回到初始状态 */
        if (menial->data.btn.way == -1 && menial->data.btn.pct <= menial->data.btn.lim) {
            /* 有点击标记发送点击事件 */
            if (menial->data.btn.click) {
                menial->data.btn.click = false;
                
                scui_event_define(event, widget->myself, true, scui_event_widget_button_click, NULL);
                scui_event_notify(&event);
            }
            /* 有选中标记且当次未选中 */
            if (menial->data.btn.check)
            if (menial->data.btn.check_c) {
                menial->data.btn.check_c = false;
                menial->data.btn.state = ~menial->data.btn.state;
                SCUI_LOG_INFO("state:%d", menial->data.btn.state);
                scui_widget_draw(event->object, NULL, false);
            }
            break;
        }
        /* 按下动画pct:[lim->100] */
        if (menial->data.btn.way == +1 && menial->data.btn.pct < 100) {
            /* 仅在动画中让tick计时 */
            menial->data.btn.tick += event->tick;
            if (menial->data.btn.tick > menial->data.btn.time)
                menial->data.btn.tick = menial->data.btn.time;
            /* 将pct根据时间映射到[lim,100] */
            menial->data.btn.pct = scui_map(menial->data.btn.tick,
                0, menial->data.btn.time, menial->data.btn.lim, 100);
            
            SCUI_LOG_INFO("<%d, %d>", menial->data.btn.pct, menial->data.btn.way);
            scui_widget_draw(event->object, NULL, false);
            break;
        }
        /* 按下动画无条件跑完 */
        if (menial->data.btn.way == +1 && menial->data.btn.pct < 100)
            break;
        
        /* 持续按下时最多允许pct:[lim->100] */
        if (menial->data.btn.hold)
            break;
        
        /* 按下动画跑完且抬起时:切换进动方向 */
        if (menial->data.btn.way == +1) {
            menial->data.btn.way  = -1;
            menial->data.btn.tick = +0;
            SCUI_ASSERT(menial->data.btn.pct == 100);
        }
        
        /* 抬起动画pct:[lim<-100] */
        if (menial->data.btn.way == -1 && menial->data.btn.pct > menial->data.btn.lim) {
            /* 仅在动画中让tick计时 */
            menial->data.btn.tick += event->tick;
            if (menial->data.btn.tick > menial->data.btn.time)
                menial->data.btn.tick = menial->data.btn.time;
            /* 将pct根据时间映射到[lim,100] */
            menial->data.btn.pct = scui_map(menial->data.btn.tick,
                0, menial->data.btn.time, 100, menial->data.btn.lim);
            
            SCUI_LOG_INFO("<%d, %d>", menial->data.btn.pct, menial->data.btn.way);
            scui_widget_draw(event->object, NULL, false);
            break;
        }
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_coord_t pct_l = menial->data.btn.lim;
        scui_coord_t pct_c = menial->data.btn.pct;
        if (menial->data.btn.fixed) pct_c = 100;
        SCUI_LOG_INFO("<%d>", menial->data.btn.pct);
        
        scui_area_t src_area = widget->clip;
        scui_coord_t scale_w = src_area.w * pct_c / 100;
        scui_coord_t scale_h = src_area.h * pct_c / 100;
        scale_w -= menial->data.btn.width * 2;
        scale_h -= menial->data.btn.width * 2;
        src_area.x += (src_area.w - scale_w) / 2;
        src_area.y += (src_area.h - scale_h) / 2;
        src_area.w  = scale_w;
        src_area.h  = scale_h;
        
        scui_color_t src_color = {0};
        scui_color32_t color_l = menial->data.btn.color[menial->data.btn.state].color_l;
        scui_color32_t color_d = menial->data.btn.color[menial->data.btn.state].color_d;
        scui_color32_mix_with(&src_color.color, &color_l, &color_d, scui_map(pct_c, pct_l, 100, 100, 0));
        
        scui_draw_dsc_t draw_dsc = {0};
        draw_dsc.type = scui_draw_type_pixel_tvg;
        draw_dsc.graph.src_area   = src_area;
        draw_dsc.graph.src_width  = menial->data.btn.width;
        draw_dsc.graph.src_radius = menial->data.btn.radius;
        draw_dsc.graph.src_tvg_cb = scui_menial_tvg_cb;
        
        scui_widget_draw_graph(widget->myself, NULL,
            widget->alpha, src_color, &draw_dsc);
        
        break;
    }
    case scui_event_ptr_down: {
        if (!scui_widget_event_inside(event))
             break;
        
        menial->data.btn.check_c = true;
        if (menial->data.btn.fixed)
            break;
        
        SCUI_LOG_INFO("");
        menial->data.btn.hold = true;
        menial->data.btn.tick = 0;
        menial->data.btn.pct  = menial->data.btn.lim;
        menial->data.btn.way  = +1;
        break;
    }
    case scui_event_ptr_move:
    case scui_event_ptr_up: {
        if (menial->data.btn.fixed) {
            
            /* 有选中标记且当次未选中 */
            if (menial->data.btn.check)
            if (menial->data.btn.check_c) {
                menial->data.btn.check_c = false;
                menial->data.btn.state = ~menial->data.btn.state;
                SCUI_LOG_INFO("state:%d", menial->data.btn.state);
                scui_widget_draw(event->object, NULL, false);
            }
            break;
        }
        
        SCUI_LOG_INFO("");
        menial->data.btn.hold = false;
        break;
    }
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        
        if (menial->data.btn.fixed) {
            
            scui_event_define(event, widget->myself, true, scui_event_widget_button_click, NULL);
            scui_event_notify(&event);
            break;
        }
        
        SCUI_LOG_INFO("");
        menial->data.btn.hold  = true;
        menial->data.btn.click = true;
        break;
    }
    default:
        break;
    }
}
