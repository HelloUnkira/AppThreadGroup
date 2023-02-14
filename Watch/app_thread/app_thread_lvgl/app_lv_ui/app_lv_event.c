/*
 *实现目的:
 *    lvgl事件
 */

#include "app_std_lib.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"

/*@brief lvgl tick更新
 */
void app_lv_tick_inc_update(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_lvgl,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_inc,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_package_notify(&package);
}

/*@brief lvgl tick执行
 */
void app_lv_tick_exec_update(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_lvgl,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_exec,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_package_notify(&package);
}

/*@brief lvgl drv更新
 */
void app_lv_drv_update(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_lvgl,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_drv,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_package_notify(&package);
}

/*@brief lvgl 场景更新
 */
void app_lv_scene_update(void *scene)
{
    app_package_t package = {
        .send_tid = app_thread_id_lvgl,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_scene,
        .dynamic  = false,
        .size     = 0,
        .data     = scene,
    };
    app_package_notify(&package);
}

/*@brief lvgl 场景状态更新
 */
void app_lv_scene_1s_update(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_lvgl,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_1s,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_package_notify(&package);
}

