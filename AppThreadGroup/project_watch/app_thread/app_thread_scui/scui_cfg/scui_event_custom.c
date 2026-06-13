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

/*@brief 事件响应
 *@param event 事件包
 */
static void scui_event_custom_system(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_engine_ready:
        scui_event_mask_over(event);
        scui_custom_ready();
        break;
    case scui_event_engine_show:
        scui_event_mask_over(event);
        scui_custom_show();
        break;
    
    case scui_event_check_time_over:
        scui_event_mask_over(event);
        SCUI_LOG_WARN("ui sleep");
        app_module_system_dlps_set(true);
        break;
    case scui_event_check_time_idle:
        scui_event_mask_over(event);
        SCUI_LOG_WARN("ui idle back home");
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_home_goto, NULL);
        scui_event_notify(&event_ui);
        break;
    
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
static void scui_event_custom_active(scui_event_t *event)
{
    /* 当我们遇到认为不能休眠的事件时 */
    /* scui_tick_active(); */
    
    /* 系统活跃标记 */
    switch (event->type) {
    case scui_event_ptr_hold:
    case scui_event_key_hold:
    case scui_event_enc_fdir:
    case scui_event_enc_bdir:
    case scui_event_scroll_start:
    case scui_event_scroll_over:
    case scui_event_scroll_keep:
        scui_tick_active();
        break;
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
static void scui_event_custom_window(scui_event_t *event)
{
    /* 自定义窗口事件响应 */
    if (event->object == SCUI_HANDLE_INVALID ||
        event->object == SCUI_HANDLE_SYSTEM  ||
        scui_widget_type(event->object) !=
        scui_widget_type_window)
        return;
    
    switch (event->type) {
    case scui_event_focus_get: {
        /* 窗口属性已迁移至 JSON 静态布局(scui_ui_maker.json默认参数 + scene JSON指定)
         * 此处保留骨架供运行时动态配置使用
         */
        break;
    }
    case scui_event_focus_lost: {
        break;
    }
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
static void scui_event_custom_vibrate(scui_event_t *event)
{
    /* 滚动时的震动 */
    switch (event->type) {
    case scui_event_scroll_start:
    case scui_event_scroll_over:
    case scui_event_scroll_keep:
        scui_presenter.vibrate_shot();
        break;
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_check(scui_event_t *event)
{
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

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_access(scui_event_t *event)
{
    scui_event_custom_system(event);
    scui_event_custom_active(event);
    scui_event_custom_window(event);
    
    scui_event_custom_check(event);
    scui_event_custom_vibrate(event);
    
    
    
    /* 此处退出休眠 */
    switch (event->type) {
    case scui_event_ptr_click:
    case scui_event_key_click:
        
        if (scui_window_active_curr() == SCUI_UI_SCENE_STANDBY) {
            
            scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false, scui_event_ui_standy_exit, NULL);
            scui_event_notify(&event_ui);
            
            app_module_system_dlps_set(false);
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
    /* 当前在待机窗口时 */
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    scui_window_stack_top(&handle_top);
    
    
    
    switch (event->type) {
    case scui_event_ui_none_goto: {
        scui_event_mask_over(event);
        
        if (handle_top != SCUI_UI_SCENE_NONE) {
            scui_window_stack_reset_by(SCUI_UI_SCENE_NONE,
                scui_window_switch_none, scui_opt_dir_none, false);
        }
        break;
    }
    case scui_event_ui_home_goto: {
        scui_event_mask_over(event);
        if (!scui_event_check_execute(event))
             break;
        
        /* 如果本来是在待机界面时 */
        if (handle_top == SCUI_UI_SCENE_STANDBY) {
            scui_window_stack_reset_by(SCUI_UI_SCENE_HOME,
                scui_window_switch_none, scui_opt_dir_none, true);
        } else {
            scui_window_stack_reset_by(SCUI_UI_SCENE_HOME,
                scui_window_switch_none, scui_opt_dir_none, false);
        }
        
        break;
    }
    case scui_event_ui_standy_enter: {
        scui_event_mask_over(event);
        if (!scui_event_check_execute(event))
             break;
        
        /* 如果本来是在待机界面时 */
        if (handle_top != SCUI_UI_SCENE_STANDBY) {
            /* 息屏时清理一下碎片 */
            scui_cache_font_rectify();
            scui_cache_glyph_rectify();
            scui_cache_image_rectify();
            
            scui_window_stack_add_by(SCUI_UI_SCENE_STANDBY,
                scui_window_switch_none, scui_opt_dir_none, false);
        }
        break;
    }
    case scui_event_ui_standy_exit: {
        scui_event_mask_over(event);
        if (!scui_event_check_execute(event))
             break;
        
        if (handle_top == SCUI_UI_SCENE_STANDBY) {
            scui_window_stack_del_by(SCUI_UI_SCENE_STANDBY,
                scui_window_switch_none, scui_opt_dir_none);
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
    switch (event->type) {
    case scui_event_ptr_fling:
        if (app_module_system_dlps_get())
            break;
        
        /* 全局的右滑响应事件 */
        if (event->ptr_dir == scui_opt_dir_ltr) {
            
            scui_event_mask_over(event);
            scui_ui_scene_return();
        }
        break;
    case scui_event_key_click:
        if (app_module_system_dlps_get())
            break;
        
        /* 全局的按键响应事件 */
        scui_event_mask_over(event);
        scui_ui_scene_return();
        break;
    default:
        break;
    }
}
