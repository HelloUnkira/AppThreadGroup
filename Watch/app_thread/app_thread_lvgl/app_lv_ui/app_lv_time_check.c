/*
 *实现目的:
 *    lvgl超时回主界面以及息屏
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"

#include "app_sys_pipe.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_time_check.h"
#include "app_lv_ui_!util.h"
#include "app_lv_ui_scene_set.h"

static bool    app_lv_scene_time_status = true;
static uint8_t app_lv_scene_idle_time = APP_LV_SCENE_IDLE_TIME;
static uint8_t app_lv_scene_over_time = APP_LV_SCENE_OVER_TIME;
static uint8_t app_lv_scene_idle_time_bak = APP_LV_SCENE_IDLE_TIME;
static uint8_t app_lv_scene_over_time_bak = APP_LV_SCENE_OVER_TIME;

/*@brief     界面状态检查功能启停
 *@param[in] status(true:启动界面状态检查,false:关闭界面状态检查)
 */
void app_lv_scene_time_check_status(bool status)
{
    app_lv_scene_time_status = status;
}

/*@brief 重置界面状态
 */
void app_lv_scene_time_check_reset(void)
{
    app_lv_scene_idle_time = app_lv_scene_idle_time_bak;
    app_lv_scene_over_time = app_lv_scene_over_time_bak;
}

/*@brief 重置界面状态
 */
void app_lv_scene_time_check_idle_time_update(uint8_t time)
{
    app_lv_scene_idle_time = time;
    app_lv_scene_idle_time_bak = time;
}

/*@brief 重置界面状态
 */
void app_lv_scene_time_check_over_time_update(uint8_t time)
{
    app_lv_scene_over_time = time;
    app_lv_scene_over_time_bak = time;
}

/*@brief 界面状态控制更新
 *       内部使用: 被lvgl线程使用
 */
void app_lv_scene_time_check_update(void)
{
    if (app_module_system_dlps_get())
        return;
    if (!app_lv_scene_time_status)
        return;
    /* 约减空闲等待 */
    if (app_lv_scene_idle_time != APP_LV_SCENE_IDLE_TIME_MAX)
    if (app_lv_scene_idle_time != 0)
        app_lv_scene_idle_time--;
    /* 约减超时等待 */
    if (app_lv_scene_over_time != APP_LV_SCENE_OVER_TIME_MAX)
    if (app_lv_scene_over_time != 0)
        app_lv_scene_over_time--;
    /* 空闲等待结束回到主界面 */
    if (app_lv_scene_idle_time == 0) {
        app_lv_scene_idle_time  = app_lv_scene_idle_time_bak;
        if (app_lv_scene_get_nest() > 1)
            app_lv_scene_reset(&app_lv_scene_main);
    }
    /* 主界面超时等待结束发送休眠请求 */
    if (app_lv_scene_over_time == 0) {
        app_lv_scene_over_time  = app_lv_scene_over_time_bak;
        app_module_system_dlps_set(true);
    }
}
