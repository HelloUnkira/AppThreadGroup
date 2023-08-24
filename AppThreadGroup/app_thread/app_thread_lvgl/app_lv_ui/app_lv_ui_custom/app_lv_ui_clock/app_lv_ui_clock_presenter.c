
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_is_am(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_am;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_is_pm(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_pm;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_is_24(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_24;
}

/*@brief lvgl ui数据交互回调
 */
static uint64_t app_lv_ui_func_local_get_utc(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.utc;
}

/*@brief lvgl ui数据交互回调
 */
static uint16_t app_lv_ui_func_local_get_year(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.year;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_get_month(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.month;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_get_day(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.day;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_get_hour(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.hour;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_get_minute(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.minute;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_get_second(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.second;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_get_week(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.week;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_func_local_format_clock_1(char str[20])
{
    sprintf(str, "%.2u:%.2u%s",
                  app_lv_ui_func_local_get_hour(),
                  app_lv_ui_func_local_get_minute(),
                  app_lv_ui_func_local_is_24() ? "" :
                  app_lv_ui_func_local_is_am() ? " AM" :
                  app_lv_ui_func_local_is_pm() ? " PM" : "");
}

app_lv_ui_clock_presenter_t app_lv_ui_clock_presenter = {
    .is_am          = app_lv_ui_func_local_is_am,
    .is_pm          = app_lv_ui_func_local_is_pm,
    .is_24          = app_lv_ui_func_local_is_24,
    .get_utc        = app_lv_ui_func_local_get_utc,
    .get_year       = app_lv_ui_func_local_get_year,
    .get_month      = app_lv_ui_func_local_get_month,
    .get_day        = app_lv_ui_func_local_get_day,
    .get_hour       = app_lv_ui_func_local_get_hour,
    .get_minute     = app_lv_ui_func_local_get_minute,
    .get_second     = app_lv_ui_func_local_get_second,
    .get_week       = app_lv_ui_func_local_get_week,
    /* extern */
    .format_clock_1 = app_lv_ui_func_local_format_clock_1,
};
