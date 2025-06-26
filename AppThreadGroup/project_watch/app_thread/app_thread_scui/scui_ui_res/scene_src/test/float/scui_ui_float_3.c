/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_3_button_event(scui_event_t *event)
{
    // 转移至控件调度
    if (event->type < scui_event_widget_s ||
        event->type > scui_event_widget_e) {
        scui_widget_event_shift(event);
        return;
    }
    
    SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_3_event_proc(scui_event_t *event)
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
void scui_ui_scene_float_3_ring_event_proc(scui_event_t *event)
{
    static scui_coord_t pct = 0;
    static scui_coord_t way = +1;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        pct += way;
        if (pct == 100 || pct == 0)
            way = -way;
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_handle_t image_bg   = scui_image_prj_image_src_03_activity_ring_big_max_01_bjbmp;
        scui_handle_t image_edge = scui_image_prj_image_src_03_activity_ring_big_max_02_bj_00bmp;
        scui_handle_t image_ring = scui_image_prj_image_src_03_activity_ring_big_max_03_bj_01bmp;
        
        /* 绘制圆环背景图: */
        // scui_widget_draw_color(event->object, NULL, (scui_color_t){.color.full = 0xFFFFFFFF,});
        scui_widget_draw_image(event->object, NULL, image_bg, NULL, SCUI_COLOR_FILTER_TRANS);
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += (466 - 458) / 2;
        clip.y += (466 - 458) / 2;
        clip.w = 458;
        clip.h = 458;
        
        //scui_widget_alpha_set(event->object, scui_alpha_pct50, false);
        scui_color_t color = {.filter = true,.color.full = 0xFFFFFFFF,};
        // scui_widget_draw_image(event->object, &clip, image_ring, NULL, color);
        
        #if 0   // 单象限(测试)
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 30,       color, 45,       100, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 30 +  90, color, 45 +  90, 100, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 30 + 180, color, 45 + 180, 100, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 30 + 270, color, 45 + 270, 100, image_edge);
        #endif
        
        #if 0   // 二象限(测试)
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  60, color, 120 + 10, 100, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 150, color, 210 + 10, 100, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 240, color, 300 + 10, 100, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, -30, color,  30 + 10, 100, image_edge);
        #endif
        
        #if 0   // 三象限(测试)
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  60, color, 120 + 10 + 90, 100, image_edge);
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 150, color, 210 + 10 + 90, 100, image_edge);
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 240, color, 300 + 10 + 90, 100, image_edge);
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL, -30, color,  30 + 10 + 90, 100, image_edge);
        #endif
        
        #if 0   // 四象限(测试)
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  60, color, 120 + 10 + 180, 100, image_edge);
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 150, color, 210 + 10 + 180, 100, image_edge);
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL, 240, color, 300 + 10 + 180, 100, image_edge);
        // scui_widget_draw_ring(event->object, &clip, image_ring, NULL, -30, color,  30 + 10 + 180, 100, image_edge);
        #endif
        
        #if 1
        scui_color_t color_r = {.filter = true,.color_s.ch.r = 0xFF,.color_e.ch.r = 0xFF,};
        scui_color_t color_g = {.filter = true,.color_s.ch.g = 0xFF,.color_e.ch.g = 0xFF,};
        scui_color_t color_b = {.filter = true,.color_s.ch.b = 0xFF,.color_e.ch.b = 0xFF,};
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  -81, color_r,  +22, pct, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL,  +39, color_g, +141, pct, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, +158, color_b, +262, pct, image_edge);
        #endif
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_3_btn_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        
        scui_button_maker_t button_maker = {0};
        scui_handle_t button_bandle     = SCUI_HANDLE_INVALID;
        button_maker.widget.type        = scui_widget_type_button;
        button_maker.widget.parent      = event->object;
        button_maker.widget.event_cb    = scui_ui_scene_float_3_button_event;
        button_maker.color.color.full   = 0xFF282828;
        button_maker.mode               = scui_button_mode_static;
        
        button_maker.widget.clip.w = 72 * 2;
        button_maker.widget.clip.h = 72;
        button_maker.widget.clip.x = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y = SCUI_VER_RES / 4 - button_maker.widget.clip.h / 2;
        button_maker.image[0] = scui_image_prj_image_src_repeat_card_04_r36_1bmp;
        button_maker.image[1] = scui_image_prj_image_src_repeat_card_05_r36_2bmp;
        button_maker.image[2] = scui_image_prj_image_src_repeat_card_06_r36_3bmp;
        button_maker.image[3] = scui_image_prj_image_src_repeat_card_07_r36_4bmp;
        button_maker.delta = -1;
        scui_widget_create(&button_maker, &button_bandle);
        
        button_maker.widget.clip.w  = 72 * 2 + 20;
        button_maker.widget.clip.x  = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y += button_maker.widget.clip.h + 15;
        button_maker.widget.clip.h  = 72 + 20;
        button_maker.image[0] = scui_image_prj_image_src_repeat_box_sleep_breathe_02_left_upbmp;
        button_maker.image[1] = scui_image_prj_image_src_repeat_box_sleep_breathe_04_right_upbmp;
        button_maker.image[2] = scui_image_prj_image_src_repeat_box_sleep_breathe_01_left_downbmp;
        button_maker.image[3] = scui_image_prj_image_src_repeat_box_sleep_breathe_03_right_downbmp;
        button_maker.delta = 4;
        scui_widget_create(&button_maker, &button_bandle);
        
        button_maker.widget.clip.w  = 72 * 2;
        button_maker.widget.clip.x  = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y += 5;
        button_maker.widget.clip.h  = 72 + 10;
        button_maker.image[0] = scui_image_prj_image_src_repeat_card_04_r36_1bmp;
        button_maker.image[1] = scui_image_prj_image_src_repeat_card_05_r36_2bmp;
        button_maker.image[2] = scui_image_prj_image_src_repeat_card_06_r36_3bmp;
        button_maker.image[3] = scui_image_prj_image_src_repeat_card_07_r36_4bmp;
        button_maker.delta = 0;
        scui_widget_create(&button_maker, &button_bandle);
        
        button_maker.widget.clip.w  = 72 * 2;
        button_maker.widget.clip.x  = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y += button_maker.widget.clip.h + 15;
        button_maker.widget.clip.h  = 72 + 15;
        button_maker.image[0] = scui_image_prj_image_src_repeat_card_04_r36_1bmp;
        button_maker.image[1] = scui_image_prj_image_src_repeat_card_05_r36_2bmp;
        button_maker.image[2] = scui_image_prj_image_src_repeat_card_06_r36_3bmp;
        button_maker.image[3] = scui_image_prj_image_src_repeat_card_07_r36_4bmp;
        button_maker.mode  = scui_button_mode_scale;
        button_maker.delta = -1;
        scui_widget_create(&button_maker, &button_bandle);
        
        break;
    }
    case scui_event_destroy:
        break;
    default:
        break;
    }
}
