/*实现目标:
 *    窗口:scui_test_ui_button
 *    用途: 测试按钮(从 scui_ui_float_3/2 挖取)
 *    构件: image button x4 + pixel button x1
 *    交互: 点击打印
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_button_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_button_click:
        SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_button_window_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        #if 1
        /* image button x4 (从 float_3 挖取, 重新纵向居中排布) */
        scui_button_maker_t button_maker = {0};
        scui_handle_t button_bandle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&button_maker, scui_widget_type_button);
        button_maker.widget.parent      = event->object;
        button_maker.widget.event_cb    = scui_test_ui_button_event_proc;
        button_maker.type               = scui_button_type_image;
        button_maker.mode               = scui_button_mode_static;
        button_maker.image.color        = SCUI_COLOR_MAKE32(false, 0x0, 0xFF282828);
        
        /* 整体垂直居中: 4个image按钮(72) + 1个pixel按钮(100) */
        scui_coord_t btn_w = 72 * 2;
        scui_coord_t btn_h = 72;
        scui_coord_t btn_gap = 15;
        scui_coord_t total_h = btn_h * 4 + 100 + btn_gap * 4;
        scui_coord_t cur_y = (SCUI_VER_RES - total_h) / 2;
        
        button_maker.widget.clip.w = btn_w;
        button_maker.widget.clip.h = btn_h;
        button_maker.widget.clip.x = SCUI_HOR_RES / 2 - btn_w / 2;
        button_maker.widget.clip.y = cur_y;
        button_maker.image.image[0] = scui_image_prj_repeat_card_04_r36_1jpg;
        button_maker.image.image[1] = scui_image_prj_repeat_card_05_r36_2jpg;
        button_maker.image.image[2] = scui_image_prj_repeat_card_06_r36_3jpg;
        button_maker.image.image[3] = scui_image_prj_repeat_card_07_r36_4jpg;
        button_maker.image.delta = -1;
        scui_widget_create(&button_maker, &button_bandle);
        
        cur_y += btn_h + btn_gap;
        button_maker.widget.clip.w  = btn_w + 20;
        button_maker.widget.clip.x  = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y  = cur_y;
        button_maker.widget.clip.h  = btn_h + 20;
        button_maker.image.image[0] = scui_image_prj_repeat_box_sleep_breathe_02_left_upjpg;
        button_maker.image.image[1] = scui_image_prj_repeat_box_sleep_breathe_04_right_upjpg;
        button_maker.image.image[2] = scui_image_prj_repeat_box_sleep_breathe_01_left_downjpg;
        button_maker.image.image[3] = scui_image_prj_repeat_box_sleep_breathe_03_right_downjpg;
        button_maker.image.delta = 4;
        scui_widget_create(&button_maker, &button_bandle);
        
        cur_y += button_maker.widget.clip.h + btn_gap;
        button_maker.widget.clip.w  = btn_w;
        button_maker.widget.clip.x  = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y  = cur_y;
        button_maker.widget.clip.h  = btn_h + 10;
        button_maker.image.image[0] = scui_image_prj_repeat_card_04_r36_1jpg;
        button_maker.image.image[1] = scui_image_prj_repeat_card_05_r36_2jpg;
        button_maker.image.image[2] = scui_image_prj_repeat_card_06_r36_3jpg;
        button_maker.image.image[3] = scui_image_prj_repeat_card_07_r36_4jpg;
        button_maker.image.delta = 0;
        scui_widget_create(&button_maker, &button_bandle);
        
        cur_y += button_maker.widget.clip.h + btn_gap;
        button_maker.widget.clip.w  = btn_w;
        button_maker.widget.clip.x  = SCUI_HOR_RES / 2 - button_maker.widget.clip.w / 2;
        button_maker.widget.clip.y  = cur_y;
        button_maker.widget.clip.h  = btn_h + 15;
        button_maker.mode = scui_button_mode_scale;
        button_maker.image.image[0] = scui_image_prj_repeat_card_04_r36_1jpg;
        button_maker.image.image[1] = scui_image_prj_repeat_card_05_r36_2jpg;
        button_maker.image.image[2] = scui_image_prj_repeat_card_06_r36_3jpg;
        button_maker.image.image[3] = scui_image_prj_repeat_card_07_r36_4jpg;
        button_maker.image.delta = -1;
        scui_widget_create(&button_maker, &button_bandle);
        #endif
        
        #if 1
        /* pixel button x1 (从 float_2 挖取, 重新排布) */
        cur_y += button_maker.widget.clip.h + btn_gap;
        scui_button_maker_t pixel_maker = {0};
        scui_handle_t pixel_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&pixel_maker, scui_widget_type_button);
        pixel_maker.widget.style.fully_bg = true;
        pixel_maker.widget.parent = event->object;
        pixel_maker.widget.event_cb = scui_test_ui_button_event_proc;
        
        pixel_maker.widget.clip.w  = btn_w;
        pixel_maker.widget.clip.h  = 100;
        pixel_maker.widget.clip.x  = SCUI_HOR_RES / 2 - btn_w / 2;
        pixel_maker.widget.clip.y  = cur_y;
        pixel_maker.type = scui_button_type_pixel;
        pixel_maker.mode = scui_button_mode_scale;
        pixel_maker.pixel.color[0].color_s.full = 0xFF00FF00;
        pixel_maker.pixel.color[0].color_e.full = 0xFF008000;
        pixel_maker.pixel.color[1].color_s.full = 0xFF000080;
        pixel_maker.pixel.color[1].color_e.full = 0xFF000080;
        pixel_maker.pixel.color[2].color_s.full = 0xFF800000;
        pixel_maker.pixel.color[2].color_e.full = 0xFF800000;
        pixel_maker.pixel.color[3].color_s.full = 0xFF87CEFA;
        pixel_maker.pixel.color[3].color_e.full = 0xFF87CEFA;
        pixel_maker.pixel.alpha[0] = scui_alpha_cover;
        pixel_maker.pixel.alpha[1] = scui_alpha_cover;
        pixel_maker.pixel.alpha[2] = scui_alpha_cover;
        pixel_maker.pixel.alpha[3] = scui_alpha_cover;
        pixel_maker.pixel.width[1] = 3;
        pixel_maker.pixel.width[2] = 3;
        pixel_maker.pixel.width[3] = 13;
        pixel_maker.pixel.radius = 20;
        scui_widget_create(&pixel_maker, &pixel_handle);
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    default:
        break;
    }
}
