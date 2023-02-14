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
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "lv_drv_conf.h"
#include "app_lv_driver.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene_set.h"
#include "app_lv_ui_watch.h"

/*@brief lvgl线程模组初始化
 */
void app_thread_lvgl_ready(void)
{
    /* 初始化框架 */
    lv_init();
    /* 初始化与lvgl绑定的驱动设备 */
    app_lv_driver_ready();
}

/*@brief lvgl线程服务例程
 */
void app_thread_lvgl_routine(void)
{
    app_sem_t  *sem  = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync_by_id(app_thread_id_lvgl, &sem);
    app_thread_get_pipe_by_id(app_thread_id_lvgl, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread lvgl recv too much package:%u\n",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_lvgl_work: {
                /* 我们利用data和size传递内部特殊信息 */
                if (package.data != NULL && package.size == (sizeof(void **) * 2)) {
                    void (*routine)(void *parameter) = ((void **)package.data)[0];
                    void  *parameter                 = ((void **)package.data)[1];
                    routine(parameter);
                }
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
                    static bool app_lv_drv_shutdown = false;
                    /* 如果lvgl驱动未就绪,中止事件调度 */
                    if (app_lv_driver_status_get())
                        app_lv_driver_handler();
                    if (app_lv_drv_shutdown)
                        break;
                    if (app_lv_driver_shutdown()) {
                        app_lv_drv_shutdown = true;
                        /* 重启系统 */
                        APP_SYS_LOG_WARN("app_lv_shutdown\n");
                        app_module_system_delay_set(2);
                        app_module_system_status_set(app_module_system_reset);
                    }
                }
                /* lvgl场景处理事件 */
                if (package.event == app_thread_lvgl_sched_scene) {
                    app_lv_scene_sched(package.data);
                }
                /* 与lvgl绑定的驱动设备进入DLPS */
                if (package.event == app_thread_lvgl_sched_dlps_enter) {
                    app_lv_driver_dlps_enter();
                    app_lv_scene_reset(&app_lv_scene_main);
                }
                /* 与lvgl绑定的驱动设备退出DLPS */
                if (package.event == app_thread_lvgl_sched_dlps_exit) {
                    app_lv_driver_dlps_exit();
                    app_lv_scene_reset(&app_lv_scene_main);
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
                    app_lv_scene_reset(&app_lv_scene_main);
                    app_lv_ui_watch_status_update(app_lv_ui_watch_start);
                    app_lv_scene_add(&app_lv_scene_watch);
                }
                /* 终止UI场景 */
                if (package.event == app_thread_lvgl_ui_scene_stop) {
                    app_lv_scene_reset(&app_lv_scene_main);
                    app_lv_ui_watch_status_update(app_lv_ui_watch_stop);
                    app_lv_scene_add(&app_lv_scene_watch);
                }
                #endif
                break;
            }
            default: {
                #if APP_THREAD_CHECK
                APP_SYS_LOG_ERROR("\n");
                APP_SYS_LOG_ERROR("thread lvgl pipe recv a unknown package\n");
                APP_SYS_LOG_ERROR("package send_tid:%u\n", package.send_tid);
                APP_SYS_LOG_ERROR("package recv_tid:%u\n", package.recv_tid);
                APP_SYS_LOG_ERROR("package module:%u\n",   package.module);
                APP_SYS_LOG_ERROR("package event:%u\n",    package.event);
                APP_SYS_LOG_ERROR("package data:%p\n",     package.data);
                APP_SYS_LOG_ERROR("package size:%u\n",     package.size);
                APP_SYS_LOG_ERROR("\n");
                #endif
                break;
            }
            }
        }
    }
}
