
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_stopwatch.h"

#include "app_lv_ui_stopwatch.h"
#include "app_lv_ui_stopwatch_presenter.h"

#define app_lv_ui_stopwatch_list_len    20
static uint8_t app_lv_ui_stopwatch_list_num = 0;
static app_lv_ui_stopwatch_t app_lv_ui_stopwatch_list[app_lv_ui_stopwatch_list_len] = {0};

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_tick_main(app_lv_ui_stopwatch_t *stopwatch)
{
   app_module_stopwatch_t stopwatch1 = {0};
    app_module_stopwatch_get(&stopwatch1);
    stopwatch->hour   = stopwatch1.hour;
    stopwatch->minute = stopwatch1.minute;
    stopwatch->second = stopwatch1.second;
    stopwatch->msec   = stopwatch1.msec;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_tick_list(app_lv_ui_stopwatch_t **stopwatch)
{
    *stopwatch = app_lv_ui_stopwatch_list;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_tick_num(void)
{
    return app_lv_ui_stopwatch_list_num;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_clean_tick_list(void)
{
    app_lv_ui_stopwatch_list_num = 0;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_refr_tick_list(void)
{
    app_lv_ui_stopwatch_t stopwatch2 = {0};
   app_module_stopwatch_t stopwatch1 = {0};
    app_module_stopwatch_get(&stopwatch1);
    stopwatch2.hour   = stopwatch1.hour;
    stopwatch2.minute = stopwatch1.minute;
    stopwatch2.second = stopwatch1.second;
    stopwatch2.msec   = stopwatch1.msec;
    /* 每次调用该函数生成一个新的tick并清除后一个 */
    for (int16_t idx = app_lv_ui_stopwatch_list_len - 1; idx > 0; idx--)
        app_lv_ui_stopwatch_list[idx] = app_lv_ui_stopwatch_list[idx - 1];
        app_lv_ui_stopwatch_list[0] = stopwatch2;
    if (app_lv_ui_stopwatch_list_num < app_lv_ui_stopwatch_list_len)
        app_lv_ui_stopwatch_list_num++;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_get_onoff(void)
{
   app_module_stopwatch_t stopwatch = {0};
    app_module_stopwatch_get(&stopwatch);
    return stopwatch.onoff;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_reset(void)
{
    app_module_stopwatch_reset();
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_start(void)
{
    app_module_stopwatch_start();
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_stop(void)
{
    app_module_stopwatch_stop();
}

app_lv_ui_stopwatch_presenter_t app_lv_ui_stopwatch_presenter = {
    .get_tick_main      = app_lv_ui_get_tick_main,
    .get_tick_list      = app_lv_ui_get_tick_list,
    .get_tick_num       = app_lv_ui_get_tick_num,
    .refr_tick_list     = app_lv_ui_refr_tick_list,
    .clean_tick_list    = app_lv_ui_clean_tick_list,
    .get_onoff          = app_lv_ui_get_onoff,
    .reset              = app_lv_ui_reset,
    .start              = app_lv_ui_start,
    .stop               = app_lv_ui_stop,
};
