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
    /* 框架初始化 */
    lv_init();
    /* 初始化与lvgl绑定的驱动设备 */
    app_lv_driver_ready();
    /* 模组初始化 */
    app_lv_timer_ready();
    /* 模组初始化 */
    app_lv_check_time_ready();
    /* 初始化启动lvgl调度定时器 */
    app_lv_timer_start();
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_lvgl_routine_package_cb(app_thread_package_t *package, uint32_t *discard_count)
{
    switch (package->module) {
    case app_thread_lvgl_sched: {
        /* lvgl时钟约减事件 */
        if (package->event == app_thread_lvgl_sched_inc) {
            lv_tick_inc(LV_SCHED_TICK_INC);
        }
        /* lvgl时钟调度事件 */
        if (package->event == app_thread_lvgl_sched_exec) {
            /* 计算事件处理时间(开始) */
            app_execute_us_t execute_us = {0};
            app_execute_us(&execute_us, true);
            lv_timer_handler();
            /* 计算事件处理时间(结束) */
            uint32_t ms = app_execute_us(&execute_us, false) / 1000.0;
            /* 此处适应性降帧 */
            if (ms > APP_THREAD_SLAVE_EXECUTE_TIME_CHECK_MS)
                *discard_count = ms / LV_SCHED_TICK_EXEC + 1;
        }
        /* lvgl驱动检查事件 */
        if (package->event == app_thread_lvgl_sched_drv) {
            app_lv_driver_handler();
        }
        /* lvgl wheel更新事件 */
        if (package->event == app_thread_lvgl_sched_wheel) {
            app_lv_wheel_update_handle(package->data);
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
            app_lv_display_dlps_enter();
            // app_lv_keyboard_dlps_enter();
            app_lv_mousewheel_dlps_enter();
            // app_lv_mouse_dlps_enter();
        }
        /* 与lvgl绑定的驱动设备退出DLPS */
        if (package->event == app_thread_lvgl_sched_dlps_exit) {
            /* 计时重置 */
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(true);
            /* 开启设备 */
            app_lv_mouse_dlps_exit();
            app_lv_mousewheel_dlps_exit();
            // app_lv_keyboard_dlps_exit();
            app_lv_display_dlps_exit();
            /* 退出dlps界面 */
            app_lv_scene_t *scene = NULL;
            app_lv_scene_del(&scene);
        }
        return true;
    }
    case app_thread_lvgl_sched_ui: {
        /* 测试模式拦截该模组全部事件 */
        #if APP_LV_UI_TEST_USE
        /* 禁用超时回退 */
        app_lv_check_time_reset(0, 0);
        app_lv_check_time_exec(false);
        app_lv_scene_reset(&app_lv_ui_test, false);
        return true;
        #endif
        
        /* 启动UI场景 */
        if (package->event == app_thread_lvgl_ui_scene_start) {
            app_lv_event_ui_default_config(NULL, true, NULL);
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(true);
            APP_SYS_LOG_WARN("ui scene start");
            app_lv_scene_reset(&app_lv_ui_watch_face, false);
            app_lv_scene_add(&app_lv_ui_start, false);
            /* 更新lvgl设备 */
            app_lv_mouse_dlps_exit();
            app_lv_mousewheel_dlps_exit();
            // app_lv_keyboard_dlps_exit();
            app_lv_display_dlps_exit();
        }
        /* 终止UI场景 */
        if (package->event == app_thread_lvgl_ui_scene_stop) {
            app_lv_event_ui_default_config(NULL, false, NULL);
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(true);
        if (app_module_system_dlps_get())
            app_module_system_dlps_set(false);
            APP_SYS_LOG_WARN("ui scene stop");
            app_lv_scene_reset(&app_lv_ui_watch_face, false);
            app_lv_scene_add(&app_lv_ui_stop, false);
            /* 更新lvgl设备 */
            app_lv_display_dlps_enter();
            app_lv_keyboard_dlps_enter();
            app_lv_mousewheel_dlps_enter();
            app_lv_mouse_dlps_enter();
        }
        /* 进入UI场景(关机) */
        if (package->event == app_thread_lvgl_ui_scene_shutdown) {
            /* 禁用UI的一切交互,仅保留按压事件响应 */
            app_lv_event_ui_default_config(NULL, true, NULL);
            app_lv_check_time_reset(0, 0);
            app_lv_check_time_exec(false);
            APP_SYS_LOG_WARN("ui scene shutdown");
            app_lv_scene_reset(&app_lv_ui_dlps, false);
            /* 更新lvgl设备 */
            app_lv_display_dlps_enter();
            // app_lv_keyboard_dlps_enter();
            app_lv_mousewheel_dlps_enter();
            app_lv_mouse_dlps_enter();
            /* 反初始化文件系统 */
            app_third_fatfs_deinit();
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
