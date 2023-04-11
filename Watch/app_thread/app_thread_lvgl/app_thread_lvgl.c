/*
 *实现目的:
 *    lvgl线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "lv_drv_conf.h"
#include "app_lv_drv.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_event_object.h"
#include "app_lv_ui_event_scene.h"
#include "app_lv_ui_check_time.h"
#include "app_lv_ui_scene_remind.h"
#include "app_lv_ui_test.h"

/*@brief lvgl线程模组初始化
 */
void app_thread_lvgl_ready(void)
{
    /* 框架初始化 */
    lv_init();
    /* 初始化与lvgl绑定的驱动设备 */
    #if APP_LV_DRV_USE_WIN == 0
        app_lv_driver_ready();
    #endif
    /* 模组初始化 */
    app_lv_ui_check_time_ready();
}

/*@brief lvgl线程服务例程
 */
void app_thread_lvgl_routine(void)
{
    app_sem_t *sem = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync(app_thread_id_lvgl, &sem);
    app_thread_get_pipe(app_thread_id_lvgl, &pipe);
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 将其从上面的接口中推延到此处 */ {
        /* 初始化与lvgl绑定的驱动设备 */
        #if APP_LV_DRV_USE_WIN == 1
            app_lv_driver_ready();
        #endif
    }
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread lvgl recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_lvgl_system: {
                if (package.event == app_thread_group_work)
                    app_sys_work_execute((void *)package.data);
                break;
            }
            case app_thread_lvgl_sched: {
                /* lvgl时钟约减事件 */
                if (package.event == app_thread_lvgl_sched_inc) {
                    lv_tick_inc(LV_SCHED_TICK_INC);
                }
                /* lvgl时钟调度事件 */
                if (package.event == app_thread_lvgl_sched_exec) {
                    lv_timer_handler();
                }
                /* lvgl驱动检查事件 */
                if (package.event == app_thread_lvgl_sched_drv) {
                    app_lv_driver_handler();
                }
                /* lvgl场景处理事件 */
                if (package.event == app_thread_lvgl_sched_scene) {
                    app_lv_scene_sched(package.data);
                }
                /* 与lvgl绑定的驱动设备进入DLPS */
                if (package.event == app_thread_lvgl_sched_dlps_enter) {
                    /* 进入dlps界面 */
                    app_lv_scene_add(&app_lv_ui_watch_dlps, false);
                    /* 关闭设备(业务需求,不就地关闭鼠标,鼠标需要有唤醒能力) */
                    app_lv_display_dlps_enter();
                    // app_lv_keyboard_dlps_enter();
                    app_lv_mousewheel_dlps_enter();
                    // app_lv_mouse_dlps_enter();
                }
                /* 与lvgl绑定的驱动设备退出DLPS */
                if (package.event == app_thread_lvgl_sched_dlps_exit) {
                    /* 计时重置 */
                    app_lv_ui_check_time_reset(0, 0);
                    app_lv_ui_check_time_exec(true);
                    /* 开启设备 */
                    app_lv_mouse_dlps_exit();
                    app_lv_mousewheel_dlps_exit();
                    // app_lv_keyboard_dlps_exit();
                    app_lv_display_dlps_exit();
                    /* 退出dlps界面 */
                    app_lv_scene_t scene = {0};
                    app_lv_scene_del(&scene);
                }
                break;
            }
            case app_thread_lvgl_ui_scene: {
                #if APP_LV_UI_TEST_USE
                app_lv_ui_test_scene();
                #else
                /* 启动UI场景 */
                if (package.event == app_thread_lvgl_ui_scene_start) {
                    app_lv_ui_event_default_config(NULL, true, NULL);
                    app_lv_ui_check_time_reset(0, 0);
                    app_lv_ui_check_time_exec(true);
                    APP_SYS_LOG_WARN("ui scene start");
                    app_lv_scene_reset(&app_lv_ui_main, false);
                    #if !APP_LV_UI_MODE_DEVELOPMENT
                    app_lv_scene_add(&app_lv_ui_watch_start, false);
                    #endif
                }
                /* 终止UI场景 */
                if (package.event == app_thread_lvgl_ui_scene_stop) {
                    app_lv_ui_event_default_config(NULL, false, NULL);
                    app_lv_ui_check_time_reset(0, 0);
                    app_lv_ui_check_time_exec(true);
                if (app_module_system_dlps_get())
                    app_module_system_dlps_set(false);
                    APP_SYS_LOG_WARN("ui scene stop");
                    app_lv_scene_reset(&app_lv_ui_main, false);
                    #if !APP_LV_UI_MODE_DEVELOPMENT
                    app_lv_scene_add(&app_lv_ui_watch_stop, false);
                    #endif
                }
                /* UI场景计时检查 */
                if (package.event == app_thread_lvgl_ui_scene_check_time) {
                    app_lv_ui_check_time_update();
                }
                #endif
                /* 集成场景 */
                switch (package.event) {
                /* 倒计时提醒 */
                case app_thread_lvgl_ui_countdown_remind: {
                    app_lv_ui_scene_remind(&app_lv_ui_countdown_remind);
                    break;
                }
                /* 提醒闹钟 */
                case app_thread_lvgl_ui_remind_alarm: {
                    app_module_remind_package_t *remind = package.data;
                    remind->remind_group;
                    remind->remind_item;
                    remind->remind_type;
                    if (package.dynamic)
                        app_mem_free(package.data);
                    break;
                }
                /* 提醒走动 */
                case app_thread_lvgl_ui_remind_sedentary: {
                    app_lv_ui_scene_remind(&app_lv_ui_remind_sedentary);
                    break;
                }
                /* 提醒喝水 */
                case app_thread_lvgl_ui_remind_drink: {
                    app_lv_ui_scene_remind(&app_lv_ui_remind_drink);
                    break;
                }
                default:
                    break;
                }
                break;
            }
            default: {
                #if APP_SYS_LOG_THREAD_CHECK
                APP_SYS_LOG_ERROR("thread lvgl pipe recv a unknown package");
                APP_SYS_LOG_ERROR("package thread:%u", package.thread);
                APP_SYS_LOG_ERROR("package module:%u", package.module);
                APP_SYS_LOG_ERROR("package event:%u",  package.event);
                APP_SYS_LOG_ERROR("package data:%p",   package.data);
                APP_SYS_LOG_ERROR("package size:%u",   package.size);
                #endif
                break;
            }
            }
        }
    }
}
