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
    default:
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
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_color_t color_black = {0};
        scui_widget_draw_color(event->object, &clip, color_black);
        
        char *url = "https://github.com/HelloUnkira/AppThreadGroup.git";
        scui_color_t color = {
            .color_l.full = 0xFF00FF00,
            .color_d.full = 0xFF0000FF,
        };
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_qrcode(event->object, &clip, color, strlen(url), url);
        
        break;
    }
    default:
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
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = {0};
        
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_color_t color_black = {0};
        scui_widget_draw_color(event->object, &clip, color_black);
        
        char *url = "https://github.com/HelloUnkira";
        scui_color_t color = {
            .color_l.full = 0xFF00FF00,
            .color_d.full = 0xFF0000FF,
        };
        clip = scui_widget_clip(event->object);
        clip.x += 15;
        clip.y += 15;
        clip.w -= 15 * 2;
        clip.h -= 15 * 2;
        scui_widget_draw_barcode(event->object, &clip, color, strlen(url), url);
        break;
    }
    default:
        break;
    }
}
