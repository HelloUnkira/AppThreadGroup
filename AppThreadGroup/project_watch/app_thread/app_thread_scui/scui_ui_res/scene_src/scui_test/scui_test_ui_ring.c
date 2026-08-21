/*实现目标:
 *    窗口:scui_test_ui_ring
 *    用途: 测试圆环绘制(从 scui_ui_float_3 挖取)
 *    构件: custom 画布全屏
 *    绘制: draw_ring 三色渐变圆环动画
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 临时迁移的 spinner 测试(宏控制, 默认关闭)
 *@param event 事件
 */
void scui_test_ui_ring_spinner_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        scui_custom_data_t *data = NULL;
        scui_custom_data_inst(event->object, &data);
        
        static scui_coord_t spinner_cnt  = 0;
        static scui_coord_t spinner_tick = 1500;
        spinner_cnt += event->tick;
        if (spinner_cnt >  spinner_tick)
            spinner_cnt -= spinner_tick;
        
        data->spinner.percent = scui_map(spinner_cnt, 0, spinner_tick, 0, 100);
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_ring_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        #if 0
        /* 临时迁移自 ui_2 的 spinner 测试 */
        scui_custom_maker_define(custom_maker);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        
        custom_maker.widget.parent = event->object;
        custom_maker.type = scui_custom_type_spinner;
        custom_maker.data.spinner.spinner = scui_image_prj_400X400pxjpg;
        custom_maker.data.spinner.edge = scui_image_prj_400X400px_dotbmp;
        custom_maker.data.spinner.color.filter = true;
        custom_maker.data.spinner.angle_s = 270;
        custom_maker.data.spinner.angle_l = 60;
        custom_maker.data.spinner.way = 1;
        custom_maker.widget.clip.x = (SCUI_HOR_RES - scui_image_w(custom_maker.data.spinner.spinner)) / 2;
        custom_maker.widget.clip.y = (SCUI_VER_RES - scui_image_h(custom_maker.data.spinner.spinner)) / 2;
        custom_maker.widget.clip.w = scui_image_w(custom_maker.data.spinner.spinner);
        custom_maker.widget.clip.h = scui_image_h(custom_maker.data.spinner.spinner);
        custom_maker.widget.event_cb = scui_test_ui_ring_spinner_event_proc;
        scui_widget_create(&custom_maker, &custom_handle);
        scui_widget_child_move_background(custom_handle);
        #endif
        break;
    }
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
