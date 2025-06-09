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

/*@brief 事件类型转标记字符串
 *@param type 事件类型
 *@retval 标记字符串
 */
const char * scui_event_type_misc_stringify(scui_event_type_t type)
{
    if (type >= scui_event_sys_s &&
        type <= scui_event_sys_e) {
        
        switch (type) {
        
        /* 系统事件: sched */
        case scui_event_sched_delay:                return scui_stringify(scui_event_sched_delay);
        case scui_event_anima_elapse:               return scui_stringify(scui_event_anima_elapse);
        case scui_event_focus_lost:                 return scui_stringify(scui_event_focus_lost);
        case scui_event_focus_get:                  return scui_stringify(scui_event_focus_get);
        case scui_event_hide:                       return scui_stringify(scui_event_hide);
        case scui_event_show:                       return scui_stringify(scui_event_show);
        case scui_event_draw:                       return scui_stringify(scui_event_draw);
        case scui_event_refr:                       return scui_stringify(scui_event_refr);
        case scui_event_layout:                     return scui_stringify(scui_event_layout);
        case scui_event_size_adjust:                return scui_stringify(scui_event_size_adjust);
        case scui_event_change_lang:                return scui_stringify(scui_event_change_lang);
        /* 系统事件: sched */
        case scui_event_widget_scroll_start:        return scui_stringify(scui_event_widget_scroll_start);
        case scui_event_widget_scroll_keep:         return scui_stringify(scui_event_widget_scroll_keep);
        case scui_event_widget_scroll_over:         return scui_stringify(scui_event_widget_scroll_over);
        case scui_event_widget_scroll_layout:       return scui_stringify(scui_event_widget_scroll_layout);
        case scui_event_widget_button_click:        return scui_stringify(scui_event_widget_button_click);
        /* 系统事件: ptr */
        case scui_event_ptr_cover:                  return scui_stringify(scui_event_ptr_cover);
        case scui_event_ptr_down:                   return scui_stringify(scui_event_ptr_down);
        case scui_event_ptr_hold:                   return scui_stringify(scui_event_ptr_hold);
        case scui_event_ptr_click:                  return scui_stringify(scui_event_ptr_click);
        case scui_event_ptr_fling:                  return scui_stringify(scui_event_ptr_fling);
        case scui_event_ptr_move:                   return scui_stringify(scui_event_ptr_move);
        case scui_event_ptr_up:                     return scui_stringify(scui_event_ptr_up);
        /* 系统事件: enc */
        case scui_event_enc_clockwise:              return scui_stringify(scui_event_enc_clockwise);
        case scui_event_enc_clockwise_anti:         return scui_stringify(scui_event_enc_clockwise_anti);
        /* 系统事件: key */
        case scui_event_key_down:                   return scui_stringify(scui_event_key_down);
        case scui_event_key_hold:                   return scui_stringify(scui_event_key_hold);
        case scui_event_key_click:                  return scui_stringify(scui_event_key_click);
        case scui_event_key_up:                     return scui_stringify(scui_event_key_up);
        
        default:
            break;
        }
    }
    
    if (type >= scui_event_custom_s &&
        type <= scui_event_custom_e) {
        
        switch (type) {
        
        case scui_event_ui_none_goto:               return scui_stringify(scui_event_ui_none_goto);
        case scui_event_ui_home_goto:               return scui_stringify(scui_event_ui_home_goto);
        case scui_event_ui_standy_enter:            return scui_stringify(scui_event_ui_standy_enter);
        case scui_event_ui_standy_exit:             return scui_stringify(scui_event_ui_standy_exit);
        
        default: break;
        }
    }
    
    return NULL;
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_access(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_widget_scroll_start:
    case scui_event_widget_scroll_over:
    case scui_event_widget_scroll_keep:
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
    case scui_event_widget_scroll_start:
    case scui_event_widget_scroll_over:
    case scui_event_widget_scroll_keep:
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
                .type   = scui_event_ui_standy_exit,
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

/*@brief 事件响应(custom)
 *@param event 事件包
 */
void scui_event_custom_myself(scui_event_t *event)
{
    // 当前在待机场景时
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    scui_window_stack_top(&handle_top);
    
    
    
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_ui_tick_frame: {
        scui_event_mask_over(event);
        // 如果要是用时序调度, 通过该事件控制
        scui_widget_refr(SCUI_HANDLE_INVALID, false);
        break;
    }
    case scui_event_ui_none_goto: {
        scui_event_mask_over(event);
        
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
        scui_event_mask_over(event);
        if (!scui_event_check_execute(event))
             break;
        
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
    case scui_event_ui_standy_enter: {
        scui_event_mask_over(event);
        if (!scui_event_check_execute(event))
             break;
        
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
    case scui_event_ui_standy_exit: {
        scui_event_mask_over(event);
        if (!scui_event_check_execute(event))
             break;
        
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
void scui_event_custom_finish(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_key_click:
        scui_event_mask_over(event);
        scui_ui_scene_return();
        break;
    default:
        break;
    }
}
