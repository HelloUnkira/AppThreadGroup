/*实现目标:
 *    lvgl线程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"
#include "app_third_fatfs.h"
#include "lvgl_lib.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_lvgl_routine_ready_cb(void)
{
    /* 初始化文件系统 */
    app_third_fatfs_init();
    /* 框架初始化 */
    lv_init();
    /* 初始化与lvgl绑定的驱动设备 */
    app_dev_gui_disp_ready(&app_dev_gui_disp);
    app_dev_gui_ptr_ready(&app_dev_gui_ptr);
    app_dev_gui_enc_ready(&app_dev_gui_enc);
    app_dev_gui_key_ready(&app_dev_gui_key);
    app_dev_gui_drv_ready(&app_dev_gui_drv);
    /*  */
    lvgl_lib_ready();
    lvgl_theme_ready();
    lvgl_view_sched_ready();
    lvgl_view_event_ready();
    lvgl_view_sched_config_slide(lvgl_view_tr_move);
    lvgl_view_sched_config_jump(lvgl_view_tr_move, LV_DIR_LEFT);
    /*  */
    lvgl_ctime_ready();
    lvgl_popup_ready();
    
    // 初始在主表盘界面
    lvgl_view_stack_reset(lvgl_view_id_home_watch, false);
    
    app_lvgl_timer_ready();
    app_lvgl_timer_start();
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_lvgl_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_lvgl_sched: {
        /* lvgl时钟调度事件 */
        if (package->event == app_thread_lvgl_sched_exec) {
            /* 计算事件处理时间(开始) */
            // uint32_t tick_us_s = app_execute_us();
            lv_timer_handler();
            /* 计算事件处理时间(结束) */
            // uint32_t tick_us_e = app_execute_us();
            // uint32_t ms = (tick_us_e - tick_us_s) / 1000.0;
            // APP_SYS_LOG_INFO("exec ms:%d", ms);
            #if 0
            /* 此处适应性降帧 */
            if (ms > APP_THREAD_SLAVE_EXECUTE_TIME_CHECK_MS)
               *discard_count = ms / LV_SCHED_TICK_EXEC + 1;
            *record = false;
            #endif
        }
        /* lvgl驱动检查事件 */
        if (package->event == app_thread_lvgl_sched_drv) {
            app_dev_gui_drv_timer_handler(&app_dev_gui_drv);
            if (app_dev_gui_drv_shutdown(&app_dev_gui_drv)) {
                static bool execute = true;
                if (execute) {
                    execute = false;
                    /* 重启系统 */
                    APP_SYS_LOG_WARN("");
                    app_module_system_dlps_set(false);
                    app_module_system_valid_set(false);
                    #if 0
                    if (app_module_system_mode_get() != app_module_data_center_system_mode_shutdown)
                        app_module_system_mode_set(app_module_data_center_system_mode_shutdown);
                    else
                        app_arch_reset();
                    #endif
                }
            }
            *record = false;
        }
        /* 与lvgl绑定的驱动设备进入DLPS */
        if (package->event == app_thread_lvgl_sched_dlps_enter) {
            lvgl_view_event_param_t param = {0};
            param.type = lvgl_view_event_type_sleep_enter;
            lvgl_view_event_send(&param);
            
            /* 关闭设备(业务需求,不就地关闭鼠标,鼠标需要有唤醒能力) */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            // app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        /* 与lvgl绑定的驱动设备退出DLPS */
        if (package->event == app_thread_lvgl_sched_dlps_exit) {
            lvgl_view_event_param_t param = {0};
            param.type = lvgl_view_event_type_sleep_exit;
            lvgl_view_event_send(&param);
            
            /* 开启设备 */
            app_dev_gui_disp_dlps_exit(&app_dev_gui_disp);
            app_dev_gui_key_dlps_exit(&app_dev_gui_key);
            app_dev_gui_enc_dlps_exit(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_exit(&app_dev_gui_ptr);
        }
        return true;
    }
    case app_thread_lvgl_ui: {
        /* 测试模式拦截该模组全部事件 */
        #if 0
        /* 禁用超时回退 */
        app_lv_check_time_reset(0, 0);
        app_lv_check_time_exec(false);
        app_lv_scene_reset(&app_lv_ui_watch_face, false);
        return true;
        #endif
        
        /* 启动UI场景 */
        if (package->event == app_thread_lvgl_ui_scene_start) {
            APP_SYS_LOG_WARN("ui scene start");
            lvgl_ctime_resume();
            /* 更新lvgl设备 */
            app_dev_gui_disp_dlps_exit(&app_dev_gui_disp);
            app_dev_gui_key_dlps_exit(&app_dev_gui_key);
            app_dev_gui_enc_dlps_exit(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_exit(&app_dev_gui_ptr);
        }
        /* 终止UI场景 */
        if (package->event == app_thread_lvgl_ui_scene_stop) {
            APP_SYS_LOG_WARN("ui scene stop");
            lvgl_ctime_pause();
            
            app_module_system_dlps_set(false);
            /* 更新lvgl设备 */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        /* 进入UI场景(关机) */
        if (package->event == app_thread_lvgl_ui_scene_shutdown) {
            APP_SYS_LOG_WARN("ui scene shutdown");
            /* 更新lvgl设备 */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        
        /* 集成场景(开始) */
        
        /* 倒计时提醒 */
        if (package->event == app_thread_lvgl_ui_countdown_remind) {
        }
        /* 提醒闹钟 */
        if (package->event == app_thread_lvgl_ui_remind_alarm) {
        }
        /* 提醒走动 */
        if (package->event == app_thread_lvgl_ui_remind_sedentary) {
        }
        /* 提醒喝水 */
        if (package->event == app_thread_lvgl_ui_remind_drink) {
        }
        
        /* 集成场景(结束) */
        
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief lvgl线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_lvgl_routine)
{
    app_thread_slave_process(app_thread_id_lvgl,
                             app_thread_lvgl_routine_ready_cb,
                             app_thread_lvgl_routine_package_cb);
}
