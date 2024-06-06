/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_3_event_proc(scui_event_t *event)
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
void scui_ui_scene_float_3_ring_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        
        scui_handle_t image_bg   = scui_image_prj_image_src_03_activity_ring_big_max_01_bjbmp;
        scui_handle_t image_edge = scui_image_prj_image_src_03_activity_ring_big_max_02_bj_00bmp;
        scui_handle_t image_ring = scui_image_prj_image_src_03_activity_ring_big_max_03_bj_01bmp;
        
        /* 绘制圆环背景图: */
        // scui_widget_surface_draw_color(event->object, NULL, (scui_color_t){.color.full = 0xFFFFFFFF,});
        scui_widget_surface_draw_image(event->object, NULL, image_bg, NULL, (scui_color_t){0});
        
        scui_area_t clip = scui_widget_surface_clip(event->object);
        clip.x += (466 - 458) / 2;
        clip.y += (466 - 458) / 2;
        clip.w = 458;
        clip.h = 458;
        
        //scui_widget_alpha_set(event->object, scui_alpha_pct50);
        scui_color_t color = {.filter = true,.color.full = 0xFFFFFFFF,};
        // scui_widget_surface_draw_image(event->object, &clip, image_ring, NULL, color);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 0, color, 360, 100, image_edge);
        
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL,  45, color, 135, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 135, color, 225, 100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 225, color, 315, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 315, color,  45, 100, image_edge);
        
        scui_color_t color_r = {.color.ch.r = 0x80,};
        scui_color_t color_g = {.color.ch.g = 0x80,};
        scui_color_t color_b = {.color.ch.b = 0x80,};
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 0, color_r, 360, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 0, color_g, 360, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 0, color_b, 360, 100, image_edge);
        
        scui_widget_event_mask_keep(event);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
