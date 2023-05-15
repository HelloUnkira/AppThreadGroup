/*
 *实现目的:
 *    lvgl事件
 */

#include "app_ext_lib.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"

#include "lvgl.h"
#include "app_lv_event.h"

/*@brief lvgl tick更新
 */
void app_lv_tick_inc_update(void)
{
    app_package_t package = {
        .thread   = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_inc,
        .priority = app_package_priority_highest,
    };
    app_package_notify(&package);
}

/*@brief lvgl tick执行
 */
void app_lv_tick_exec_update(void)
{
    app_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_sched,
        .event  = app_thread_lvgl_sched_exec,
    };
    app_package_notify(&package);
}

/*@brief lvgl drv更新
 */
void app_lv_drv_update(void)
{
    app_package_t package = {
        .thread   = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_drv,
        .priority = app_package_priority_normal_above,
    };
    app_package_notify(&package);
}

/*@brief lvgl 场景更新
 */
void app_lv_scene_update(void *scene)
{
    app_package_t package = {
        .thread   = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_scene,
        .priority = app_package_priority_normal,
        .dynamic  = false,
        .size     = sizeof(void *),
        .data     = scene,
    };
    app_package_notify(&package);
}

/*@brief lvgl 场景启动
 */
void app_lv_scene_start(void)
{
    /* 向线程发送场景启动事件 */
    static bool not_run_yet = true;
    if (not_run_yet) {
        app_package_t package = {
            .thread = app_thread_id_lvgl,
            .module = app_thread_lvgl_ui_scene,
            .event  = app_thread_lvgl_ui_scene_start,
        };
        app_package_notify(&package);
        not_run_yet = false;
    }
}

/*@brief lvgl 场景启动
 */
void app_lv_scene_stop(void)
{
    /* 向线程发送场景停止事件 */
    static bool not_run_yet = true;
    if (not_run_yet) {
        app_package_t package = {
            .thread = app_thread_id_lvgl,
            .module = app_thread_lvgl_ui_scene,
            .event  = app_thread_lvgl_ui_scene_stop,
        };
        app_package_notify(&package);
        not_run_yet = false;
    }
}

/*@brief lvgl 场景启动
 */
void app_lv_scene_dlps(bool status)
{
    /* 向线程发送场景休眠唤醒事件 */
    app_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_sched,
    };
    if (status)
        package.event = app_thread_lvgl_sched_dlps_enter;
    else
        package.event = app_thread_lvgl_sched_dlps_exit;
    app_package_notify(&package);
}

