/*实现目标:
 *    窗口:scui_test_ui_ring
 *    用途: 测试圆环绘制(从 scui_ui_float_3 挖取)
 *    构件: custom 画布全屏
 *    绘制: draw_ring 三色渐变圆环动画
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_ring_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}

/*@brief 画布控件事件响应回调(draw_ring)
 *@param event 事件
 */
void scui_test_ui_ring_canvas_event_proc(scui_event_t *event)
{
    static scui_coord_t pct = 0;
    static scui_coord_t way = +1;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        pct += way;
        if (pct == 100 || pct == 0)
            way = -way;
        
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    case scui_event_draw_graph: {
        
        scui_handle_t image_bg   = scui_image_prj_act_ring_big_max_01_bjjpg;
        scui_handle_t image_edge = scui_image_prj_act_ring_big_max_02_bj_00bmp;
        scui_handle_t image_ring = scui_image_prj_act_ring_big_max_03_bj_01bmp;
        
        /* 绘制圆环背景图: */
        scui_widget_draw_image(event->object, NULL, image_bg, NULL, SCUI_COLOR_FILTER_TRANS);
        
        scui_area_t clip = {.x = 4, .y = 4, .w = 458, .h = 458};
        
        scui_color_t color = {.filter = true,.color.full = 0xFFFFFFFF,};
        
        #if 1
        scui_color_t color_r = {.filter = true,.color_s.ch.r = 0xFF,.color_e.ch.r = 0xFF,};
        scui_color_t color_g = {.filter = true,.color_s.ch.g = 0xFF,.color_e.ch.g = 0xFF,};
        scui_color_t color_b = {.filter = true,.color_s.ch.b = 0xFF,.color_e.ch.b = 0xFF,};
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, -81, color_r, +22, pct, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, +39, color_g, +141, pct, image_edge);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, +158, color_b, +262, pct, image_edge);
        #endif
        
        break;
    }
    default:
        break;
    }
}
