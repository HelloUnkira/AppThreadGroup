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
        
        scui_area_t clip = {0};
        scui_color_t color_black = {0};
        scui_color_t color_mix = {
            .color_s.full = 0xFF00FF00,
            .color_e.full = 0xFF0000FF,
        };
        
        clip = scui_widget_draw_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        clip = scui_widget_draw_clip(event->object);
        clip.x += 15;
        clip.y += 15;
        clip.w -= 15 * 2;
        clip.h -= 15 * 2;
        clip.h /= 2;
        clip.h -= 5;
        scui_custom_draw_area(event, &clip, color_mix, true, false);
        
        clip = scui_widget_draw_clip(event->object);
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
        
        clip = scui_widget_draw_clip(event->object);
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
        scui_custom_draw_line(event, 2, pos_s, pos_e, color_mix);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = pos_s.y;
        scui_custom_draw_line(event, 2, pos_s, pos_e, color_mix);
        
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = pos_s.x + del_x / 2;
        pos_e.y = clip.y2 - 10;
        scui_custom_draw_line(event, 2, pos_s, pos_e, color_mix);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = pos_s.y + del_y / 2;
        scui_custom_draw_line(event, 2, pos_s, pos_e, color_mix);
        
        pos_s.x = clip.x2 - 10;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x1 + 5 + del_x / 2;
        pos_e.y = clip.y2 - 10;
        scui_custom_draw_line(event, 2, pos_s, pos_e, color_mix);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y2 - 10;
        pos_e.x = clip.x2 - 10;
        pos_e.y = clip.y1 + 5 + del_y / 2;
        scui_custom_draw_line(event, 2, pos_s, pos_e, color_mix);
        
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
        
        clip = scui_widget_draw_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        
        
        scui_widget_event_mask_keep(event);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
