/*实现目标:
 *    lvgl线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "lv_drv_conf.h"
#include "app_lv_drv.h"
#include "app_lv_event.h"
#include "app_lv_timer.h"
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
    app_lv_timer_ready();
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
        /* 初始化启动lvgl调度定时器 */
        app_lv_timer_start();
    }
    /* 主流程 */
    while (true) {
        app_sem_process(sem, app_sem_take);
        /* 计算事件处理时间(开始) */
        #if APP_SYS_LOG_EXECUTE
        app_execute_us_t execute_us = {0};
        app_execute_us(&execute_us, true);
        #endif
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_pkg_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread lvgl recv too much package:%u",
                              app_sys_pipe_pkg_num(pipe));
        #endif
        while (app_sys_pipe_pkg_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package, false);
            /* 计算事件处理时间(开始) */
            #if APP_SYS_LOG_EXECUTE_CHECK
            bool execute_us_remind = true;
            app_execute_us_t execute_us = {0};
            app_execute_us(&execute_us, true);
            #endif
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_lvgl_system: {
                if (package.event == app_thread_event_work)
                    app_sys_work_execute((void *)package.data);
                if (package.event == app_thread_event_works)
                    app_sys_works_execute((void *)package.data);
                break;
            }
            case app_thread_lvgl_sched: {
                /* lvgl时钟约减事件 */
                if (package.event == app_thread_lvgl_sched_inc) {
                    lv_tick_inc(LV_SCHED_TICK_INC);
                }
                /* lvgl时钟调度事件 */
                if (package.event == app_thread_lvgl_sched_exec) {
                    /* 计算事件处理时间(开始) */
                    app_execute_us_t execute_us = {0};
                    app_execute_us(&execute_us, true);
                    lv_timer_handler();
                    /* 计算事件处理时间(结束) */
                    uint32_t ms = app_execute_us(&execute_us, false) / 1000.0;
                    /* 系统对于此事件包的处理负载过大,主动丢弃一部分相同的事件包 */
                    /* 这是无奈之举,应该避免事件包的丢弃,这会对事件系统产生危害 */
                    /* 对于此处的影响是:适应性降帧 */
                    if (ms > APP_SYS_LOG_EXECUTE_CHECK_MS) {
                        uint32_t discard_cnt = ms / LV_SCHED_TICK_EXEC + 1;
                        for (uint32_t idx = 0; idx < discard_cnt; idx++) {
                            if (app_sys_pipe_pkg_num(pipe) != 0)
                                app_sys_pipe_take(pipe, &package, true);
                            else
                                break;
                        }
                        #if APP_SYS_LOG_EXECUTE_CHECK
                        APP_SYS_LOG_WARN("app_thread_lvgl_sched_exec ms:%d discard package:%d", ms, discard_cnt);
                        execute_us_remind = false;
                        #endif
                    }
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
                    app_lv_scene_add(&app_lv_ui_watch_start, false);
                    /* 更新lvgl设备 */
                    app_lv_mouse_dlps_exit();
                    app_lv_mousewheel_dlps_exit();
                    // app_lv_keyboard_dlps_exit();
                    app_lv_display_dlps_exit();
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
                    app_lv_scene_add(&app_lv_ui_watch_stop, false);
                    /* 更新lvgl设备 */
                    app_lv_display_dlps_enter();
                    app_lv_keyboard_dlps_enter();
                    app_lv_mousewheel_dlps_enter();
                    app_lv_mouse_dlps_enter();
                }
                /* 进入UI场景(关机) */
                if (package.event == app_thread_lvgl_ui_scene_shutdown) {
                    /* 禁用UI的一切交互,仅保留按压事件响应 */
                    app_lv_ui_event_default_config(NULL, true, NULL);
                    app_lv_ui_check_time_reset(0, 0);
                    app_lv_ui_check_time_exec(false);
                    APP_SYS_LOG_WARN("ui scene shutdown");
                    app_lv_scene_reset(&app_lv_ui_watch_dlps, false);
                    /* 更新lvgl设备 */
                    app_lv_display_dlps_enter();
                    // app_lv_keyboard_dlps_enter();
                    app_lv_mousewheel_dlps_enter();
                    app_lv_mouse_dlps_enter();
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
            /* 计算事件处理时间(结束) */
            #if APP_SYS_LOG_EXECUTE_CHECK
            uint32_t ms = app_execute_us(&execute_us, false) / 1000.0;
            /* 如果我们已经手动处理了该事件包,无需再检查 */
            if (ms > APP_SYS_LOG_EXECUTE_CHECK_MS && execute_us_remind) {
                APP_SYS_LOG_WARN("thread lvgl package execute %d ms", ms);
                APP_SYS_LOG_WARN("package thread:%u", package.thread);
                APP_SYS_LOG_WARN("package module:%u", package.module);
                APP_SYS_LOG_WARN("package event:%u",  package.event);
                APP_SYS_LOG_WARN("package data:%p",   package.data);
                APP_SYS_LOG_WARN("package size:%u",   package.size);
            }
            #endif
        }
        /* 计算事件处理时间(结束) */
        #if APP_SYS_LOG_EXECUTE
        double ms = app_execute_us(&execute_us, false) / 1000.0;
        app_thread_execute_us_set(app_thread_id_lvgl, &ms);
        #endif
    }
}
