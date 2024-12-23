/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
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
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        SCUI_LOG_WARN("scui_event_key_click");
        static scui_window_switch_type_t switch_type = SCUI_WINDOW_MGR_TYPE_CFG;
        switch_type++;
        
        if (switch_type < scui_window_switch_single_s + 1)
            switch_type = scui_window_switch_single_e - 1;
        if (switch_type > scui_window_switch_single_e - 1)
            switch_type = scui_window_switch_single_s + 1;
        
        SCUI_ASSERT(switch_type > scui_window_switch_single_s);
        SCUI_ASSERT(switch_type < scui_window_switch_single_e);
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        *cfg_type = switch_type;
        
        scui_widget_event_mask_over(event);
        break;
    }
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}