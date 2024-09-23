/*实现目标:
 *    数据解析层:
 *    ui与外界所有通信

 *    都通过此处的适配回调进行交互
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
/* 外部引用: */
#include "app_module_clock.h"

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_is_am(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_am;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_is_pm(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_pm;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_is_24(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.is_24;
}

/*@brief scui ui数据交互回调
 */
static uint64_t scui_ui_func_local_get_utc(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.utc;
}

/*@brief scui ui数据交互回调
 */
static uint16_t scui_ui_func_local_get_year(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.year;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_month(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.month;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_day(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.day;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_hour(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.hour;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_minute(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.minute;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_second(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.second;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_week(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.week;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* scui 唯一交互实例回调集合 */
scui_ui_presenter_t scui_ui_presenter = {
    .is_am          = scui_ui_func_local_is_am,
    .is_pm          = scui_ui_func_local_is_pm,
    .is_24          = scui_ui_func_local_is_24,
    .get_utc        = scui_ui_func_local_get_utc,
    .get_year       = scui_ui_func_local_get_year,
    .get_month      = scui_ui_func_local_get_month,
    .get_day        = scui_ui_func_local_get_day,
    .get_hour       = scui_ui_func_local_get_hour,
    .get_minute     = scui_ui_func_local_get_minute,
    .get_second     = scui_ui_func_local_get_second,
    .get_week       = scui_ui_func_local_get_week,
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/