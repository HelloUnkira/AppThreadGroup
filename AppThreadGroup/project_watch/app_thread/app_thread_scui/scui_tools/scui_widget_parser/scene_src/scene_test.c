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
    window_cfg.sibling[1] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[2] = SCUI_UI_SCENE_HOME;
    window_cfg.sibling[3] = SCUI_UI_SCENE_1;
    scui_window_cfg_set(SCUI_UI_SCENE_TEST, &window_cfg);
}

static void scui_scene_test_show(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    if (event->type == scui_event_show) {
        
        #if 0
        #elif 1 /* test scroll widget */
        scui_scroll_maker_t scroll_maker = {0};
        scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
        scroll_maker.widget.type = scui_widget_type_scroll;
        scroll_maker.widget.style.indev_ptr = true;
        scroll_maker.widget.style.indev_enc = true;
        scroll_maker.widget.style.indev_key = true;
        scroll_maker.widget.clip.x = SCUI_DRV_HOR_RES / 6;
        scroll_maker.widget.clip.y = SCUI_DRV_HOR_RES / 6;
        scroll_maker.widget.clip.w = SCUI_DRV_HOR_RES * 4 / 6;
        scroll_maker.widget.clip.h = SCUI_DRV_HOR_RES * 4 / 6;
        scroll_maker.widget.parent = SCUI_UI_SCENE_TEST;
        scroll_maker.widget.child_num = 10;
        scroll_maker.widget.color.color.full = 0xFF4F4F4F;
        scroll_maker.springback = 70;
        scroll_maker.space = 50;
        #if 1   /* just for test */
        scroll_maker.loop = true;
        scroll_maker.fling_page = 5;
        // scroll_maker.dir = scui_event_dir_hor;
        scroll_maker.dir = scui_event_dir_ver;
        // scroll_maker.pos = scui_event_pos_l;
        // scroll_maker.pos = scui_event_pos_r;
        // scroll_maker.pos = scui_event_pos_u;
        // scroll_maker.pos = scui_event_pos_d;
        scroll_maker.pos = scui_event_pos_c;
        #endif
        scui_scroll_create(&scroll_maker, &scroll_handle, false);
        
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.type   = scui_widget_type_custom;
        custom_maker.widget.clip.w = SCUI_DRV_HOR_RES / 4;
        custom_maker.widget.clip.h = SCUI_DRV_HOR_RES / 4;
        custom_maker.widget.parent = scroll_handle;
        
        uint32_t bg_color_list[]= {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFFFFFF};
        
        for (uint8_t idx = 0; idx < scui_arr_len(bg_color_list); idx++) {
            custom_maker.widget.color.color.full = bg_color_list[idx];
            scui_custom_create(&custom_maker, &custom_handle, false);
        }
        
        #else
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
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
