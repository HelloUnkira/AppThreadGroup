/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static inline void scui_ui_scene_window_float_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = SCUI_UI_SCENE_TEST;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    scui_window_float_cfg_set(&float_cfg);
}

static inline void scui_ui_scene_window_list_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(SCUI_UI_SCENE_TEST, &window_cfg);
    window_cfg.sibling[0] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[1] = SCUI_UI_SCENE_1;
    window_cfg.sibling[2] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[3] = SCUI_HANDLE_INVALID;
    scui_window_cfg_set(SCUI_UI_SCENE_TEST, &window_cfg);
}

static void scui_scene_test_show(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    if (event->type == scui_event_show) {
        
        /* 设置背景 */
        scui_widget_image_set(SCUI_UI_SCENE_TEST, scui_image_prj_image_src_home_watch_D10450001_bg_01_bgbmp);
        
        #if 1 /* test string widget */
        scui_string_maker_t string_maker = {0};
        scui_handle_t string_handle = SCUI_HANDLE_INVALID;
        string_maker.widget.type    = scui_widget_type_string;
        string_maker.widget.parent  = SCUI_UI_SCENE_TEST;
        string_maker.name           = SCUI_FONT_TYPE_32_ZH,
        string_maker.args.margin   = SCUI_STRING_MARGIN;
        string_maker.args.gap_none = SCUI_STRING_SPACE_WIDTH;
        string_maker.args.color.color_s.full = 0xFFFFFFFF;
        string_maker.args.color.color_e.full = 0xFFFFFFFF;
        
        string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
        string_maker.widget.clip.y = 25;
        string_maker.widget.clip.h = 40;
        string_maker.text          = SCUI_MULTI_LANG_0X0000;
        scui_string_create(&string_maker, &string_handle, false);
        string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
        string_maker.widget.clip.y = 75;
        string_maker.widget.clip.h = 40;
        string_maker.text          = SCUI_MULTI_LANG_0X0025;
        scui_string_create(&string_maker, &string_handle, false);
        string_maker.widget.clip.x = SCUI_DRV_HOR_RES / 4;
        string_maker.widget.clip.w = SCUI_DRV_HOR_RES / 2;
        string_maker.widget.clip.y = 125;
        string_maker.widget.clip.h = SCUI_DRV_VER_RES / 2;
        string_maker.text          = SCUI_MULTI_LANG_0X0025;
        string_maker.args.line_multi = true;
        scui_string_create(&string_maker, &string_handle, false);
        #endif
    }
    
}

static void scui_scene_test_hide(scui_event_t *event)
{
    if (!scui_widget_event_check_finish(event))
         return;
    
    if (event->type == scui_event_hide) {
        
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_test_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_scene_test_show(event);
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_scene_test_hide(event);
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_get:
        scui_ui_scene_window_float_cfg(event);
        scui_ui_scene_window_list_cfg(event);
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_key_click:
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_widget_scroll_s:
        if (!scui_widget_event_check_execute(event))
             break;
        SCUI_LOG_INFO("scui_event_widget_scroll_s");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_widget_scroll_c:
        if (!scui_widget_event_check_execute(event))
             break;
        SCUI_LOG_INFO("scui_event_widget_scroll_c");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_widget_scroll_e:
        if (!scui_widget_event_check_execute(event))
             break;
        SCUI_LOG_INFO("scui_event_widget_scroll_e");
        scui_widget_event_mask_keep(event);
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
