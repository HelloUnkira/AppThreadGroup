/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_2_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
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
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw:
        SCUI_LOG_INFO("scui_event_draw");
        scui_widget_event_mask_keep(event);
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
        
        static uint8_t cnt = 0;
        cnt++;
        
        if (cnt % 10 == 0) {
            indicator_index++;
            if (indicator_index >= 5)
                indicator_index  = 0;
            progressbar_s = (uint32_t)scui_rand(0, 100) % 45;
            progressbar_e = 100 - (uint32_t)scui_rand(0, 100) % 45;
            scui_widget_draw(event->object, NULL, false);
        }
        
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
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
        clip_hor = scui_widget_surface_clip(event->object);
        if (scui_area_limit_offset(&clip_hor, &offset_hor))
            scui_custom_draw_image_indicator(event, &clip_hor, wait, color_black, focus, color_black,
                                             5, indicator_index, 6, true);
        
        offset_ver.x = 15;
        offset_ver.y = SCUI_DRV_VER_RES - 150;
        clip_ver = scui_widget_surface_clip(event->object);
        if (scui_area_limit_offset(&clip_ver, &offset_ver))
            scui_custom_draw_image_indicator(event, &clip_ver, wait, color_black, focus, color_black,
                                             5, indicator_index, 6, false);
        
        scui_handle_t bar  = scui_image_prj_image_src_repeat_03_barbmp;
        scui_handle_t edge = scui_image_prj_image_src_repeat_05_dotbmp;
        scui_color_t color_bar  = {.color.full = 0xFF4F4F4F,};
        scui_color_t color_edge = {.color.full = 0xFFFFFFFF,};
        
        #if 0   // 没有水平滚动条背景
        offset_hor.x = 15;
        offset_hor.y = SCUI_DRV_VER_RES - 40;
        clip_hor = scui_widget_surface_clip(event->object);
        if (scui_area_limit_offset(&clip_hor, &offset_hor))
            scui_custom_draw_image_slider(event, &clip_hor, bar, color_bar, edge, color_edge, 0, 100, progressbar_s, progressbar_e, 152, true);
        #endif
        
        offset_ver.x = 5;
        offset_ver.y = SCUI_DRV_VER_RES - 160;
        clip_ver = scui_widget_surface_clip(event->object);
        if (scui_area_limit_offset(&clip_ver, &offset_ver))
            scui_custom_draw_image_slider(event, &clip_ver, bar, color_bar, edge, color_edge, 0, 100, progressbar_s, progressbar_e, 152, false);
        
        scui_widget_event_mask_keep(event);
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
        scui_widget_event_mask_keep(event);
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
        
        clip = scui_widget_surface_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        clip = scui_widget_surface_clip(event->object);
        clip.x += 15;
        clip.y += 15;
        clip.w -= 15 * 2;
        clip.h -= 15 * 2;
        clip.h /= 2;
        clip.h -= 5;
        scui_custom_draw_area(event, &clip, color_mix, true, false);
        
        clip = scui_widget_surface_clip(event->object);
        clip.x += 15;
        clip.y += 15;
        clip.w -= 15 * 2;
        clip.h -= 15 * 2;
        clip.h /= 2;
        clip.y += clip.h + 5;
        clip.h -= 5;
        scui_custom_draw_area(event, &clip, color_mix, true, true);
        
        scui_widget_event_mask_keep(event);
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
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        
        clip = scui_widget_surface_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        scui_coord_t del_x = clip.w - 10;
        scui_coord_t del_y = clip.h - 10;
        scui_point_t pos_s = {0};
        scui_point_t pos_e = {0};
        scui_area_m_to_s(&clip);
        
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = pos_s.x;
        pos_e.y = clip.y2 - 10;
        scui_widget_surface_draw_line(event->object, 2, pos_s, pos_e, color_mix);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = pos_s.y;
        scui_widget_surface_draw_line(event->object, 2, pos_s, pos_e, color_mix);
        
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = pos_s.x + del_x / 2;
        pos_e.y = clip.y2 - 10;
        scui_widget_surface_draw_line(event->object, 2, pos_s, pos_e, color_mix);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = pos_s.y + del_y / 2;
        scui_widget_surface_draw_line(event->object, 2, pos_s, pos_e, color_mix);
        
        pos_s.x = clip.x2 - 10;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x1 + 5 + del_x / 2;
        pos_e.y = clip.y2 - 10;
        scui_widget_surface_draw_line(event->object, 2, pos_s, pos_e, color_mix);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y2 - 10;
        pos_e.x = clip.x2 - 10;
        pos_e.y = clip.y1 + 5 + del_y / 2;
        scui_widget_surface_draw_line(event->object, 2, pos_s, pos_e, color_mix);
        
        scui_widget_event_mask_keep(event);
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
        
        image_ring_angle += 1;
        scui_widget_draw(event->object, NULL, false);
        
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        
        clip = scui_widget_surface_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        scui_point_t center = {
            .x = clip.x + clip.w / 2,
            .y = clip.y + clip.h / 2,
        };
        scui_coord_t  radius = clip.w / 2 - 20;
        scui_coord_t  angle  = image_ring_angle;
        scui_handle_t image_handle = scui_image_prj_image_src_repeat_dot_02_whitebmp;
        scui_custom_draw_image_circle_rotate(event, &center, image_handle, color_mix, radius, angle);
        
        scui_widget_event_mask_keep(event);
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
        
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        
        clip = scui_widget_surface_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        scui_handle_t image_handle = scui_image_prj_image_src_repeat_btn_22_retry_heartbmp;
        scui_image_t *image = scui_handle_get(image_handle);
        SCUI_ASSERT(image != NULL);
        
        scui_point_t scale = image_scale;
        scui_widget_surface_draw_image_scale(event->object, &clip, image_handle, NULL, scale);
        
        scui_widget_event_mask_keep(event);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
