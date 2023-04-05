
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_clock.h"

#include "app_lv_ui_clock.h"
#include "app_lv_ui_clock_presenter.h"

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_is_am(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_am;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_is_pm(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_pm;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_is_24(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_24;
}

/*@brief lvgl ui数据交互回调
 */
static uint64_t app_lv_ui_get_utc(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.utc;
}

/*@brief lvgl ui数据交互回调
 */
static uint16_t app_lv_ui_get_year(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.year;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_month(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.month;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_day(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.day;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_hour(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.hour;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_minute(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.minute;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_second(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.second;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_week(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.week;
}

app_lv_ui_clock_presenter_t app_lv_ui_clock_presenter = {
    .is_am          = app_lv_ui_is_am,
    .is_pm          = app_lv_ui_is_pm,
    .is_24          = app_lv_ui_is_24,
    .get_utc        = app_lv_ui_get_utc,
    .get_year       = app_lv_ui_get_year,
    .get_month      = app_lv_ui_get_month,
    .get_day        = app_lv_ui_get_day,
    .get_hour       = app_lv_ui_get_hour,
    .get_minute     = app_lv_ui_get_minute,
    .get_second     = app_lv_ui_get_second,
    .get_week       = app_lv_ui_get_week,
};
