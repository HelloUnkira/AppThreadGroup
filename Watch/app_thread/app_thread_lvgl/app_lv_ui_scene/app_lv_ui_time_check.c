/*
 *实现目的:
 *    lvgl超时回主界面以及息屏
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_timer.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_time_check.h"
#include "app_lv_ui_scene_set.h"
#include "app_lv_ui_watch.h"

static app_mutex_t app_lv_ui_scene_time_check_mutex = {0};
static app_module_timer_t app_lv_ui_scene_time_check_timer = {0};
static bool    app_lv_ui_scene_dlps = true;    /* 需要进入dlps */
static bool    app_lv_ui_scene_back = true;    /* 需要回退界面 */
static uint8_t app_lv_ui_scene_over_time = APP_LV_UI_SCENE_OVER_TIME;
static uint8_t app_lv_ui_scene_idle_time = APP_LV_UI_SCENE_IDLE_TIME;
static uint8_t app_lv_ui_scene_over_time_bak = APP_LV_UI_SCENE_OVER_TIME;
static uint8_t app_lv_ui_scene_idle_time_bak = APP_LV_UI_SCENE_IDLE_TIME;

/*@brief 重置界面状态
 */
void app_lv_ui_scene_time_check_reset(uint8_t over_time, uint8_t idle_time)
{
    APP_SYS_LOG_INFO("app_lv_ui_scene_time_check_reset\n");
    app_mutex_take(&app_lv_ui_scene_time_check_mutex);
    app_lv_ui_scene_over_time_bak = over_time != 0 ? over_time : app_lv_ui_scene_over_time_bak;
    app_lv_ui_scene_idle_time_bak = idle_time != 0 ? idle_time : app_lv_ui_scene_idle_time_bak;
    if (over_time == APP_LV_UI_SCENE_OVER_TIME_MAX ||
        idle_time == APP_LV_UI_SCENE_IDLE_TIME_MAX) {
        app_lv_ui_scene_over_time_bak = APP_LV_UI_SCENE_OVER_TIME_MAX;
        app_lv_ui_scene_idle_time_bak = APP_LV_UI_SCENE_IDLE_TIME_MAX;
    }
    app_lv_ui_scene_over_time = app_lv_ui_scene_over_time_bak;
    app_lv_ui_scene_idle_time = app_lv_ui_scene_idle_time_bak;
    app_lv_ui_scene_dlps = true;
    app_lv_ui_scene_back = true;
    app_mutex_give(&app_lv_ui_scene_time_check_mutex);
}

/*@brief 界面状态控制更新
 *       内部使用: 被lvgl线程使用
 */
static void app_lv_ui_scene_time_check_update(void *timer)
{
    app_mutex_take(&app_lv_ui_scene_time_check_mutex);
    /* 约减超时等待 */
    if (app_lv_ui_scene_over_time != 0 &&
        app_lv_ui_scene_over_time != APP_LV_UI_SCENE_OVER_TIME_MAX)
        app_lv_ui_scene_over_time--;
    /* 主界面超时等待结束发送休眠请求 */
    if (app_lv_ui_scene_over_time == 0) {
        if (app_lv_ui_scene_dlps) {
            app_lv_ui_scene_dlps = false;
            if (!app_module_system_dlps_get())
                 app_module_system_dlps_set(true);
        } else {
            /* 约减空闲等待 */
            if (app_lv_ui_scene_idle_time != 0 &&
                app_lv_ui_scene_idle_time != APP_LV_UI_SCENE_IDLE_TIME_MAX)
                app_lv_ui_scene_idle_time--;
            /* 空闲等待结束回到主界面 */
            if (app_lv_ui_scene_idle_time == 0) {
                if (app_lv_ui_scene_back) {
                    app_lv_ui_scene_back = false;
                    app_lv_scene_reset(&app_lv_scene_main, true);
                }
            }
        }
    }
    app_mutex_give(&app_lv_ui_scene_time_check_mutex);
}

/*@brief lvgl 时间检查更新初始化
 */
void app_lv_ui_scene_time_check_ready(void)
{
    app_mutex_process(&app_lv_ui_scene_time_check_mutex);
    app_lv_ui_scene_time_check_timer.expired = app_lv_ui_scene_time_check_update;
    app_lv_ui_scene_time_check_timer.peroid  = 1000;
    app_lv_ui_scene_time_check_timer.reload  = true;
}

/*@brief     界面状态检查功能启停
 *@param[in] status(true:启动界面状态检查,false:关闭界面状态检查)
 */
void app_lv_ui_scene_time_check_exec(bool status)
{
    if (status)
        app_module_timer_start(&app_lv_ui_scene_time_check_timer);
    else
        app_module_timer_stop(&app_lv_ui_scene_time_check_timer);
}