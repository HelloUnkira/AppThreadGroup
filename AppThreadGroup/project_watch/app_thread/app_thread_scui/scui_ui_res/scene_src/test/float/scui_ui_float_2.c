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
    SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_window_float_event_grasp_show(event);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_window_float_event_grasp_hide(event);
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_draw:
        SCUI_LOG_INFO("scui_event_draw");
        break;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_grasp_ptr(event);
        if (event->type >= scui_event_key_s && event->type <= scui_event_key_e)
            scui_window_float_event_grasp_key(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
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
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
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
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t clip_hor     = {0};
        scui_area_t clip_ver     = {0};
        scui_point_t offset_hor  = {0};
        scui_point_t offset_ver  = {0};
        scui_color_t color_black = {0};
        
        scui_handle_t wait  = scui_image_prj_image_src_repeat_dot_01_greybmp;
        scui_handle_t focus = scui_image_prj_image_src_repeat_dot_02_whitebmp;
        
        offset_hor.x = 15;
        offset_hor.y = SCUI_DRV_VER_RES - 30;
        clip_hor = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_hor, &offset_hor))
            scui_custom_draw_indicator(event, &clip_hor, wait, color_black, focus, color_black,
                                             5, indicator_index, 6, true);
        
        offset_ver.x = 15;
        offset_ver.y = SCUI_DRV_VER_RES - 150;
        clip_ver = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_ver, &offset_ver))
            scui_custom_draw_indicator(event, &clip_ver, wait, color_black, focus, color_black,
                                             5, indicator_index, 6, false);
        
        scui_handle_t bar  = scui_image_prj_image_src_repeat_03_barbmp;
        scui_handle_t edge = scui_image_prj_image_src_repeat_05_dotbmp;
        scui_color_t color_bar  = {.color.full = 0xFF4F4F4F,};
        scui_color_t color_edge = {.color.full = 0xFFFFFFFF,};
        
        #if 0   // 没有水平滚动条背景
        offset_hor.x = 15;
        offset_hor.y = SCUI_DRV_VER_RES - 40;
        clip_hor = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_hor, &offset_hor))
            scui_custom_draw_slider(event, &clip_hor, bar, color_bar, edge, color_edge, 0, 100, progressbar_s, progressbar_e, 152, true);
        #endif
        
        offset_ver.x = 5;
        offset_ver.y = SCUI_DRV_VER_RES - 160;
        clip_ver = scui_widget_clip(event->object);
        if (scui_area_limit_offset(&clip_ver, &offset_ver))
            scui_custom_draw_slider(event, &clip_ver, bar, color_bar, edge, color_edge, 0, 100, progressbar_s, progressbar_e, 152, false);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
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
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
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
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
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
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        image_pct++;
        if (image_pct > 100)
            image_pct = 0;
        
        image_scale.x = scui_map(image_pct, 0, 100, 512, 1536);
        image_scale.y = scui_map(image_pct, 0, 100, 512, 1536);
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
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
        scui_image_t *image = scui_handle_get(image_handle);
        SCUI_ASSERT(image != NULL);
        
        scui_point_t scale = image_scale;
        scui_widget_draw_image_scale(event->object, &clip, image_handle, NULL, scale, scui_opt_pos_c);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
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
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        image_ring_angle += 1;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
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
        scui_custom_draw_ring_edge(event, &center, image_handle, color_mix, radius, angle);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
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
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        spinner_cnt++;
        if (spinner_cnt >  spinner_tick)
            spinner_cnt -= spinner_tick;
        
        spinner_pct = scui_map(spinner_cnt, 0, spinner_tick, 0, 100);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
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
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_5_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        break;
    }
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_objbtn_maker_t objbtn_maker = {0};
        scui_handle_t objbtn_handle = SCUI_HANDLE_INVALID;
        objbtn_maker.widget.type   = scui_widget_type_objbtn;
        objbtn_maker.widget.clip.w = scui_widget_clip(SCUI_UI_SCENE_FLOAT_2_5).w - 30;
        objbtn_maker.widget.clip.h = 90;
        objbtn_maker.widget.clip.x = (scui_widget_clip(SCUI_UI_SCENE_FLOAT_2_5).w - objbtn_maker.widget.clip.w) / 2;
        objbtn_maker.widget.clip.y = 10;
        objbtn_maker.widget.clip.x += 10;
        objbtn_maker.widget.clip.y += 10;
        objbtn_maker.widget.clip.w -= 10 * 2;
        objbtn_maker.widget.clip.h -= 10 * 2;
        objbtn_maker.widget.parent = SCUI_UI_SCENE_FLOAT_2_5;
        objbtn_maker.notify_cb = scui_ui_scene_2_button_event_proc;
        objbtn_maker.color[0].color_s.full = 0xFF00FF00;
        objbtn_maker.color[0].color_e.full = 0xFF008000;
        objbtn_maker.color[1].color_s.full = 0xFF0000FF;
        objbtn_maker.color[1].color_e.full = 0xFF000080;
        objbtn_maker.color[2].color_s.full = 0xFFFF0000;
        objbtn_maker.color[2].color_e.full = 0xFF800000;
        objbtn_maker.color[3].color_s.full = 0xFF808080;
        objbtn_maker.color[3].color_e.full = 0xFF404040;
        objbtn_maker.alpha[0] = scui_alpha_cover;
        objbtn_maker.alpha[1] = scui_alpha_cover;
        objbtn_maker.alpha[2] = scui_alpha_cover;
        objbtn_maker.alpha[3] = scui_alpha_cover;
        objbtn_maker.width[1] = 3;
        objbtn_maker.width[2] = 3;
        objbtn_maker.width[3] = 3;
        objbtn_maker.side[0]  = scui_opt_pos_all;
        objbtn_maker.side[1]  = scui_opt_pos_all;
        objbtn_maker.side[2]  = scui_opt_pos_all;
        objbtn_maker.side[3]  = scui_opt_pos_all;
        objbtn_maker.radius = 13;
        scui_widget_create(&objbtn_maker, &objbtn_handle, false);
        /* 该控件未完成,待定中... */
        
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
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
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
