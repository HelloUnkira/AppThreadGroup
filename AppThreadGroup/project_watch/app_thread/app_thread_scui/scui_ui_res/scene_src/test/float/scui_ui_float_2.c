/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_2_button_event_proc(scui_event_t *event)
{
    // 转移至控件调度
    if (event->type < scui_event_widget_s ||
        event->type > scui_event_widget_e) {
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
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw:
        break;
    default:
        #if 0   // discard, we don't need this
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_grasp_ptr(event);
        if (event->type >= scui_event_key_s && event->type <= scui_event_key_e)
            scui_window_float_event_grasp_key(event);
        #endif
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_c_event_proc(scui_event_t *event)
{
    static scui_coord_t indicator_index = 0;
    static scui_coord_t progressbar_s = 0;
    static scui_coord_t progressbar_e = 0;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        static uint8_t cnt = 0;
        cnt++;
        
        if (cnt % 10 == 0) {
            indicator_index++;
            if (indicator_index >= 5)
                indicator_index  = 0;
            progressbar_s = (uint32_t)scui_rand(100) % 45;
            progressbar_e = 100 - (uint32_t)scui_rand(100) % 45;
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip_hor     = {0};
        scui_area_t clip_ver     = {0};
        scui_point_t offset_hor  = {0};
        scui_point_t offset_ver  = {0};
        scui_color_t color_black = {0};
        
        scui_handle_t wait  = scui_image_prj_image_src_repeat_dot_01_greybmp;
        scui_handle_t focus = scui_image_prj_image_src_repeat_dot_02_whitebmp;
        
        offset_hor.x = 15;
        offset_hor.y = SCUI_VER_RES - 30;
        clip_hor = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_hor, &offset_hor))
            scui_custom_draw_indicator(event, &clip_hor, wait, color_black, focus, color_black,
                                             5, indicator_index, 6, false);
        
        offset_ver.x = 15;
        offset_ver.y = SCUI_VER_RES - 150;
        clip_ver = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_ver, &offset_ver))
            scui_custom_draw_indicator(event, &clip_ver, wait, color_black, focus, color_black,
                                             5, indicator_index, 6, true);
        
        scui_handle_t bar  = scui_image_prj_image_src_repeat_03_barbmp;
        scui_handle_t edge = scui_image_prj_image_src_repeat_05_dotbmp;
        scui_color_t color_bar  = {.color.full = 0xFF4F4F4F,};
        scui_color_t color_edge = {.color.full = 0xFFFFFFFF,};
        
        #if 0   // 没有水平滚动条背景
        offset_hor.x = 15;
        offset_hor.y = SCUI_VER_RES - 40;
        clip_hor = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_hor, &offset_hor))
            scui_custom_draw_slider(event, &clip_hor, bar, color_bar, edge, color_edge, 0, 100, progressbar_s, progressbar_e, 152, true);
        #endif
        
        offset_ver.x = 5;
        offset_ver.y = SCUI_VER_RES - 160;
        clip_ver = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_ver, &offset_ver))
            scui_custom_draw_slider(event, &clip_ver, bar, color_bar, edge, color_edge, 0, 100, progressbar_s, progressbar_e, 152, false);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        scui_color_t color_mix = {
            .color_s.full = 0xFF00FF00,
            .color_e.full = 0xFF0000FF,
        };
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        clip = scui_widget_clip(event->object);
        clip.x += 15;
        clip.y += 15;
        clip.w -= 15 * 2;
        clip.h -= 15 * 2;
        clip.h /= 2;
        clip.h -= 5;
        scui_widget_draw_color_grad(event->object, &clip, color_mix, false);
        
        clip = scui_widget_clip(event->object);
        clip.x += 15;
        clip.y += 15;
        clip.w -= 15 * 2;
        clip.h -= 15 * 2;
        clip.h /= 2;
        clip.y += clip.h + 5;
        clip.h -= 5;
        scui_widget_draw_color_grad(event->object, &clip, color_mix, true);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_2_event_proc(scui_event_t *event)
{
    static scui_multi_t image_pct = 0;
    static scui_point_t image_scale = {.x = 1024,.y = 1024,};
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        image_pct++;
        if (image_pct > 100)
            image_pct = 0;
        
        image_scale.x = scui_map(image_pct, 0, 100, 512, 1536);
        image_scale.y = scui_map(image_pct, 0, 100, 512, 1536);
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_handle_t image_handle = scui_image_prj_image_src_repeat_btn_22_retry_heartbmp;
        
        scui_point_t scale = image_scale;
        scui_widget_draw_image_scale(event->object, &clip, image_handle, NULL, scale, scui_opt_pos_c);
        
        break;
    }
    default:
        break;
    }
}


/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_3_event_proc(scui_event_t *event)
{
    static scui_coord_t image_ring_angle = 0;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        image_ring_angle += 1;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_point_t center = {
            .x = clip.x + clip.w / 2,
            .y = clip.y + clip.h / 2,
        };
        scui_coord_t  radius = clip.w / 2 - 20;
        scui_coord_t  angle  = image_ring_angle;
        scui_handle_t image_handle = scui_image_prj_image_src_repeat_dot_02_whitebmp;
        scui_custom_draw_ring_edge(event, NULL, &center, image_handle, color_mix, radius, angle);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_4_event_proc(scui_event_t *event)
{
    static scui_coord_t spinner_pct  = 0;
    static scui_coord_t spinner_cnt  = 0;
    static scui_coord_t spinner_tick = 750 / SCUI_ANIMA_TICK;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        spinner_cnt += event->tick;
        if (spinner_cnt >  spinner_tick)
            spinner_cnt -= spinner_tick;
        
        spinner_pct = scui_map(spinner_cnt, 0, spinner_tick, 0, 100);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_handle_t image_edge = scui_image_prj_image_src_19_widget_activity_04_dotbmp;
        scui_handle_t image_ring = scui_image_prj_image_src_19_widget_activity_05_ringbmp;
        
        clip = scui_widget_clip(event->object);
        clip.x += (clip.w - scui_image_w(image_ring)) / 2;
        clip.y += (clip.h - scui_image_h(image_ring)) / 2;
        clip.w = scui_image_w(image_ring);
        clip.h = scui_image_h(image_ring);
        
        scui_color_t color = {
            .color_l.full = 0xFFFFFFFF,
            .color_d.full = 0xFF404040,
            .filter = true,
        };
        
        scui_custom_draw_spinner(event, &clip, image_ring, color, image_edge,
                                 spinner_pct, 270, 60, +1);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_5_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        scui_button_maker_t button_maker = {0};
        scui_handle_t button_handle = SCUI_HANDLE_INVALID;
        button_maker.widget.type    = scui_widget_type_button;
        button_maker.widget.clip.w  = scui_widget_clip(SCUI_UI_SCENE_FLOAT_2_5).w - 15;
        button_maker.widget.clip.h  = 120;
        button_maker.widget.clip.x  = (scui_widget_clip(SCUI_UI_SCENE_FLOAT_2_5).w - button_maker.widget.clip.w) / 2;
        button_maker.widget.clip.y  = 10;
        button_maker.widget.clip.x += 10;
        button_maker.widget.clip.y += 10;
        button_maker.widget.clip.w -= 10 * 2;
        button_maker.widget.clip.h -= 10 * 2;
        button_maker.widget.parent = SCUI_UI_SCENE_FLOAT_2_5;
        button_maker.widget.event_cb = scui_ui_scene_2_button_event_proc;
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
        
        break;
    case scui_event_destroy:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        break;
    }
    default:
        break;
    }
}
