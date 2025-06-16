/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_test_event_proc(scui_event_t *event)
{
    static bool ptr_long_jump = false;
    
    static uint16_t font_size = 32;
    static const uint16_t font_size_gap = 2;
    static const uint16_t font_size_min = 10;
    static const uint16_t font_size_max = 40;
    static scui_handle_t handle_string[4] = {0};
    // 备注: 如果需要使用动态文字缩放, 则应该放弃掉缓冲帧和级联着色, 否则可能会爆内存
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        
        /* 设置背景 */
        scui_widget_image_set(SCUI_UI_SCENE_TEST, scui_image_prj_image_src_watchface_bg_01_bgjpg);
        
        #if 1 /* test string widget */
        scui_string_maker_t string_maker = {0};
        string_maker.widget.type                = scui_widget_type_string;
        // string_maker.widget.style.trans         = true;
        string_maker.widget.parent              = SCUI_UI_SCENE_TEST;
        string_maker.args.line_width            = 2;
        string_maker.args.mode_scroll           = 1;
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.filter          = true;
        string_maker.args.regrad                = true;
        string_maker.draw_cache                 = true;
        
        string_maker.font_idx                   = SCUI_FONT_IDX_MZ;
        string_maker.lang_type                  = SCUI_MULTI_LANG_TYPE_MZ;
        string_maker.args.size                  = font_size;
        
        /* test:全文本渐变 */
        scui_coord_t grad_n = 8;
        scui_color_t grad_s[8] = {
            {.color.full = 0xFFFF5733}, {.color.full = 0xFFFFBD33},
            {.color.full = 0xFF75FF33}, {.color.full = 0xFF33FF57},
            {.color.full = 0xFF33FFBD}, {.color.full = 0xFF3375FF},
            {.color.full = 0xFF5733FF}, {.color.full = 0xFFBD33FF},
        };
        
        scui_coord_t y_offset = 25;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 50;
        string_maker.text             = SCUI_MULTI_LANG_0X0029;
        string_maker.args.line_under  = 1;
        string_maker.args.line_delete = 0;
        string_maker.args.color.color_s.full = 0xFFFFFFFF;
        string_maker.args.color.color_e.full = 0xFFFFFFFF;
        scui_widget_create(&string_maker, &handle_string[0], false);
        scui_string_upgrade_grads(handle_string[0], grad_s, grad_n, false);
        
        y_offset += string_maker.widget.clip.h + 10;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 50;
        string_maker.text             = SCUI_MULTI_LANG_0X002e;
        string_maker.args.line_under  = 0;
        string_maker.args.line_delete = 1;
        string_maker.args.color.color_s.full = 0xFFFF00FF;
        string_maker.args.color.color_e.full = 0xFF00FF00;
        scui_widget_create(&string_maker, &handle_string[1], false);
        scui_string_upgrade_grads(handle_string[1], grad_s, grad_n, false);
        
        y_offset += string_maker.widget.clip.h + 10;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 130;
        string_maker.text             = SCUI_MULTI_LANG_0X0034;
        string_maker.args.line_under  = 1;
        string_maker.args.line_delete = 0;
        string_maker.args.color.color_s.full = 0xFFFF0000;
        string_maker.args.color.color_e.full = 0xFF0000FF;
        string_maker.args.line_multi = true;
        scui_widget_create(&string_maker, &handle_string[2], false);
        scui_string_upgrade_grads(handle_string[2], grad_s, grad_n, true);
        
        y_offset += string_maker.widget.clip.h + 10;
        string_maker.widget.clip.x = SCUI_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_HOR_RES / 2;
        string_maker.widget.clip.y = y_offset;
        string_maker.widget.clip.h = 130;
        string_maker.text             = SCUI_MULTI_LANG_0X0042;
        string_maker.args.line_under  = 0;
        string_maker.args.line_delete = 1;
        string_maker.args.color.color_s.full = 0xFF0000FF;
        string_maker.args.color.color_e.full = 0xFFFF0000;
        string_maker.args.line_multi = true;
        scui_widget_create(&string_maker, &handle_string[3], false);
        scui_string_upgrade_grads(handle_string[3], grad_s, grad_n, true);
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    
    case scui_event_ptr_down:
        ptr_long_jump = false;
        break;
    case scui_event_ptr_hold:
        if (event->ptr_tick > 3000) {
            if (!ptr_long_jump) SCUI_LOG_WARN("ptr long hold");
            ptr_long_jump = true;
        }
        break;
    case scui_event_ptr_up:
        if (ptr_long_jump)
            scui_ui_scene_return();
        break;
    
    case scui_event_key_click:
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        static uint8_t multi_lang_table_idx = 0;
        static const scui_handle_t multi_lang_table[] = {
            scui_multi_lang_type_zh,
            scui_multi_lang_type_en,
        };
        multi_lang_table_idx++;
        multi_lang_table_idx %= scui_arr_len(multi_lang_table);
        scui_multi_lang_set(&multi_lang_table[multi_lang_table_idx]);
        
        scui_event_mask_over(event);
        break;
    case scui_event_enc_clockwise:
        if (font_size +  font_size_gap <= font_size_max)
            font_size += font_size_gap;
        for (uint8_t idx = 0; idx < scui_arr_len(handle_string); idx++)
            scui_string_adjust_size(handle_string[idx], font_size);
        scui_event_mask_over(event);
        break;
    case scui_event_enc_clockwise_anti:
        if (font_size -  font_size_gap >= font_size_min)
            font_size -= font_size_gap;
        for (uint8_t idx = 0; idx < scui_arr_len(handle_string); idx++)
            scui_string_adjust_size(handle_string[idx], font_size);
        scui_event_mask_over(event);
        break;
    default:
        break;
    }
}
