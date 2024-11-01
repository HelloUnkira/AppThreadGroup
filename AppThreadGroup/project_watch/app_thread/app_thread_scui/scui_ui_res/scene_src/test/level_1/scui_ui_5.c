/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_5_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_key_click:
        break;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_5_ring_event_proc(scui_event_t *event)
{
    static scui_coord_t spinner_pct  = 0;
    static scui_coord_t spinner_cnt  = 0;
    static scui_coord_t spinner_tick = 1500 / SCUI_ANIMA_TICK;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        spinner_cnt++;
        if (spinner_cnt >  spinner_tick)
            spinner_cnt -= spinner_tick;
        
        spinner_pct = scui_map(spinner_cnt, 0, spinner_tick, 0, 100);
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             return;
        
        scui_handle_t image_edge = SCUI_HANDLE_INVALID;
        scui_handle_t image_ring = scui_image_prj_image_src_400X400pxpng;
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += (clip.w - scui_image_w(image_ring)) / 2;
        clip.y += (clip.h - scui_image_h(image_ring)) / 2;
        clip.w = scui_image_w(image_ring);
        clip.h = scui_image_h(image_ring);
        
        // scui_color_t color_black = {0};
        // scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_color_t color = {.filter = true,};
        scui_custom_draw_spinner(event, &clip, image_ring, color, image_edge,
                                 spinner_pct, 270, 60, +1);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
