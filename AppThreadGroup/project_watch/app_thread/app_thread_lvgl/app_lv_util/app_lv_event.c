/*实现目标:
 *    lvgl事件
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief tick执行 事件包吸收
 */
static bool app_lv_tick_exec_package_absorb(void *pkg_old, void *pkg_new)
{
    app_thread_package_t *package_old = pkg_old;
    app_thread_package_t *package_new = pkg_new;
    
    /* 多的直接丢弃即可,这里为空 */
    return true;
}

/*@brief lvgl tick执行
 */
void app_lv_tick_exec_update(void)
{
    app_thread_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_sched,
        .event  = app_thread_lvgl_sched_exec,
        .absorb = app_lv_tick_exec_package_absorb,
    };
    app_thread_package_notify(&package);
}

/*@brief tick更新 事件包吸收
 */
static bool app_lv_tick_drv_package_absorb(void *pkg_old, void *pkg_new)
{
    app_thread_package_t *package_old = pkg_old;
    app_thread_package_t *package_new = pkg_new;
    
    /* 多的直接丢弃即可,这里为空 */
    return true;
}

/*@brief lvgl drv更新
 */
void app_lv_drv_update(void)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_drv,
        .priority = app_thread_package_priority_normal_above,
        .absorb   = app_lv_tick_drv_package_absorb,
    };
    app_thread_package_notify(&package);
}

/*@brief lvgl wheel更新
 *@param wheel 轮盘实例
 *@param size  轮盘实例大小
 */
void app_lv_wheel_update(void *wheel, uint32_t size)
{
    APP_SYS_ASSERT(size == sizeof(app_lv_wheel_t));
    void  *data = app_mem_alloc(size);
    memcpy(data, wheel, size);
    
    app_thread_package_t package = {
        .thread  = app_thread_id_lvgl,
        .module  = app_thread_lvgl_sched,
        .event   = app_thread_lvgl_sched_wheel,
        .dynamic = true,
        .data    = data,
        .size    = size,
    };
    app_thread_package_notify(&package);
}

/*@brief lvgl 场景启动
 */
void app_lv_scene_start(void)
{
    /* 向线程发送场景启动事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui,
        .event  = app_thread_lvgl_ui_scene_start,
    };
    app_thread_package_notify(&package);
}

/*@brief lvgl 场景停止
 */
void app_lv_scene_stop(void)
{
    /* 向线程发送场景停止事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui,
        .event  = app_thread_lvgl_ui_scene_stop,
    };
    app_thread_package_notify(&package);
}

/*@brief lvgl 场景低功耗
 */
void app_lv_scene_dlps(bool status)
{
    /* 向线程发送场景休眠唤醒事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_sched,
    };
    if (status)
        package.event = app_thread_lvgl_sched_dlps_enter;
    else
        package.event = app_thread_lvgl_sched_dlps_exit;
    app_thread_package_notify(&package);
}

/*@brief lvgl 场景停止
 */
void app_lv_scene_shutdown(void)
{
    /* 向线程发送场景停止事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui,
        .event  = app_thread_lvgl_ui_scene_shutdown,
    };
    app_thread_package_notify(&package);
}
