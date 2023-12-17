/*实现目标:
 *    lvgl线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_third_fatfs.h"
#include "app_lv_lib.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_lvgl_routine_ready_cb(void)
{
    /* 初始化文件系统 */
    app_third_fatfs_init();
    /* 反初始化文件系统 */
    // app_third_fatfs_deinit();
    /* 框架初始化 */
    lv_init();
    /* 初始化与lvgl绑定的驱动设备 */
    app_dev_gui_disp_ready(&app_dev_gui_disp);
    app_dev_gui_ptr_ready(&app_dev_gui_ptr);
    app_dev_gui_enc_ready(&app_dev_gui_enc);
    app_dev_gui_key_ready(&app_dev_gui_key);
    app_dev_gui_drv_ready(&app_dev_gui_drv);
    /* 模组初始化 */
    app_lv_timer_ready();
    /* 模组初始化 */
    app_lv_check_time_ready();
    /* 初始化启动lvgl调度定时器 */
    app_lv_timer_start();
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_lvgl_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_lvgl_sched: {
        /* lvgl时钟约减事件 */
        if (package->event == app_thread_lvgl_sched_inc) {
            lv_tick_inc(LV_SCHED_TICK_INC * package->byte_fixed);
            *record = false;
        }
        /* lvgl时钟调度事件 */
        if (package->event == app_thread_lvgl_sched_exec) {
            /* 计算事件处理时间(开始) */
            // app_execute_us_t execute_us = {0};
            // app_execute_us(&execute_us, true);
            lv_timer_handler();
            /* 计算事件处理时间(结束) */
            // uint32_t ms = app_execute_us(&execute_us, false) / 1000.0;
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
                    if (app_module_system_mode_get() != app_module_data_center_system_mode_shutdown)
                        app_module_system_mode_set(app_module_data_center_system_mode_shutdown);
                    else
                        app_arch_reset();
                }
            }
            *record = false;
        }
        /* lvgl wheel更新事件 */
        if (package->event == app_thread_lvgl_sched_wheel) {
            app_lv_wheel_update_handle(package->data);
            #if 0
            /* 补充:如果调度主界面不是主界面,重定位它 */
            if (app_lv_scene_get_nest() == 1) {
                app_lv_scene_t *current = NULL;
                app_lv_scene_get_top(&current);
                if (current != &app_lv_ui_watch_face)
                    app_lv_scene_cover(&app_lv_ui_watch_face);
            }
            #endif
            /*  */
            if (package->dynamic)
                app_mem_free(package->data);
        }
        /* lvgl场景计时检查 */
        if (package->event == app_thread_lvgl_sched_check_time) {
            app_lv_check_time_update();
        }
        /* 与lvgl绑定的驱动设备进入DLPS */
        if (package->event == app_thread_lvgl_sched_dlps_enter) {
            /* 进入dlps界面 */
            app_lv_scene_add(&app_lv_ui_dlps, false);
            /* 关闭设备(业务需求,不就地关闭鼠标,鼠标需要有唤醒能力) */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            // app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        /* 与lvgl绑定的驱动设备退出DLPS */
        if (package->event == app_thread_lvgl_sched_dlps_exit) {
            /* 计时重置 */
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(true);
            /* 开启设备 */
            app_dev_gui_disp_dlps_exit(&app_dev_gui_disp);
            app_dev_gui_key_dlps_exit(&app_dev_gui_key);
            app_dev_gui_enc_dlps_exit(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_exit(&app_dev_gui_ptr);
            /* 退出dlps界面 */
            app_lv_scene_t *scene = NULL;
            app_lv_scene_del(&scene);
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
            app_lv_event_default_config(NULL, true, NULL);
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(true);
            APP_SYS_LOG_WARN("ui scene start");
            app_lv_scene_reset(&app_lv_ui_watch_face, false);
            app_lv_scene_add(&app_lv_ui_start, false);
            /* 设置默认字体和默认尺寸 */
            app_lv_multi_font_type_config(APP_LV_MULTI_FONT_TYPE_DEFAULT);
            app_lv_multi_font_size_config(APP_LV_MULTI_FONT_SIZE_DEFAULT);
            /* 更新lvgl设备 */
            app_dev_gui_disp_dlps_exit(&app_dev_gui_disp);
            app_dev_gui_key_dlps_exit(&app_dev_gui_key);
            app_dev_gui_enc_dlps_exit(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_exit(&app_dev_gui_ptr);
        }
        /* 终止UI场景 */
        if (package->event == app_thread_lvgl_ui_scene_stop) {
            app_lv_event_default_config(NULL, false, NULL);
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(true);
            app_module_system_dlps_set(false);
            APP_SYS_LOG_WARN("ui scene stop");
            app_lv_scene_reset(&app_lv_ui_watch_face, false);
            app_lv_scene_add(&app_lv_ui_stop, false);
            /* 更新lvgl设备 */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        /* 进入UI场景(关机) */
        if (package->event == app_thread_lvgl_ui_scene_shutdown) {
            /* 禁用UI的一切交互,仅保留按压事件响应 */
            app_lv_event_default_config(NULL, true, NULL);
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(false);
            APP_SYS_LOG_WARN("ui scene shutdown");
            app_lv_scene_reset(&app_lv_ui_watch_face, false);
            app_lv_scene_add(&app_lv_ui_dlps, false);
            /* 更新lvgl设备 */
            app_dev_gui_disp_dlps_enter(&app_dev_gui_disp);
            app_dev_gui_key_dlps_enter(&app_dev_gui_key);
            app_dev_gui_enc_dlps_enter(&app_dev_gui_enc);
            app_dev_gui_ptr_dlps_enter(&app_dev_gui_ptr);
        }
        
        /* 集成场景(开始) */
        
        /* 倒计时提醒 */
        if (package->event == app_thread_lvgl_ui_countdown_remind) {
            app_lv_ui_scene_remind(&app_lv_ui_countdown_remind);
        }
        /* 提醒闹钟 */
        if (package->event == app_thread_lvgl_ui_remind_alarm) {
            app_module_remind_package_t *remind = package->data;
            remind->remind_group;
            remind->remind_item;
            remind->remind_type;
            if (package->dynamic)
                app_mem_free(package->data);
        }
        /* 提醒走动 */
        if (package->event == app_thread_lvgl_ui_remind_sedentary) {
            app_lv_ui_scene_remind(&app_lv_ui_remind_sedentary);
        }
        /* 提醒喝水 */
        if (package->event == app_thread_lvgl_ui_remind_drink) {
            app_lv_ui_scene_remind(&app_lv_ui_remind_drink);
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
