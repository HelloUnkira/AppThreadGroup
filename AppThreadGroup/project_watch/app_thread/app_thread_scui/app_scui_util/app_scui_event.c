/*实现目标:
 *    scui事件
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_scui_lib.h"

/*@brief tick执行 事件包吸收
 */
static bool app_scui_tick_exec_package_absorb(void *pkg_old, void *pkg_new)
{
    app_thread_package_t *package_old = pkg_old;
    app_thread_package_t *package_new = pkg_new;
    
    /* 多的直接丢弃即可,这里为空 */
    return true;
}

/*@brief scui tick执行
 */
void app_scui_tick_exec_update(void)
{
    app_thread_package_t package = {
        .thread = app_thread_id_scui,
        .module = app_thread_scui_sched,
        .event  = app_thread_scui_sched_exec,
        .absorb = app_scui_tick_exec_package_absorb,
    };
    app_thread_package_notify(&package);
}

/*@brief tick更新 事件包吸收
 */
static bool app_scui_tick_drv_package_absorb(void *pkg_old, void *pkg_new)
{
    app_thread_package_t *package_old = pkg_old;
    app_thread_package_t *package_new = pkg_new;
    
    /* 多的直接丢弃即可,这里为空 */
    return true;
}

/*@brief scui drv更新
 */
void app_scui_drv_update(void)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_scui,
        .module   = app_thread_scui_sched,
        .event    = app_thread_scui_sched_drv,
        .priority = app_thread_package_priority_normal_above,
        .absorb   = app_scui_tick_drv_package_absorb,
    };
    app_thread_package_notify(&package);
}

/*@brief scui 场景启动
 */
void app_scui_scene_start(void)
{
    /* 向线程发送场景启动事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_scui,
        .module = app_thread_scui_ui,
        .event  = app_thread_scui_ui_scene_start,
    };
    app_thread_package_notify(&package);
}

/*@brief scui 场景停止
 */
void app_scui_scene_stop(void)
{
    /* 向线程发送场景停止事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_scui,
        .module = app_thread_scui_ui,
        .event  = app_thread_scui_ui_scene_stop,
    };
    app_thread_package_notify(&package);
}

/*@brief scui 场景低功耗
 */
void app_scui_scene_dlps(bool status)
{
    /* 向线程发送场景休眠唤醒事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_scui,
        .module = app_thread_scui_sched,
    };
    if (status)
        package.event = app_thread_scui_sched_dlps_enter;
    else
        package.event = app_thread_scui_sched_dlps_exit;
    app_thread_package_notify(&package);
}

/*@brief scui 场景停止
 */
void app_scui_scene_shutdown(void)
{
    /* 向线程发送场景停止事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_scui,
        .module = app_thread_scui_ui,
        .event  = app_thread_scui_ui_scene_shutdown,
    };
    app_thread_package_notify(&package);
}
