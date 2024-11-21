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
#include "app_module_vibrate.h"

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
    app_module_clock_set_system_clock_mode(false);
    
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

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_hr_min(void)
{
    return 60;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_hr_max(void)
{
    return 140;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_hr_cur(void)
{
    uint32_t min = scui_ui_func_local_get_hr_min();
    uint32_t max = scui_ui_func_local_get_hr_max();
    return scui_rand(min, max);
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_spo2_min(void)
{
    return 92;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_spo2_max(void)
{
    return 100;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_spo2_cur(void)
{
    uint32_t min = scui_ui_func_local_get_spo2_min();
    uint32_t max = scui_ui_func_local_get_spo2_max();
    return scui_rand(min, max);
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_kcal_min(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_kcal_max(void)
{
    return 9999;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_kcal_cur(void)
{
    uint32_t min = scui_ui_func_local_get_kcal_min();
    uint32_t max = scui_ui_func_local_get_kcal_max();
    return scui_rand(min, max);
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_step_min(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_step_max(void)
{
    return 99999;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_step_cur(void)
{
    uint32_t min = scui_ui_func_local_get_step_min();
    uint32_t max = scui_ui_func_local_get_step_max();
    return scui_rand(min, max);
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_batt_min(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_batt_max(void)
{
    return 100;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_batt_cur(void)
{
    uint32_t min = scui_ui_func_local_get_batt_min();
    uint32_t max = scui_ui_func_local_get_batt_max();
    return scui_rand(min, max);
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_dist_min(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_dist_max(void)
{
    return 9999;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_dist_cur(void)
{
    uint32_t min = scui_ui_func_local_get_dist_min();
    uint32_t max = scui_ui_func_local_get_dist_max();
    return scui_rand(min, max);
}

/*@brief scui ui数据交互回调
 */
static int16_t scui_ui_func_local_get_temp_cur(void)
{
    return 25;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_get_temp_unit(void)
{
    return false;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_vibrate_shot(void)
{
    SCUI_LOG_INFO("vibrate_shot");
    
    app_module_vibrate_t vibrate = {
        .melody  = app_module_vibrate_melody_default_2,
        .pwm_max = 100,
        .pwm_min = 0,
        .period  = 75,
        .repeat  = 0,
    };
    /* 可以先获取震动状态考虑是否需要打断,这里默认打断 */
    app_module_vibrate_stop();
    app_module_vibrate_set(&vibrate);
    app_module_vibrate_start();
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* scui 唯一交互实例回调集合 */
scui_ui_presenter_t scui_ui_presenter = {
    /* date time: */
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
    /* dev data: */
    .get_hr_min     = scui_ui_func_local_get_hr_min,
    .get_hr_max     = scui_ui_func_local_get_hr_max,
    .get_hr_cur     = scui_ui_func_local_get_hr_cur,
    .get_spo2_min   = scui_ui_func_local_get_spo2_min,
    .get_spo2_max   = scui_ui_func_local_get_spo2_max,
    .get_spo2_cur   = scui_ui_func_local_get_spo2_cur,
    .get_kcal_min   = scui_ui_func_local_get_kcal_min,
    .get_kcal_max   = scui_ui_func_local_get_kcal_max,
    .get_kcal_cur   = scui_ui_func_local_get_kcal_cur,
    .get_step_min   = scui_ui_func_local_get_step_min,
    .get_step_max   = scui_ui_func_local_get_step_max,
    .get_step_cur   = scui_ui_func_local_get_step_cur,
    .get_batt_min   = scui_ui_func_local_get_batt_min,
    .get_batt_max   = scui_ui_func_local_get_batt_max,
    .get_batt_cur   = scui_ui_func_local_get_batt_cur,
    .get_dist_min   = scui_ui_func_local_get_dist_min,
    .get_dist_max   = scui_ui_func_local_get_dist_max,
    .get_dist_cur   = scui_ui_func_local_get_dist_cur,
    .get_temp_cur   = scui_ui_func_local_get_temp_cur,
    .get_temp_unit  = scui_ui_func_local_get_temp_unit,
    /* drv func: */
    .vibrate_shot   = scui_ui_func_local_vibrate_shot,
    
    /* keep adding... */
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
