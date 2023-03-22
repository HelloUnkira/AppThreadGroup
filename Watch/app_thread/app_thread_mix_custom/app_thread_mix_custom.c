/*
 *实现目的:
 *    杂项事件不归属与其他线程模组时放于此处
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_alarm.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_do_not_disturb.h"
#include "app_module_drink_remind.h"

/*@brief 混合事件线程模组初始化
 */
void app_thread_mix_custom_ready(void)
{
    /* 模组初始化 */
    app_module_alarm_group_ready();
    app_module_stopwatch_ready();
    app_module_countdown_ready();
    app_module_do_not_disturb_ready();
    app_module_drink_remind_ready();
}

/*@brief 混合事件线程服务例程
 */
void app_thread_mix_custom_routine(void)
{
    app_sem_t *sem = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync(app_thread_id_mix_custom, &sem);
    app_thread_get_pipe(app_thread_id_mix_custom, &pipe);
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread mix custom recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe)) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_mix_custom_system: {
                if (package.event == app_thread_group_work)
                    app_sys_work_execute((void *)package.data);
                break;
            }
            case app_thread_mix_custom_alarm: {
                if (package.size != sizeof(app_module_alarm_t))
                    break;
                if (package.data == NULL)
                    break;
                app_module_alarm_t *alarm = package.data;
                /* 根据实际的情况处理或者转发事件包到特定的线程特定的模组 */
                if (package.event == app_thread_mix_custom_alarm_expired);
                /* 常规式闹钟事件 */
                if (package.event == app_thread_mix_custom_alarm_month);
                if (package.event == app_thread_mix_custom_alarm_week);
                /* 提醒式闹钟事件 */
                if (package.event == app_thread_mix_custom_alarm_repeat);
                break;
            }
            case app_thread_mix_custom_stopwatch: {
                /* 秒表模组更新事件 */
                if (package.event == app_thread_mix_custom_stopwatch_msec_update)
                    app_module_stopwatch_xmsec_update();
                break;
            }
            case app_thread_mix_custom_countdown: {
                /* 倒计时模组更新事件 */
                if (package.event == app_thread_mix_custom_stopwatch_msec_update)
                    app_module_countdown_xmsec_update();
                /* 倒计时模组到期事件 */
                if (package.event == app_thread_mix_custom_countdown_expired) {
                    app_package_t package = {
                        .thread  = app_thread_id_lvgl,
                        .module  = app_thread_lvgl_ui_scene,
                        .event   = app_thread_lvgl_ui_countdown_remind,
                    };
                    app_package_notify(&package);
                }
                break;
            }
            case app_thread_mix_custom_drink_remind: {
                if (package.event == app_thread_mix_custom_drink_remind_update)
                    app_module_drink_remind_xmin_update();
                /* 喝水提醒模组到期事件 */
                if (package.event == app_thread_mix_custom_drink_remind_interval) {
                    app_package_t package = {
                        .thread  = app_thread_id_lvgl,
                        .module  = app_thread_lvgl_ui_scene,
                        .event   = app_thread_lvgl_ui_drink_remind,
                    };
                    app_package_notify(&package);
                }
                break;
            }
            default: {
                #if APP_SYS_LOG_THREAD_CHECK
                APP_SYS_LOG_ERROR("thread mix custom pipe recv a unknown package");
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
