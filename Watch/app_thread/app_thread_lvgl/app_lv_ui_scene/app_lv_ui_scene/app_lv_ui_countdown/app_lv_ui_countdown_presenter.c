
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_clock.h"
#include "app_module_countdown.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_!util.h"
#include "app_lv_ui_scene_set.h"
#include "app_lv_ui_countdown.h"
#include "app_lv_ui_countdown_presenter.h"

app_lv_ui_countdown_t app_lv_ui_countdown = {0};

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_is_am(void)
{
    return false;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_is_pm(void)
{
    return false;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_is_24_mode(void)
{
    return true;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_user_hour(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.hour;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_user_minute(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.minute;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_tick(app_lv_ui_countdown_t *countdown)
{
   app_module_countdown_t countdown1 = {0};
    app_module_countdown_get(&countdown1);
    countdown->hour   = countdown1.hour;
    countdown->minute = countdown1.minute;
    countdown->second = countdown1.second;
    countdown->msec   = countdown1.msec;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_get_onoff(void)
{
   app_module_countdown_t countdown = {0};
    app_module_countdown_get(&countdown);
    return countdown.onoff;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_set(app_lv_ui_countdown_t *countdown)
{
    app_lv_ui_countdown = *countdown;
   app_module_countdown_t countdown1 = {0};
    countdown1.hour   = app_lv_ui_countdown.hour;
    countdown1.second = app_lv_ui_countdown.second;
    countdown1.minute = app_lv_ui_countdown.minute;
    countdown1.msec   = app_lv_ui_countdown.msec;
    app_module_countdown_set(&countdown1);
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get(app_lv_ui_countdown_t *countdown)
{
    *countdown = app_lv_ui_countdown;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_reset(void)
{
    app_module_countdown_reset();
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_start(void)
{
    app_module_countdown_start();
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_stop(void)
{
    app_module_countdown_stop();
}

app_lv_ui_countdown_presenter_t app_lv_ui_countdown_presenter = {
    .is_am              = app_lv_ui_is_am,
    .is_pm              = app_lv_ui_is_pm,
    .is_24_mode         = app_lv_ui_is_24_mode,
    .get_user_hour      = app_lv_ui_get_user_hour,
    .get_user_minute    = app_lv_ui_get_user_minute,
    .get_tick           = app_lv_ui_get_tick,
    .get_onoff          = app_lv_ui_get_onoff,
    .set                = app_lv_ui_set,
    .get                = app_lv_ui_get,
    .reset              = app_lv_ui_reset,
    .start              = app_lv_ui_start,
    .stop               = app_lv_ui_stop,
};


