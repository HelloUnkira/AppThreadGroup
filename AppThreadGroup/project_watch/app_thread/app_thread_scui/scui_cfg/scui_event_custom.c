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
        
        static const char * event_sys_str[scui_event_sys_num] = {
            /* 系统事件: sched */
            [scui_event_sched_delay] =              scui_stringify(scui_event_sched_delay),
            [scui_event_anima_elapse] =             scui_stringify(scui_event_anima_elapse),
            [scui_event_focus_lost] =               scui_stringify(scui_event_focus_lost),
            [scui_event_focus_get] =                scui_stringify(scui_event_focus_get),
            [scui_event_destroy] =                  scui_stringify(scui_event_destroy),
            [scui_event_create] =                   scui_stringify(scui_event_create),
            [scui_event_hide] =                     scui_stringify(scui_event_hide),
            [scui_event_show] =                     scui_stringify(scui_event_show),
            [scui_event_draw] =                     scui_stringify(scui_event_draw),
            [scui_event_refr] =                     scui_stringify(scui_event_refr),
            [scui_event_layout] =                   scui_stringify(scui_event_layout),
            [scui_event_child_nums] =               scui_stringify(scui_event_child_nums),
            [scui_event_child_size] =               scui_stringify(scui_event_child_size),
            [scui_event_child_pos] =                scui_stringify(scui_event_child_pos),
            [scui_event_size_auto] =                scui_stringify(scui_event_size_auto),
            [scui_event_size_adjust] =              scui_stringify(scui_event_size_adjust),
            [scui_event_lang_change] =              scui_stringify(scui_event_lang_change),
            /* 系统事件: sched */
            [scui_event_widget_scroll_start] =      scui_stringify(scui_event_widget_scroll_start),
            [scui_event_widget_scroll_keep] =       scui_stringify(scui_event_widget_scroll_keep),
            [scui_event_widget_scroll_over] =       scui_stringify(scui_event_widget_scroll_over),
            [scui_event_widget_scroll_layout] =     scui_stringify(scui_event_widget_scroll_layout),
            [scui_event_widget_button_click] =      scui_stringify(scui_event_widget_button_click),
            /* 系统事件: ptr */
            [scui_event_ptr_cover] =                scui_stringify(scui_event_ptr_cover),
            [scui_event_ptr_down] =                 scui_stringify(scui_event_ptr_down),
            [scui_event_ptr_hold] =                 scui_stringify(scui_event_ptr_hold),
            [scui_event_ptr_click] =                scui_stringify(scui_event_ptr_click),
            [scui_event_ptr_fling] =                scui_stringify(scui_event_ptr_fling),
            [scui_event_ptr_move] =                 scui_stringify(scui_event_ptr_move),
            [scui_event_ptr_up] =                   scui_stringify(scui_event_ptr_up),
            /* 系统事件: enc */
            [scui_event_enc_clockwise] =            scui_stringify(scui_event_enc_clockwise),
            [scui_event_enc_clockwise_anti] =       scui_stringify(scui_event_enc_clockwise_anti),
            /* 系统事件: key */
            [scui_event_key_down] =                 scui_stringify(scui_event_key_down),
            [scui_event_key_hold] =                 scui_stringify(scui_event_key_hold),
            [scui_event_key_click] =                scui_stringify(scui_event_key_click),
            [scui_event_key_up] =                   scui_stringify(scui_event_key_up),
        };
        
        return event_sys_str[type];
    }
    
    if (scui_event_type_custom(type)) {
        
        static const char * event_custom_str[scui_event_custom_num - scui_event_sys_num] = {
            [scui_event_ui_none_goto                - scui_event_sys_num] = scui_stringify(scui_event_ui_none_goto),
            [scui_event_ui_home_goto                - scui_event_sys_num] = scui_stringify(scui_event_ui_home_goto),
            [scui_event_ui_standy_enter             - scui_event_sys_num] = scui_stringify(scui_event_ui_standy_enter),
            [scui_event_ui_standy_exit              - scui_event_sys_num] = scui_stringify(scui_event_ui_standy_exit),
        };
        
        return event_custom_str[type - scui_event_sys_num];
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
    case scui_event_ptr_click:
    case scui_event_key_click:
        
        if (scui_window_active_curr() == SCUI_UI_SCENE_STANDBY) {
            
            scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false, scui_event_ui_standy_exit, NULL);
            scui_event_notify(&event_ui);
            
            app_module_system_dlps_set(false);
        } else {
            
        }
        break;
    default:
        break;
    }
    
    switch (event->type) {
    case scui_event_ptr_click:
        SCUI_LOG_INFO("event ptr click:%d", event->ptr_cnt);
        break;
    case scui_event_key_click:
        SCUI_LOG_INFO("event key click:%d", event->key_cnt);
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
            // 息屏时清理一下碎片
            scui_image_cache_clear();
            scui_font_cache_clear();
            scui_font_glyph_cache_clear();
            
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
            scui_window_stack_del(SCUI_UI_SCENE_STANDBY);
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
    case scui_event_ptr_fling:
        if (app_module_system_dlps_get())
            break;
        
        // 全局的右滑响应事件
        if (scui_indev_ptr_dir(event) == scui_opt_dir_to_r) {
            
            scui_event_mask_over(event);
            scui_ui_scene_return();
        }
        break;
    case scui_event_key_click:
        if (app_module_system_dlps_get())
            break;
        
        // 全局的按键响应事件
        scui_event_mask_over(event);
        scui_ui_scene_return();
        break;
    default:
        break;
    }
}
