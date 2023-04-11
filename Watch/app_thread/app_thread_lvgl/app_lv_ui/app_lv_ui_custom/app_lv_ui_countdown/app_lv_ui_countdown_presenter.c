
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_module_countdown.h"
#include "app_module_vibrate.h"

#include "app_lv_ui_check_time.h"
#include "app_lv_ui_countdown.h"
#include "app_lv_ui_countdown_presenter.h"

static app_lv_ui_countdown_t app_lv_ui_countdown = {0};

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

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_remind_start(void)
{
    if (true/* 重设超时息屏事件 */)
        app_lv_ui_check_time_reset(15, APP_LV_UI_IDLE_TIME_MAX);
    if (true/* 开启震动 */) {
        app_module_vibrate_t vibrate = {
            .melody = app_module_vibrate_melody_default_2,
            .period = 1000,
            .repeat = 15,
        };
        /* 可以先获取震动状态考虑是否需要打断,这里默认打断 */
        app_module_vibrate_stop();
        app_module_vibrate_set(&vibrate);
        app_module_vibrate_start();
    }
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_remind_end(void)
{
    if (true/* 重设超时息屏事件 */)
        app_lv_ui_check_time_reset(0, 0);
    if (true/* 关闭震动 */) {
        app_module_vibrate_stop();
    }
}

app_lv_ui_countdown_presenter_t app_lv_ui_countdown_presenter = {
    .get_tick       = app_lv_ui_get_tick,
    .get_onoff      = app_lv_ui_get_onoff,
    .set            = app_lv_ui_set,
    .get            = app_lv_ui_get,
    .reset          = app_lv_ui_reset,
    .start          = app_lv_ui_start,
    .stop           = app_lv_ui_stop,
    .remind_start   = app_lv_ui_remind_start,
    .remind_end     = app_lv_ui_remind_end,
};
