/*
 *实现目的:
 *    lvgl线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "lv_drv_conf.h"
#include "app_lv_mouse.h"
#include "app_lv_mousewheel.h"
#include "app_lv_keyboard.h"
#include "app_lv_display.h"
#include "app_lv_driver.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_check_time.h"
#include "app_lv_ui_watch.h"

/*@brief lvgl线程模组初始化
 */
void app_thread_lvgl_ready(void)
{
    /* 初始化框架 */
    lv_init();
    /* 初始化与lvgl绑定的驱动设备 */
    app_lv_driver_ready();
    /* 内部调用 */
    app_lv_ui_check_time_ready();
}

/*@brief lvgl线程服务例程
 */
void app_thread_lvgl_routine(void)
{
    app_sem_t *sem = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_lvgl, &sem);
    app_thread_get_pipe_by_id(app_thread_id_lvgl, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread lvgl recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
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
                    app_lv_ui_watch_status_update(app_lv_ui_watch_dlps);
                    app_lv_scene_add(&app_lv_scene_watch, false);
                    /* 关闭设备(业务需求,不就地关闭鼠标,鼠标需要有唤醒能力) */
                    app_lv_display_dlps_enter();
                    app_lv_keyboard_dlps_enter();
                    app_lv_mousewheel_dlps_enter();
                    // app_lv_mouse_dlps_enter();
                }
                /* 与lvgl绑定的驱动设备退出DLPS */
                if (package.event == app_thread_lvgl_sched_dlps_exit) {
                    /* 计时重置 */
                    app_lv_ui_check_time_reset(0, 0);
                    /* 开启设备 */
                    app_lv_mouse_dlps_exit();
                    app_lv_mousewheel_dlps_exit();
                    app_lv_keyboard_dlps_exit();
                    app_lv_display_dlps_exit();
                    /* 退出dlps界面 */
                    app_lv_scene_t scene = {0};
                    app_lv_scene_del(&scene);
                }
                break;
            }
            case app_thread_lvgl_ui_scene: {
                /* 测试时使用,在此处添加自定义窗口 */
                #if 0
                    /* to do something */
                #else
                /* 启动UI场景 */
                if (package.event == app_thread_lvgl_ui_scene_start) {
                    app_lv_scene_reset(&app_lv_scene_main, false);
                    app_lv_ui_watch_status_update(app_lv_ui_watch_start);
                    app_lv_scene_add(&app_lv_scene_watch, false);
                    app_lv_ui_check_time_exec(true);
                    break;
                }
                /* 终止UI场景 */
                if (package.event == app_thread_lvgl_ui_scene_stop) {
                    app_lv_scene_reset(&app_lv_scene_main, false);
                    app_lv_ui_watch_status_update(app_lv_ui_watch_stop);
                    app_lv_scene_add(&app_lv_scene_watch, false);
                    app_lv_ui_check_time_exec(false);
                    break;
                }
                /* UI场景计时检查 */
                if (package.event == app_thread_lvgl_ui_scene_check_time) {
                    app_lv_ui_check_time_update();
                    break;
                }
                /* 产生到特定场景内的事件...... */
                app_lv_ui_scene_event(package.event, package.data, package.size);
                if (package.dynamic)
                    app_mem_free(package.data);
                #endif
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_SYS_LOG_ERROR("thread lvgl pipe recv a unknown package");
                APP_SYS_LOG_ERROR("package send_tid:%u", package.send_tid);
                APP_SYS_LOG_ERROR("package recv_tid:%u", package.recv_tid);
                APP_SYS_LOG_ERROR("package module:%u",   package.module);
                APP_SYS_LOG_ERROR("package event:%u",    package.event);
                APP_SYS_LOG_ERROR("package data:%p",     package.data);
                APP_SYS_LOG_ERROR("package size:%u",     package.size);
                #endif
                break;
            }
            }
        }
    }
}
