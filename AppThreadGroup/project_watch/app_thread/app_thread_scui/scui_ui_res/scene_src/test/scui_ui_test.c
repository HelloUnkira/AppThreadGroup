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
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        if (scui_widget_event_check_prepare(event)) {
        
            /* 设置背景 */
            scui_widget_image_set(SCUI_UI_SCENE_TEST, scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp);
            
            #if 1 /* test string widget */
            scui_string_maker_t string_maker = {0};
            scui_handle_t string_handle             = SCUI_HANDLE_INVALID;
            string_maker.widget.type                = scui_widget_type_string;
            string_maker.widget.style.trans         = true;
            string_maker.widget.parent              = SCUI_UI_SCENE_TEST;
            string_maker.args.mode_scroll           = 1;
            string_maker.args.align_hor             = 2;
            string_maker.args.align_ver             = 2;
            string_maker.args.color.filter          = true;
            string_maker.draw_cache                 = true;
            
            scui_coord_t y_offset = 25;
            string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
            string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
            string_maker.widget.clip.y = y_offset;
            string_maker.widget.clip.h = 50;
            string_maker.font_idx      = 0,
            string_maker.text          = SCUI_MULTI_LANG_0X0029;
            string_maker.args.color.color_s.full = 0xFFFFFFFF;
            string_maker.args.color.color_e.full = 0xFFFFFFFF;
            scui_string_create(&string_maker, &string_handle, false);
            
            y_offset += string_maker.widget.clip.h + 10;
            string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
            string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
            string_maker.widget.clip.y = y_offset;
            string_maker.widget.clip.h = 50;
            string_maker.font_idx      = 0,
            string_maker.text          = SCUI_MULTI_LANG_0X002e;
            string_maker.args.color.color_s.full = 0xFFFF00FF;
            string_maker.args.color.color_e.full = 0xFF00FF00;
            scui_string_create(&string_maker, &string_handle, false);
            
            y_offset += string_maker.widget.clip.h + 10;
            string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
            string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
            string_maker.widget.clip.y = y_offset;
            string_maker.widget.clip.h = 130;
            string_maker.font_idx      = 0,
            string_maker.text          = SCUI_MULTI_LANG_0X0034;
            string_maker.args.color.color_s.full = 0xFFFF0000;
            string_maker.args.color.color_e.full = 0xFF0000FF;
            string_maker.args.line_multi = true;
            scui_string_create(&string_maker, &string_handle, false);
            
            y_offset += string_maker.widget.clip.h + 10;
            string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
            string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
            string_maker.widget.clip.y = y_offset;
            string_maker.widget.clip.h = 130;
            string_maker.font_idx      = 0,
            string_maker.text          = SCUI_MULTI_LANG_0X0042;
            string_maker.args.color.color_s.full = 0xFF0000FF;
            string_maker.args.color.color_e.full = 0xFFFF0000;
            string_maker.args.line_multi = true;
            scui_string_create(&string_maker, &string_handle, false);
            #endif
        }
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
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        static uint8_t font_name_table_idx = 0;
        static const scui_handle_t font_name_table[] = {
            SCUI_FONT_TYPE_EN,
            SCUI_FONT_TYPE_ZH,
        };
        font_name_table_idx++;
        font_name_table_idx %= scui_arr_len(font_name_table);
        scui_font_name_set(font_name_table[font_name_table_idx]);
        
        scui_widget_event_mask_over(event);
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
