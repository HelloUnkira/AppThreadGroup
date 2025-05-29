/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

void scui_ui_bar_arc_reset(scui_ui_bar_arc_t *bar_arc)
{
    bar_arc->bar_wait   = 0;
    bar_arc->bar_alpha  = scui_alpha_cover;
    
    // 不要使用默认重绘,只重绘背景这一部分即可
    #if 0
    scui_area_t draw_clip = {
        .x = (SCUI_HOR_RES - 44),
        .y = (SCUI_VER_RES - 236) / 2,
        .w =  44,
        .h = 236,
    };
    scui_widget_draw(bar_arc->bar_handle, &draw_clip, false);
    #else
    scui_widget_draw(bar_arc->bar_handle, NULL, false);
    #endif
    
    scui_widget_alpha_set(bar_arc->bar_handle, bar_arc->bar_alpha, false);
}

void scui_ui_bar_arc_event_proc(scui_ui_bar_arc_t *bar_arc, scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        if (bar_arc->bar_wait  < SCUI_UI_SCROLL_BAR_STOP_TIME)
            bar_arc->bar_wait += SCUI_ANIMA_TICK;
        else
        if (bar_arc->bar_alpha > 0) {
            scui_alpha_t tick = scui_alpha_cover / (SCUI_UI_SCROLL_BAR_FADE_TIME / SCUI_ANIMA_TICK);
            if (bar_arc->bar_alpha  > tick)
                bar_arc->bar_alpha -= tick;
            else
                bar_arc->bar_alpha  = 0;
            
            scui_widget_alpha_set(event->object, bar_arc->bar_alpha, false);
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t   clip = {0};
        scui_color_t  color_black = {0};
        scui_color_t  color_white = {.filter = true,.color_s.full = 0xFFFFFFFF,.color_e.full = 0xFFFFFFFF,};
        scui_handle_t image_bg   = scui_image_prj_image_src_repeat_slider_04_bgpng;     // 44 * 236
        scui_handle_t image_ring = scui_image_prj_image_src_repeat_slider_03_ringbmp;
        scui_handle_t image_edge = scui_image_prj_image_src_repeat_slider_02_dotbmp;
        
        clip.x = (SCUI_HOR_RES - 44);
        clip.y = (SCUI_VER_RES - 236) / 2;
        clip.w =  44;
        clip.h = 236;
        scui_widget_draw_image(event->object, &clip, image_bg, NULL, color_black);
        
        clip.x = (466 - 462) / 2;
        clip.y = (466 - 462) / 2;
        clip.w = 462;
        clip.h = 462;
        
        scui_coord_t angle_d = 29;
        scui_coord_t angle_s = -29 + bar_arc->bar_pct * (angle_d) / 100;
        scui_coord_t angle_e = angle_s + angle_d;
        SCUI_LOG_INFO("angle:<s:%d,e:%d>", angle_s, angle_e);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, angle_s, color_white, angle_e, 100, image_edge);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
