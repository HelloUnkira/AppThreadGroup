/*实现目标:
 *    杂项事件不归属与其他线程模组时放于此处
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_mix_custom_routine_ready_cb(void)
{
    /* 相关模组初始化 */
    app_module_stopwatch_ready();
    app_module_countdown_ready();
    app_module_remind_group_ready();
    app_module_remind_alarm_ready();
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_mix_custom_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_mix_custom_data_dump: {
        /* 将系统敏感的资源转储到外存 */
        app_module_data_dump_process();
        return true;
    }
    case app_thread_mix_custom_data_load: {
        /* 将系统敏感的资源加载到内存 */
        app_module_data_load_process();
        return true;
    }
    case app_thread_mix_custom_stopwatch: {
        /* 秒表模组更新事件 */
        if (package->event == app_thread_mix_custom_stopwatch_msec_update)
            app_module_stopwatch_xmsec_update();
        return true;
    }
    case app_thread_mix_custom_countdown: {
        /* 倒计时模组更新事件 */
        if (package->event == app_thread_mix_custom_stopwatch_msec_update)
            app_module_countdown_xmsec_update();
        /* 倒计时模组到期事件 */
        if (package->event == app_thread_mix_custom_countdown_expired) {
            #if APP_DEV_GUI_IS_LVGL
            app_thread_package_t package = {
                .thread  = app_thread_id_lvgl,
                .module  = app_thread_lvgl_ui,
                .event   = app_thread_lvgl_ui_countdown_remind,
            };
            app_thread_package_notify(&package);
            #endif
        }
        return true;
    }
    case app_thread_mix_custom_remind_group: {
        if (package->event == app_thread_mix_custom_remind_group_update) {
            app_module_remind_group_reflush();
        }
        if (package->event == app_thread_mix_custom_remind_group_package) { 
            /* 提醒组事件分拣 */
            app_module_remind_package_t *remind = package->data;
            /* 该提醒组事件来自提醒闹钟组: */
            if (app_module_remind_alarm_group_check(remind->remind_group)) {
                #if APP_DEV_GUI_IS_LVGL
                /* 发送闹钟事件(直接转发) */
                package->thread = app_thread_id_lvgl;
                package->module = app_thread_lvgl_ui;
                package->event  = app_thread_lvgl_ui_remind_alarm;
                app_thread_package_notify(package);
                #endif
            }
        }
        return true;
    }
    case app_thread_mix_custom_remind_misc: {
        if (package->event == app_thread_mix_custom_remind_sedentary_update)
            app_module_remind_sedentary_xmin_update();
        if (package->event == app_thread_mix_custom_remind_drink_update)
            app_module_remind_drink_xmin_update();
        /* 走动提醒模组到期事件 */
        if (package->event == app_thread_mix_custom_remind_sedentary_interval) {
            #if APP_DEV_GUI_IS_LVGL
            app_thread_package_t package = {
                .thread  = app_thread_id_lvgl,
                .module  = app_thread_lvgl_ui,
                .event   = app_thread_lvgl_ui_remind_sedentary,
            };
            app_thread_package_notify(&package);
            #endif
        }
        /* 喝水提醒模组到期事件 */
        if (package->event == app_thread_mix_custom_remind_drink_interval) {
            #if APP_DEV_GUI_IS_LVGL
            app_thread_package_t package = {
                .thread  = app_thread_id_lvgl,
                .module  = app_thread_lvgl_ui,
                .event   = app_thread_lvgl_ui_remind_drink,
            };
            app_thread_package_notify(&package);
            #endif
        }
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief 混合事件线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_mix_custom_routine)
{
    app_thread_slave_process(app_thread_id_mix_custom,
                             app_thread_mix_custom_routine_ready_cb,
                             app_thread_mix_custom_routine_package_cb);
}
