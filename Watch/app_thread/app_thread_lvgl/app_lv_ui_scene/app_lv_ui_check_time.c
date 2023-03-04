/*
 *实现目的:
 *    lvgl超时回主界面以及息屏
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_mouse.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_check_time.h"

#include "app_lv_ui_scene.h"
#include "app_lv_ui_watch.h"

static app_mutex_t app_lv_ui_check_time_mutex = {0};
static app_sys_timer_t app_lv_ui_check_time_timer = {0};
static bool    app_lv_ui_dlps = true;    /* 需要进入dlps */
static bool    app_lv_ui_back = true;    /* 需要回退界面 */
static uint8_t app_lv_ui_over_time = APP_LV_UI_OVER_TIME;
static uint8_t app_lv_ui_idle_time = APP_LV_UI_IDLE_TIME;
static uint8_t app_lv_ui_over_time_bak = APP_LV_UI_OVER_TIME;
static uint8_t app_lv_ui_idle_time_bak = APP_LV_UI_IDLE_TIME;

/*@brief 重置界面状态
 */
void app_lv_ui_check_time_reset(uint8_t over_time, uint8_t idle_time)
{
    APP_SYS_LOG_INFO("");
    app_mutex_take(&app_lv_ui_check_time_mutex);
    app_lv_ui_over_time_bak = over_time != 0 ? over_time : app_lv_ui_over_time_bak;
    app_lv_ui_idle_time_bak = idle_time != 0 ? idle_time : app_lv_ui_idle_time_bak;
    if (over_time == APP_LV_UI_OVER_TIME_MAX ||
        idle_time == APP_LV_UI_IDLE_TIME_MAX) {
        app_lv_ui_over_time_bak = APP_LV_UI_OVER_TIME_MAX;
        app_lv_ui_idle_time_bak = APP_LV_UI_IDLE_TIME_MAX;
    }
    app_lv_ui_over_time = app_lv_ui_over_time_bak;
    app_lv_ui_idle_time = app_lv_ui_idle_time_bak;
    app_lv_ui_dlps = true;
    app_lv_ui_back = true;
    app_mutex_give(&app_lv_ui_check_time_mutex);
}

/*@brief 界面状态控制更新
 *       内部使用: 被lvgl线程使用
 */
void app_lv_ui_check_time_update(void)
{
    app_mutex_take(&app_lv_ui_check_time_mutex);
    /* 约减超时等待 */
    if (app_lv_ui_over_time != 0 &&
        app_lv_ui_over_time != APP_LV_UI_OVER_TIME_MAX)
        app_lv_ui_over_time--;
    /* 主界面超时等待结束发送休眠请求 */
    if (app_lv_ui_over_time == 0) {
        if (app_lv_ui_dlps) {
            app_lv_ui_dlps = false;
            if (!app_module_system_dlps_get())
                 app_module_system_dlps_set(true);
        } else {
            /* 约减空闲等待 */
            if (app_lv_ui_idle_time != 0 &&
                app_lv_ui_idle_time != APP_LV_UI_IDLE_TIME_MAX)
                app_lv_ui_idle_time--;
            /* 空闲等待结束回到主界面 */
            if (app_lv_ui_idle_time == 0) {
                if (app_lv_ui_back) {
                    app_lv_ui_back = false;
                    app_lv_scene_reset(&app_lv_scene_main, true);
                    /* 关闭鼠标 */
                    app_lv_mouse_dlps_enter();
                }
            }
        }
    }
    app_mutex_give(&app_lv_ui_check_time_mutex);
}


/*@brief 界面状态控制更新
 *       内部使用: 被lvgl线程使用
 */
static void app_lv_ui_check_time_timer_handler(void *timer)
{
    /* 发送场景计时检查事件 */
    app_package_t package = {
        .send_tid = app_thread_id_lvgl,
        .recv_tid = app_thread_id_lvgl,
        .module   = app_thread_lvgl_ui_scene,
        .event    = app_thread_lvgl_ui_scene_check_time,
    };
    app_package_notify(&package);
}

/*@brief lvgl 时间检查更新初始化
 */
void app_lv_ui_check_time_ready(void)
{
    app_mutex_process(&app_lv_ui_check_time_mutex);
    app_lv_ui_check_time_timer.expired = app_lv_ui_check_time_timer_handler;
    app_lv_ui_check_time_timer.peroid  = 1000;
    app_lv_ui_check_time_timer.reload  = true;
}

/*@brief     界面状态检查功能启停
 *@param[in] status(true:启动界面状态检查,false:关闭界面状态检查)
 */
void app_lv_ui_check_time_exec(bool status)
{
    if (status)
        app_sys_timer_start(&app_lv_ui_check_time_timer);
    else
        app_sys_timer_stop(&app_lv_ui_check_time_timer);
}
