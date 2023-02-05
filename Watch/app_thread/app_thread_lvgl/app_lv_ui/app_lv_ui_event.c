/*
 *实现目的:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_pipe.h"
#include "app_sys_log.h"
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
    app_thread_package_notify(&package);
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
    app_thread_package_notify(&package);
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
    app_thread_package_notify(&package);
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
    app_thread_package_notify(&package);
}

