/*实现目标:
 *    自定义事件响应
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"
#include "app_scui_lib.h"

/*@brief 事件响应(custom)
 *@param event 事件包
 */
void scui_event_custom(scui_event_t *event)
{
    // 当前在待机场景时
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    scui_window_stack_top(&handle_top);
    
    
    
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_ui_none_goto: {
        scui_widget_event_mask_over(event);
        
        if (handle_top != SCUI_UI_SCENE_NONE) {
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_none;
            scui_window_stack_reset(SCUI_UI_SCENE_NONE, false);
            *cfg_type = type;
        }
        break;
    }
    case scui_event_ui_home_goto: {
        scui_widget_event_mask_over(event);
        
        if (handle_top != SCUI_UI_SCENE_HOME) {
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_none;
            scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
            *cfg_type = type;
        }
        
        // 如果本来是在待机界面时
        if (handle_top == SCUI_UI_SCENE_STANDBY) {
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_none;
            scui_window_stack_add(SCUI_UI_SCENE_STANDBY, false);
            *cfg_type = type;
        }
        break;
    }
    case scui_event_ui_stady_enter: {
        scui_widget_event_mask_over(event);
        
        // 如果本来是在待机界面时
        if (handle_top != SCUI_UI_SCENE_STANDBY) {
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_none;
            scui_window_stack_add(SCUI_UI_SCENE_STANDBY, false);
            *cfg_type = type;
        }
        break;
    }
    case scui_event_ui_stady_exit: {
        scui_widget_event_mask_over(event);
        
        if (handle_top == SCUI_UI_SCENE_STANDBY) {
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_none;
            scui_window_stack_del();
            *cfg_type = type;
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_prepare(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_widget_scroll_s:
    case scui_event_widget_scroll_e:
    case scui_event_widget_scroll_c:
        scui_presenter.vibrate_shot();
        break;
    default:
        break;
    }
    
    // 当我们遇到认为不能休眠的事件时,重置时间
    switch (event->type) {
    case scui_event_ptr_hold:
    case scui_event_key_hold:
    case scui_event_enc_clockwise:
    case scui_event_enc_clockwise_anti:
    case scui_event_widget_scroll_s:
    case scui_event_widget_scroll_e:
    case scui_event_widget_scroll_c:
        app_scui_check_time_reset(0, 0);
        break;
    default:
        break;
    }
    
    // 此处退出休眠
    switch (event->type) {
    case scui_event_ptr_hold:
    case scui_event_key_hold:
        
        if (scui_window_active_curr() == SCUI_UI_SCENE_STANDBY) {
            
            scui_event_t event_ui = {
                .object = SCUI_HANDLE_SYSTEM,
                .type   = scui_event_ui_stady_exit,
                .absorb = scui_event_absorb_none,
            };
            scui_event_notify(&event_ui);
            app_module_system_dlps_set(false);
        } else {
            
        }
        break;
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_finish(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_key_click: {
        
        scui_handle_t stack_top = {0};
        scui_window_stack_nest(&stack_top);
        if (stack_top > 1) {
            // 返回上一层
            scui_window_stack_del();
        } else {
            // 回到主界面
            scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
        }
        
        scui_widget_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
