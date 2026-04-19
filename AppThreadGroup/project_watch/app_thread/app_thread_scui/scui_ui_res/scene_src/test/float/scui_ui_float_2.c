/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_slider_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        scui_custom_data_t *data = NULL;
        scui_custom_data_inst(event->object, &data);
        
        data->slider.cmin = (uint32_t)scui_rand(100) % 45;
        data->slider.cmax = 100 - (uint32_t)scui_rand(100) % 45;
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    }
    
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_spinner_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        scui_custom_data_t *data = NULL;
        scui_custom_data_inst(event->object, &data);
        
        static scui_coord_t spinner_cnt  = 0;
        static scui_coord_t spinner_tick = 750;
        spinner_cnt += event->tick;
        if (spinner_cnt >  spinner_tick)
            spinner_cnt -= spinner_tick;
        
        data->spinner.percent = scui_map(spinner_cnt, 0, spinner_tick, 0, 100);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    }
    
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_indicator_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        scui_custom_data_t *data = NULL;
        scui_custom_data_inst(event->object, &data);
        
        static uint8_t cnt = 0; cnt++;
        
        if (cnt % 10 == 0 || cnt % 11 == 0) {
            data->indicator.index++;
            if (data->indicator.index >= 5)
                data->indicator.index  = 0;
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    }
    }
    
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_ring_edge_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        scui_custom_data_t *data = NULL;
        scui_custom_data_inst(event->object, &data);
        
        data->ring_edge.angle += 1;
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    }
    
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_button_event_proc(scui_event_t *event)
{
    // 转移至控件调度
    if (!scui_event_type_widget(event->type)) {
         scui_widget_event_shift(event);
         return;
    }
    
    SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_event_proc(scui_event_t *event)
{
    static scui_multi_t image_pct = 0;
    static scui_point_t image_scale = {.x = 1024,.y = 1024,};
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        image_pct++;
        if (image_pct > 100)
            image_pct = 0;
        
        image_scale.x = scui_map(image_pct, 0, 100, 512, 1536);
        image_scale.y = scui_map(image_pct, 0, 100, 512, 1536);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_custom_data_t custom_data_zero = {0};
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.type = scui_widget_type_custom;
        custom_maker.widget.parent = event->object;
        custom_maker.widget.style.fully_bg = true;
        
        #if 1
        custom_maker.type = scui_custom_type_slider;
        custom_maker.data = custom_data_zero;
        custom_maker.data.slider.bar  = scui_image_prj_image_src_repeat_03_barbmp;
        custom_maker.data.slider.edge = scui_image_prj_image_src_repeat_05_dotbmp;
        custom_maker.data.slider.color_bar.color.full  = 0xFF4F4F4F;
        custom_maker.data.slider.color_edge.color.full = 0xFFFFFFFF;
        custom_maker.data.slider.vmin = 0;
        custom_maker.data.slider.vmax = 100;
        custom_maker.data.slider.dist = 152;    // 152 没有水平滚动条背景
        custom_maker.data.slider.way  = 0;      // 1
        custom_maker.widget.clip.x = 5;
        custom_maker.widget.clip.y = SCUI_VER_RES - 160;
        custom_maker.widget.clip.w = scui_image_w(custom_maker.data.slider.bar);
        custom_maker.widget.clip.h = scui_image_h(custom_maker.data.slider.bar);
        custom_maker.widget.event_cb = scui_ui_scene_float_2_slider_event_proc;
        scui_widget_create(&custom_maker, &custom_handle);
        #endif
        
        #if 1
        custom_maker.widget.clip.x = SCUI_HOR_RES * 1 / 13 + 10;
        custom_maker.widget.clip.y = SCUI_VER_RES * 5 / 13 + 10;
        custom_maker.widget.clip.w = SCUI_HOR_RES * 5 / 13 - 10 * 2;
        custom_maker.widget.clip.h = SCUI_VER_RES * 5 / 13 - 10 * 2;
        custom_maker.type = scui_custom_type_spinner;
        custom_maker.data = custom_data_zero;
        custom_maker.data.spinner.spinner = scui_image_prj_image_src_19_widget_activity_05_ringbmp;
        custom_maker.data.spinner.edge = scui_image_prj_image_src_19_widget_activity_04_dotbmp;
        custom_maker.data.spinner.color.color_l.full = 0xFFFFFFFF;
        custom_maker.data.spinner.color.color_d.full = 0xFF404040;
        custom_maker.data.spinner.color.filter = true;
        custom_maker.data.spinner.angle_s = 270;
        custom_maker.data.spinner.angle_l = 60;
        custom_maker.data.spinner.way = 1;
        custom_maker.widget.clip.x += (custom_maker.widget.clip.w - scui_image_w(custom_maker.data.spinner.spinner)) / 2;
        custom_maker.widget.clip.y += (custom_maker.widget.clip.h - scui_image_h(custom_maker.data.spinner.spinner)) / 2;
        custom_maker.widget.clip.w = scui_image_w(custom_maker.data.spinner.spinner);
        custom_maker.widget.clip.h = scui_image_h(custom_maker.data.spinner.spinner);
        custom_maker.widget.event_cb = scui_ui_scene_float_2_spinner_event_proc;
        scui_widget_create(&custom_maker, &custom_handle);
        #endif
        
        #if 1
        custom_maker.type = scui_custom_type_indicator;
        custom_maker.data = custom_data_zero;
        custom_maker.data.indicator.wait = scui_image_prj_image_src_repeat_dot_01_greybmp;
        custom_maker.data.indicator.focus = scui_image_prj_image_src_repeat_dot_02_whitebmp;
        custom_maker.data.indicator.count = 5;
        custom_maker.data.indicator.span = 6;
        custom_maker.widget.event_cb = scui_ui_scene_float_2_indicator_event_proc;
        
        custom_maker.data.indicator.way = 0;
        custom_maker.widget.clip.x  = 15;
        custom_maker.widget.clip.y  = SCUI_VER_RES - 30;
        custom_maker.widget.clip.w  = scui_image_w(custom_maker.data.indicator.wait);
        custom_maker.widget.clip.w += custom_maker.data.indicator.span;
        custom_maker.widget.clip.w *= custom_maker.data.indicator.count;
        custom_maker.widget.clip.h  = scui_image_h(custom_maker.data.indicator.wait);
        scui_widget_create(&custom_maker, &custom_handle);
        
        custom_maker.data.indicator.way = 1;
        custom_maker.widget.clip.x  = 15;
        custom_maker.widget.clip.y  = SCUI_VER_RES - 150;
        custom_maker.widget.clip.w  = scui_image_w(custom_maker.data.indicator.wait);
        custom_maker.widget.clip.h  = scui_image_h(custom_maker.data.indicator.wait);
        custom_maker.widget.clip.h += custom_maker.data.indicator.span;
        custom_maker.widget.clip.h *= custom_maker.data.indicator.count;
        scui_widget_create(&custom_maker, &custom_handle);
        #endif
        
        #if 1
        custom_maker.widget.clip.x = SCUI_HOR_RES * 9 / 13 + 10;
        custom_maker.widget.clip.y = SCUI_VER_RES * 1 / 13 + 10;
        custom_maker.widget.clip.w = SCUI_HOR_RES * 3 / 13 - 10 * 2;
        custom_maker.widget.clip.h = SCUI_VER_RES * 3 / 13 - 10 * 2;
        custom_maker.widget.event_cb = scui_ui_scene_float_2_ring_edge_event_proc;
        custom_maker.type = scui_custom_type_ring_edge;
        custom_maker.data = custom_data_zero;
        custom_maker.data.ring_edge.image = scui_image_prj_image_src_repeat_dot_02_whitebmp;
        custom_maker.data.ring_edge.color.color.full = 0xFF00FF00;
        custom_maker.data.ring_edge.center.x = custom_maker.widget.clip.x + custom_maker.widget.clip.w / 2;
        custom_maker.data.ring_edge.center.y = custom_maker.widget.clip.y + custom_maker.widget.clip.h / 2;
        custom_maker.data.ring_edge.radius = custom_maker.widget.clip.w / 2 - 20;
        scui_widget_create(&custom_maker, &custom_handle);
        #endif
        
        
        
        #if 1
        scui_button_maker_t button_maker = {0};
        scui_handle_t button_handle = SCUI_HANDLE_INVALID;
        button_maker.widget.type = scui_widget_type_button;
        button_maker.widget.style.fully_bg = true;
        button_maker.widget.parent = event->object;
        button_maker.widget.event_cb = scui_ui_scene_float_2_button_event_proc;
        
        button_maker.widget.clip.w  = SCUI_HOR_RES * 5 / 13 - 15;
        button_maker.widget.clip.h  = 120;
        button_maker.widget.clip.x  = SCUI_HOR_RES * 7 / 13 + 15 / 2;
        button_maker.widget.clip.y  = SCUI_VER_RES * 5 / 13 + 10;
        button_maker.widget.clip.x += 10;
        button_maker.widget.clip.y += 10;
        button_maker.widget.clip.w -= 10 * 2;
        button_maker.widget.clip.h -= 10 * 2;
        button_maker.type = scui_button_type_pixel;
        button_maker.mode = scui_button_mode_scale;
        button_maker.pixel.color[0].color_s.full = 0xFF00FF00;
        button_maker.pixel.color[0].color_e.full = 0xFF008000;
        button_maker.pixel.color[1].color_s.full = 0xFF000080;
        button_maker.pixel.color[1].color_e.full = 0xFF000080;
        button_maker.pixel.color[2].color_s.full = 0xFF800000;
        button_maker.pixel.color[2].color_e.full = 0xFF800000;
        button_maker.pixel.color[3].color_s.full = 0xFF87CEFA;
        button_maker.pixel.color[3].color_e.full = 0xFF87CEFA;
        button_maker.pixel.alpha[0] = scui_alpha_cover;
        button_maker.pixel.alpha[1] = scui_alpha_cover;
        button_maker.pixel.alpha[2] = scui_alpha_cover;
        button_maker.pixel.alpha[3] = scui_alpha_cover;
        button_maker.pixel.width[1] = 3;
        button_maker.pixel.width[2] = 3;
        button_maker.pixel.width[3] = 13;
        button_maker.pixel.radius = 20;
        scui_widget_create(&button_maker, &button_handle);
        /* 该控件未完成,待定中... */
        #endif
        
        
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw: {
        // 无需叠加widget_clip, 它为0
        scui_area_t clip = {0};
        
        #if 1
        clip.x = SCUI_HOR_RES * 1 / 13;
        clip.y = SCUI_VER_RES * 1 / 13;
        clip.w = SCUI_HOR_RES * 3 / 13;
        clip.h = SCUI_VER_RES * 3 / 13;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_WHITE);
        clip.x = SCUI_HOR_RES * 5 / 13;
        clip.y = SCUI_VER_RES * 1 / 13;
        clip.w = SCUI_HOR_RES * 3 / 13;
        clip.h = SCUI_VER_RES * 3 / 13;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_WHITE);
        clip.x = SCUI_HOR_RES * 9 / 13;
        clip.y = SCUI_VER_RES * 1 / 13;
        clip.w = SCUI_HOR_RES * 3 / 13;
        clip.h = SCUI_VER_RES * 3 / 13;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_WHITE);
        clip.x = SCUI_HOR_RES * 1 / 13;
        clip.y = SCUI_VER_RES * 5 / 13;
        clip.w = SCUI_HOR_RES * 5 / 13;
        clip.h = SCUI_VER_RES * 5 / 13;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_WHITE);
        clip.x = SCUI_HOR_RES * 7 / 13;
        clip.y = SCUI_VER_RES * 5 / 13;
        clip.w = SCUI_HOR_RES * 5 / 13;
        clip.h = SCUI_VER_RES * 7 / 13;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_WHITE);
        #endif
        
        #if 1
        scui_color_t color_mix = {
            .color_s.full = 0xFF00FF00,
            .color_e.full = 0xFF0000FF,
        };
        
        clip.x = SCUI_HOR_RES * 1 / 13 + 10;
        clip.y = SCUI_VER_RES * 1 / 13 + 10;
        clip.w = SCUI_HOR_RES * 3 / 13 - 10 * 2;
        clip.h = SCUI_VER_RES * 3 / 13 - 10 * 2;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_BLACK);
        
        clip.x = SCUI_HOR_RES * 1 / 13 + 15;
        clip.y = SCUI_HOR_RES * 1 / 13 + 15;
        clip.w = SCUI_HOR_RES * 3 / 13 - 15 * 2;
        clip.h = SCUI_HOR_RES * 3 / 13 - 15 * 2;
        clip.h /= 2;
        clip.h -= 5;
        scui_widget_draw_color_grad(event->object, &clip, color_mix, false);
        scui_widget_draw_dither(event->object, &clip);
        
        clip.x = SCUI_HOR_RES * 1 / 13 + 15;
        clip.y = SCUI_VER_RES * 1 / 13 + 15;
        clip.w = SCUI_HOR_RES * 3 / 13 - 15 * 2;
        clip.h = SCUI_HOR_RES * 3 / 13 - 15 * 2;
        clip.h /= 2;
        clip.y += clip.h + 5;
        clip.h -= 5;
        scui_widget_draw_color_grad(event->object, &clip, color_mix, true);
        scui_widget_draw_dither(event->object, &clip);
        #endif
        
        #if 1
        clip.x = SCUI_HOR_RES * 5 / 13 + 10;
        clip.y = SCUI_VER_RES * 1 / 13 + 10;
        clip.w = SCUI_HOR_RES * 3 / 13 - 10 * 2;
        clip.h = SCUI_VER_RES * 3 / 13 - 10 * 2;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_BLACK);
        
        scui_handle_t image_handle = scui_image_prj_image_src_repeat_btn_22_retry_heartbmp;
        scui_widget_draw_image_scale(event->object, &clip, image_handle, NULL, image_scale, scui_opt_pos_c);
        #endif
        
        
        
        break;
    }
    default:
        #if 0   // discard, we don't need this
        if (scui_event_type_ptr(event->type))
            scui_window_float_event_grasp_ptr(event);
        if (scui_event_type_key(event->type))
            scui_window_float_event_grasp_key(event);
        #endif
        break;
    }
}
