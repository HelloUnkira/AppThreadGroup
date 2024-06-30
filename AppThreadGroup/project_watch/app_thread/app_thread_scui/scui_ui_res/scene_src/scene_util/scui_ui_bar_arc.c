/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_coord_t  bar_pct   = 0;
static scui_coord_t  bar_wait  = 0;
static scui_coord_t  bar_alpha = scui_alpha_cover;

void scui_ui_bar_arc_pct(scui_handle_t pct)
{
    bar_pct = pct;
}

void scui_ui_bar_arc_reset(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    
    bar_wait   = 0;
    bar_alpha  = scui_alpha_cover;
    scui_widget_draw(handle, NULL, false);
    scui_widget_alpha_set(handle, bar_alpha, false);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_bar_arc_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (bar_wait  < SCUI_UI_SCROLL_BAR_STOP_TIME)
            bar_wait += SCUI_ANIMA_TICK;
        else
        if (bar_alpha > 0) {
            scui_alpha_t tick = scui_alpha_cover / (SCUI_UI_SCROLL_BAR_FADE_TIME / SCUI_ANIMA_TICK);
            if (bar_alpha  > tick)
                bar_alpha -= tick;
            else
                bar_alpha  = 0;
            
            scui_widget_alpha_set(event->object, bar_alpha, false);
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t   clip = {0};
        scui_color_t  color_black = {0};
        scui_color_t  color_white = {.filter = true,.color.full = 0xFFFFFFFF,};
        scui_handle_t image_bg   = scui_image_prj_image_src_repeat_slider_04_bgpng;     // 44 * 236
        scui_handle_t image_ring = scui_image_prj_image_src_repeat_slider_03_ringbmp;
        scui_handle_t image_edge = scui_image_prj_image_src_repeat_slider_02_dotbmp;
        
        clip.x = (SCUI_DRV_HOR_RES - 44);
        clip.y = (SCUI_DRV_VER_RES - 236) / 2;
        clip.w =  44;
        clip.h = 236;
        scui_widget_draw_image(event->object, &clip, image_bg, NULL, color_black);
        
        clip.x = (466 - 462) / 2;
        clip.y = (466 - 462) / 2;
        clip.w = 462;
        clip.h = 462;
        
        scui_coord_t angle_d = 29;
        scui_coord_t angle_s = -29 + bar_pct * (angle_d) / 100;
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
