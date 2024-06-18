/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_1_event_proc(scui_event_t *event)
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
void scui_ui_scene_float_1_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        
        clip = scui_widget_draw_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_color_t color_black = {0};
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        char *url = "https://github.com/HelloUnkira/AppThreadGroup.git";
        scui_color_t color = {
            .color_l.full = 0xFF00FF00,
            .color_d.full = 0xFF0000FF,
        };
        clip = scui_widget_draw_clip(event->object);
        clip.x += clip.w / 6;
        clip.y += clip.h / 6;
        clip.w -= clip.w / 6 * 2;
        clip.h -= clip.h / 6 * 2;
        scui_custom_draw_qrcode(event, &clip, color, false, url, strlen(url));
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
void scui_ui_scene_float_1_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        
        clip = scui_widget_draw_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_color_t color_black = {0};
        scui_custom_draw_area(event, &clip, color_black, false, false);
        
        char *url = "https://github.com/HelloUnkira";
        scui_color_t color_xxx = {
            .color_l.full = 0xFF000000,
            .color_d.full = 0xFFFFFFFF,
        };
        clip = scui_widget_draw_clip(event->object);
        clip.x += 10;
        clip.y += 10 + 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2 + 10 * 2;
        scui_custom_draw_barcode(event, &clip, color_xxx, false, url, strlen(url));
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
