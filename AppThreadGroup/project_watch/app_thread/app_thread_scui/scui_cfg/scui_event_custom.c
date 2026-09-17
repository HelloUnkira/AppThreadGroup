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
    
    
    
    case scui_event_ui_start: {
        scui_event_mask_over(event);
        SCUI_LOG_WARN("ui start");
        
        /* 开启超时模组 */
        scui_overtime_ready();
        scui_overtime_work(true);
        
        #if SCUI_UI_TEST
        /* 测试入口: 进测试主界面, ui_start延迟到点击main后 */
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_test_goto, NULL);
        scui_event_notify(&event_ui);
        #else
        /* 进入主界面 */
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_home_goto, NULL);
        scui_event_notify(&event_ui);
        #endif
        break;
    }
    case scui_event_ui_stop: {
        scui_event_mask_over(event);
        SCUI_LOG_WARN("ui stop");
        
        /* 关闭超时模组 */
        scui_overtime_work(false);
        
        /* 回空窗口 */
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_none_goto, NULL);
        scui_event_notify(&event_ui);
        break;
    }
    case scui_event_ui_pause: {
        scui_event_mask_over(event);
        SCUI_LOG_WARN("ui pause");
        
        /* 暂停超时模组 */
        scui_overtime_work(false);
        
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_standy_enter, NULL);
        scui_event_notify(&event_ui);
        break;
    }
    case scui_event_ui_resume: {
        scui_event_mask_over(event);
        SCUI_LOG_WARN("ui resume");
        
        /* 恢复超时模组 */
        scui_overtime_work(true);
        
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_standy_exit, NULL);
        scui_event_notify(&event_ui);
        break;
    }
    
    
    
    case scui_event_overtime: {
        scui_event_mask_over(event);
        SCUI_LOG_WARN("overtime sleep");
        
        /* 息屏: 记录息屏时刻 */
        scui_tick_idle(true);
        
        app_module_system_dlps_set(true);
        break;
    }
    
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
    case scui_event_enc_tick:
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
        /* 界面获得焦点: 按界面句柄值取超时配置(未登记界面用默认值) */
        const scui_check_time_t *cfg = scui_check_time_find(event->object);
        scui_overtime_reset(cfg != NULL ? cfg->sleep_tick : SCUI_CHECK_TIME_DEF);
        
        /* 为当前窗口动态配置sibling或者别的风格 */
        /* keep adding...... */
        
        break;
    }
    case scui_event_focus_lost: {
        break;
    }
    case scui_event_lang_mirror: {
        scui_event_mask_over(event);
        /* 界面句柄对框架不可见: 由自定义层重置到基准界面 */
        /* 重置会销毁并重建控件树, 借此完成全局RTL镜像刷新 */
        scui_handle_t handle_top = SCUI_HANDLE_INVALID;
        scui_window_stack_top(&handle_top);
        
        /* 基准界面: 测试态回测试主页, 其余回主界面 */
        scui_handle_t handle_base = handle_top == SCUI_UI_SCENE_TEST_UI_MAIN ?
            SCUI_UI_SCENE_TEST_UI_MAIN : SCUI_UI_SCENE_HOME;
        
        if (handle_top != handle_base) {
            /* 栈顶发生变更: 走常规跳转以重建控件树 */
            scui_event_define_absorb_none(event_ui, SCUI_HANDLE_SYSTEM, false,
                scui_ui_scene_test() ? scui_event_ui_test_goto : scui_event_ui_home_goto);
            scui_event_notify(&event_ui);
        } else {
            /* 栈顶不变时重置语义不生效, 以隐藏+显示强制重建控件树 */
            scui_widget_hide(handle_top, true);
            scui_widget_show(handle_top, true);
        }
    }
    default:
        break;
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
    
    /* 自定义事件统一标记为访问:偷懒!!! */
    scui_event_mask_keep(event);
    
    
    /* 此处退出休眠 */
    switch (event->type) {
    case scui_event_ptr_click:
    case scui_event_key_click:
        
        if (scui_window_active_curr() == SCUI_UI_SCENE_STANDBY) {
            scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
                scui_event_ui_standy_exit, NULL);
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
    scui_handle_t handle_top = SCUI_HANDLE_INVALID;
    scui_window_stack_top(&handle_top);
    
    
    
    switch (event->type) {
    case scui_event_ui_none_goto: {
        scui_event_mask_over(event);
        
        scui_window_stack_reset_by(SCUI_UI_SCENE_NONE,
            scui_window_switch_none, scui_opt_dir_none, false);
        break;
    }
    case scui_event_ui_test_goto: {
        scui_event_mask_over(event);
        
        scui_window_stack_reset_by(SCUI_UI_SCENE_TEST_UI_MAIN,
            scui_window_switch_none, scui_opt_dir_none, false);
        break;
    }
    case scui_event_ui_home_goto: {
        scui_event_mask_over(event);
        
        scui_window_stack_reset_by(SCUI_UI_SCENE_HOME,
            scui_window_switch_none, scui_opt_dir_none, false);
        
        break;
    }
    case scui_event_ui_standy_enter: {
        scui_event_mask_over(event);
        
        if (handle_top != SCUI_UI_SCENE_STANDBY) {
            scui_window_stack_add_by(SCUI_UI_SCENE_STANDBY,
                scui_window_switch_none, scui_opt_dir_none, false);
        }
        
        /* 进入息屏时清理一下碎片 */
        scui_cache_font_rectify();
        scui_cache_glyph_rectify();
        scui_cache_image_rectify();
        break;
    }
    case scui_event_ui_standy_exit: {
        scui_event_mask_over(event);
        
        if (handle_top == SCUI_UI_SCENE_STANDBY) {
            scui_window_stack_del_by(SCUI_UI_SCENE_STANDBY,
                scui_window_switch_none, scui_opt_dir_none);
        }
        
        /* 未登记的界面(如待机窗口)使用默认值 */
        /* 息屏时长超过当前界面的"超时返回主界面"时间则回主界面, 否则仅退出息屏 */
        const scui_check_time_t *cfg = scui_check_time_find(scui_window_active_curr());
        uint32_t home_tick = cfg != NULL ? cfg->home_tick : SCUI_CHECK_TIME_HOME_DEF;
        if (home_tick != SCUI_OVERTIME_UNUSE && scui_tick_idle(false) > home_tick * 1000) {
            
            scui_window_stack_reset_by(SCUI_UI_SCENE_HOME,
                scui_window_switch_none, scui_opt_dir_none, false);
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
