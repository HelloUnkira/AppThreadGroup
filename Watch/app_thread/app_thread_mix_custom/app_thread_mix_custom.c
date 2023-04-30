/*
 *实现目的:
 *    杂项事件不归属与其他线程模组时放于此处
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_remind_sedentary.h"
#include "app_module_remind_drink.h"
#include "app_module_do_not_disturb.h"

/*@brief 混合事件线程模组初始化
 */
void app_thread_mix_custom_ready(void)
{
    /* 模组初始化 */
    app_module_stopwatch_ready();
    app_module_countdown_ready();
    app_module_remind_group_ready();
    app_module_remind_alarm_ready();
    app_module_remind_drink_ready();
    app_module_do_not_disturb_ready();
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
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 将其从上面的接口中推延到此处 */ {
    }
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread mix custom recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package, false);
            /* 计算事件处理时间(开始) */
            #if APP_SYS_LOG_EXECUTE
            bool execute_ms_remind = true;
            app_execute_ms_t execute_ms = {0};
            app_execute_ms(&execute_ms, true);
            #endif
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_mix_custom_system: {
                if (package.event == app_thread_group_work)
                    app_sys_work_execute((void *)package.data);
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
            case app_thread_mix_custom_remind_group: {
                if (package.event == app_thread_mix_custom_remind_group_update) {
                    app_module_remind_group_reflush();
                }
                if (package.event == app_thread_mix_custom_remind_group_package) { 
                    /* 提醒组事件分拣 */
                    app_module_remind_package_t *remind = package.data;
                    /* 该提醒组事件来自提醒闹钟组: */
                    if (app_module_remind_alarm_group_check(remind->remind_group)) {
                        /* 发送闹钟事件(直接转发) */
                        package.thread = app_thread_id_lvgl;
                        package.module = app_thread_lvgl_ui_scene;
                        package.event  = app_thread_lvgl_ui_remind_alarm;
                        app_package_notify(&package);
                    }
                }
                break;
            }
            case app_thread_mix_custom_remind_misc: {
                if (package.event == app_thread_mix_custom_remind_sedentary_update)
                    app_module_remind_sedentary_xmin_update();
                if (package.event == app_thread_mix_custom_remind_drink_update)
                    app_module_remind_drink_xmin_update();
                /* 走动提醒模组到期事件 */
                if (package.event == app_thread_mix_custom_remind_sedentary_interval) {
                    app_package_t package = {
                        .thread  = app_thread_id_lvgl,
                        .module  = app_thread_lvgl_ui_scene,
                        .event   = app_thread_lvgl_ui_remind_sedentary,
                    };
                    app_package_notify(&package);
                }
                /* 喝水提醒模组到期事件 */
                if (package.event == app_thread_mix_custom_remind_drink_interval) {
                    app_package_t package = {
                        .thread  = app_thread_id_lvgl,
                        .module  = app_thread_lvgl_ui_scene,
                        .event   = app_thread_lvgl_ui_remind_drink,
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
            /* 计算事件处理时间(结束) */
            #if APP_SYS_LOG_EXECUTE
            uint32_t ms = app_execute_ms(&execute_ms, false);
            if (ms > APP_SYS_LOG_EXECUTE_MS && execute_ms_remind) {
                APP_SYS_LOG_WARN("thread mix custom package execute %d ms", ms);
                APP_SYS_LOG_WARN("package thread:%u", package.thread);
                APP_SYS_LOG_WARN("package module:%u", package.module);
                APP_SYS_LOG_WARN("package event:%u",  package.event);
                APP_SYS_LOG_WARN("package data:%p",   package.data);
                APP_SYS_LOG_WARN("package size:%u",   package.size);
            }
            #endif
        }
    }
}
