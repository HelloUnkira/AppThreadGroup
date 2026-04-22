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

/*@brief 属性过渡配置
 *@param menial 控件实例
 */
static void scui_mem_btn_prop_tran_def_pre(scui_menial_t *menial)
{
    scui_widget_t *widget = (void *)menial;
    
    scui_object_prop_t prop_def = {0};
    scui_object_prop_t prop_pre = {0};
    scui_object_tran_t tran_def = {0};
    scui_object_tran_t tran_pre = {0};
    prop_def.part  = scui_object_part_main;
    prop_pre.part  = scui_object_part_main;
    prop_def.state = scui_object_state_def;
    prop_pre.state = scui_object_state_pre;
    tran_def.state_p = scui_object_state_pre;
    tran_def.state_n = scui_object_state_def;
    tran_pre.state_p = scui_object_state_def;
    tran_pre.state_n = scui_object_state_pre;
    tran_def.part = scui_object_part_main;
    tran_pre.part = scui_object_part_main;
    tran_def.time = menial->data.btn.time;
    tran_pre.time = menial->data.btn.time;
    
    /* color bg prop */
    prop_def.style = scui_object_style_color_bg;
    prop_pre.style = scui_object_style_color_bg;
    prop_def.data.color32 = menial->data.btn.color[0].color_l;
    prop_pre.data.color32 = menial->data.btn.color[0].color_d;
    scui_object_prop_add(widget->myself, &prop_def);
    scui_object_prop_add(widget->myself, &prop_pre);
    /* color bg tran */
    tran_def.style = scui_object_style_color_bg;
    tran_pre.style = scui_object_style_color_bg;
    scui_object_tran_add_by(widget->myself, &tran_def);
    scui_object_tran_add_by(widget->myself, &tran_pre);
    
    /* width && height prop */
    prop_def.style = scui_object_style_width;
    prop_pre.style = scui_object_style_width;
    prop_def.data.number = menial->data.btn.lim;
    prop_pre.data.number = 100;
    scui_object_prop_add(widget->myself, &prop_def);
    scui_object_prop_add(widget->myself, &prop_pre);
    prop_def.style = scui_object_style_height;
    prop_pre.style = scui_object_style_height;
    prop_def.data.number = menial->data.btn.lim;
    prop_pre.data.number = 100;
    scui_object_prop_add(widget->myself, &prop_def);
    scui_object_prop_add(widget->myself, &prop_pre);
    
    if (menial->data.btn.fixed)
        return;
    
    /* width && height tran */
    tran_def.style = scui_object_style_width;
    tran_pre.style = scui_object_style_width;
    scui_object_tran_add_by(widget->myself, &tran_def);
    scui_object_tran_add_by(widget->myself, &tran_pre);
    tran_def.style = scui_object_style_height;
    tran_pre.style = scui_object_style_height;
    scui_object_tran_add_by(widget->myself, &tran_def);
    scui_object_tran_add_by(widget->myself, &tran_pre);
}

/*@brief 属性过渡配置
 *@param menial 控件实例
 */
static void scui_mem_btn_prop_tran_chk_pre(scui_menial_t *menial)
{
    scui_widget_t *widget = (void *)menial;
    
    scui_object_prop_t prop_chk = {0};
    scui_object_prop_t prop_pre = {0};
    scui_object_tran_t tran_chk = {0};
    scui_object_tran_t tran_pre = {0};
    prop_chk.part  = scui_object_part_main;
    prop_pre.part  = scui_object_part_main;
    prop_chk.state = scui_object_state_chk;
    prop_pre.state = scui_object_state_pre;
    tran_chk.state_p = scui_object_state_pre;
    tran_chk.state_n = scui_object_state_chk;
    tran_pre.state_p = scui_object_state_chk;
    tran_pre.state_n = scui_object_state_pre;
    tran_chk.part = scui_object_part_main;
    tran_pre.part = scui_object_part_main;
    tran_chk.time = menial->data.btn.time;
    tran_pre.time = menial->data.btn.time;
    
    /* color bg prop */
    prop_chk.style = scui_object_style_color_bg;
    prop_pre.style = scui_object_style_color_bg;
    prop_chk.data.color32 = menial->data.btn.color[1].color_l;
    prop_pre.data.color32 = menial->data.btn.color[1].color_d;
    scui_object_prop_add(widget->myself, &prop_chk);
    scui_object_prop_add(widget->myself, &prop_pre);
    /* color bg tran */
    tran_chk.style = scui_object_style_color_bg;
    tran_pre.style = scui_object_style_color_bg;
    scui_object_tran_add_by(widget->myself, &tran_chk);
    scui_object_tran_add_by(widget->myself, &tran_pre);
    
    /* width && height prop */
    prop_chk.style = scui_object_style_width;
    prop_pre.style = scui_object_style_width;
    prop_chk.data.number = menial->data.btn.lim;
    prop_pre.data.number = 100;
    scui_object_prop_add(widget->myself, &prop_chk);
    scui_object_prop_add(widget->myself, &prop_pre);
    prop_chk.style = scui_object_style_height;
    prop_pre.style = scui_object_style_height;
    prop_chk.data.number = menial->data.btn.lim;
    prop_pre.data.number = 100;
    scui_object_prop_add(widget->myself, &prop_chk);
    scui_object_prop_add(widget->myself, &prop_pre);
    
    if (menial->data.btn.fixed)
        return;
    
    /* width && height tran */
    tran_chk.style = scui_object_style_width;
    tran_pre.style = scui_object_style_width;
    scui_object_tran_add_by(widget->myself, &tran_chk);
    scui_object_tran_add_by(widget->myself, &tran_pre);
    tran_chk.style = scui_object_style_height;
    tran_pre.style = scui_object_style_height;
    scui_object_tran_add_by(widget->myself, &tran_chk);
    scui_object_tran_add_by(widget->myself, &tran_pre);
}


/*@brief 控件构造器初始化(子类型)
 *@param menial_maker 控件构造器实例
 */
void scui_menial_btn_maker(scui_menial_maker_t *menial_maker)
{
    /* 必须标记widget事件 */
    menial_maker->widget.style.sched_widget = true;
    
    menial_maker->object.prop_num = 20;
    menial_maker->object.tran_num = 40;
    menial_maker->object.check = menial_maker->data.btn.check;
}

/*@brief 控件初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_btn_config(scui_menial_t *menial)
{
    /* 未配置使用默认值 */
    if (menial->data.btn.time == 0)
        menial->data.btn.time  = SCUI_WIDGET_MENIAL_BTN_TIME;
    
    /* 未配置使用默认值 */
    if (menial->data.btn.lim == 0)
        menial->data.btn.lim  = SCUI_WIDGET_MENIAL_BTN_PCT;
    if (menial->data.btn.fixed)
        menial->data.btn.lim  = 100;
    
    scui_mem_btn_prop_tran_def_pre(menial);
    if (menial->data.btn.check)
    scui_mem_btn_prop_tran_chk_pre(menial);
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
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_object_prop_t prop = {0};
        prop.part  = scui_object_part_main;
        prop.style = scui_object_style_color_bg;
        scui_object_prop_sync(event->object, &prop);
        scui_color_t src_color = {.color = prop.data.color32};
        prop.style = scui_object_style_width;
        scui_object_prop_sync(event->object, &prop);
        scui_coord_t pct_w = prop.data.number;
        prop.style = scui_object_style_height;
        scui_object_prop_sync(event->object, &prop);
        scui_coord_t pct_h = prop.data.number;
        SCUI_LOG_INFO("<%d,%d>", pct_w, pct_h);
        
        scui_area_t src_area = widget->clip;
        scui_coord_t scale_w = src_area.w * pct_w / 100;
        scui_coord_t scale_h = src_area.h * pct_h / 100;
        scale_w -= menial->data.btn.width * 2;
        scale_h -= menial->data.btn.width * 2;
        src_area.x += (src_area.w - scale_w) / 2;
        src_area.y += (src_area.h - scale_h) / 2;
        src_area.w  = scale_w;
        src_area.h  = scale_h;
        
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
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        break;
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        
        /* 这里是直接响应的,要不要延迟到抬起? */
        scui_event_define(event, widget->myself, true, scui_event_widget_button_click, NULL);
        scui_event_notify(&event);
        break;
    }
    default:
        break;
    }
}
