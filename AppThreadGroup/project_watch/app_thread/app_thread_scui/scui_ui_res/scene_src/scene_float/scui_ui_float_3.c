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
    static scui_coord_t pct = 0;
    static scui_coord_t way = +1;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        static uint32_t cnt = 0;
        cnt++;
        
        if (cnt % 10 == 0) {
            pct += way;
            
            if (pct == 100 || pct == 0)
                way = -way;
            
            scui_widget_draw(event->object, NULL, false);
        }
        
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        
        if (!scui_widget_event_check_execute(event))
             return;
        
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
        
        #if 0   // 单象限(测试)
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 30,       color, 45,       100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 30 +  90, color, 45 +  90, 100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 30 + 180, color, 45 + 180, 100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 30 + 270, color, 45 + 270, 100, image_edge);
        #endif
        
        #if 0   // 二象限(测试)
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL,  60, color, 120 + 10, 100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 150, color, 210 + 10, 100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 240, color, 300 + 10, 100, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, -30, color,  30 + 10, 100, image_edge);
        #endif
        
        #if 0   // 三象限(测试)
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL,  60, color, 120 + 10 + 90, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 150, color, 210 + 10 + 90, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 240, color, 300 + 10 + 90, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, -30, color,  30 + 10 + 90, 100, image_edge);
        #endif
        
        #if 0   // 四象限(测试)
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL,  60, color, 120 + 10 + 180, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 150, color, 210 + 10 + 180, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, 240, color, 300 + 10 + 180, 100, image_edge);
        // scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, -30, color,  30 + 10 + 180, 100, image_edge);
        #endif
        
        #if 1
        scui_color_t color_r = {.filter = true,.color.ch.r = 0xFF,};
        scui_color_t color_g = {.filter = true,.color.ch.g = 0xFF,};
        scui_color_t color_b = {.filter = true,.color.ch.b = 0xFF,};
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL,  -81, color_r,  +22, pct, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL,  +39, color_g, +141, pct, image_edge);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, +158, color_b, +262, pct, image_edge);
        
        scui_area_t   btn_clip = {0};
        scui_color_t  btn_color_full = {0};
        scui_handle_t btn_image_full[4] = {0};
        
        btn_clip.w = 72 * 2;
        btn_clip.h = 72;
        btn_clip.x = SCUI_DRV_HOR_RES / 2 - btn_clip.w / 2;
        btn_clip.y = SCUI_DRV_VER_RES / 4 - btn_clip.h / 2;
        btn_image_full[0] = scui_image_prj_image_src_repeat_card_04_r36_1bmp;
        btn_image_full[1] = scui_image_prj_image_src_repeat_card_05_r36_2bmp;
        btn_image_full[2] = scui_image_prj_image_src_repeat_card_06_r36_3bmp;
        btn_image_full[3] = scui_image_prj_image_src_repeat_card_07_r36_4bmp;
        btn_color_full.color.full = 0xFF282828;
        scui_custom_draw_area_image4(event, &btn_clip, btn_image_full, btn_color_full, -1);
        
        btn_clip.w = 72 * 2 + 20;
        btn_clip.x = SCUI_DRV_HOR_RES / 2 - btn_clip.w / 2;
        btn_clip.y = btn_clip.y + btn_clip.h + 15;
        btn_clip.h = 72 + 20;
        btn_image_full[0] = scui_image_prj_image_src_repeat_box_sleep_breathe_02_left_upbmp;
        btn_image_full[1] = scui_image_prj_image_src_repeat_box_sleep_breathe_04_right_upbmp;
        btn_image_full[2] = scui_image_prj_image_src_repeat_box_sleep_breathe_01_left_downbmp;
        btn_image_full[3] = scui_image_prj_image_src_repeat_box_sleep_breathe_03_right_downbmp;
        btn_color_full.color.full = 0xFF00F6EB;
        scui_custom_draw_area_image4(event, &btn_clip, btn_image_full, btn_color_full, 4);
        
        btn_clip.w = 72 * 2;
        btn_clip.x = SCUI_DRV_HOR_RES / 2 - btn_clip.w / 2;
        btn_clip.y = btn_clip.y + 5;
        btn_clip.h = 72 + 10;
        btn_image_full[0] = scui_image_prj_image_src_repeat_card_04_r36_1bmp;
        btn_image_full[1] = scui_image_prj_image_src_repeat_card_05_r36_2bmp;
        btn_image_full[2] = scui_image_prj_image_src_repeat_card_06_r36_3bmp;
        btn_image_full[3] = scui_image_prj_image_src_repeat_card_07_r36_4bmp;
        btn_color_full.color.full = 0xFF282828;
        scui_custom_draw_area_image4(event, &btn_clip, btn_image_full, btn_color_full, 0);
        #endif
        
        
        scui_widget_event_mask_keep(event);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
