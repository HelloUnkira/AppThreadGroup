/*实现目标:
 *    数据解析层:
 *    ui与外界所有通信

 *    都通过此处的适配回调进行交互
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
/* 外部引用: */
#include "app_module_rtc.h"
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
static uint8_t scui_ui_func_local_get_mon(void)
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
static uint8_t scui_ui_func_local_get_min(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.minute;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_sec(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    return clock.second;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_get_msec(void)
{
    app_module_rtc_t rtc = {0};
    app_module_rtc_get(&rtc);
    return rtc.utc_ms;
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
    return scui_rand(max - min) + min;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_hr_passby(void)
{
    return scui_rand(60 * 24 * 7 + 1);
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
    return scui_rand(max - min) + min;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_spo2_passby(void)
{
    return scui_rand(60 * 24 * 7 + 1);
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_stress_min(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_stress_max(void)
{
    return 100;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_stress_cur(void)
{
    uint32_t min = scui_ui_func_local_get_stress_min();
    uint32_t max = scui_ui_func_local_get_stress_max();
    return scui_rand(max - min) + min;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_stress_passby(void)
{
    return scui_rand(60 * 24 * 7 + 1);
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
    return scui_rand(max - min) + min;
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
    return scui_rand(max - min) + min;
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
    return scui_rand(max - min) + min;
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
    return scui_rand(max - min) + min;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_temp_type(void)
{
    return SCUI_MULTI_LANG_0X009d;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_temp_icon(void)
{
    return scui_image_prj_image_src_02_weather_icon_weather_small_01_00_sunpng;
}

/*@brief scui ui数据交互回调
 */
static int16_t scui_ui_func_local_get_temp_min(void)
{
    return -25;
}

/*@brief scui ui数据交互回调
 */
static int16_t scui_ui_func_local_get_temp_max(void)
{
    return 40;
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
static uint32_t scui_ui_func_local_get_kcal_day7(uint32_t day7[7])
{
    for (uint8_t idx = 0; idx < 7; idx++)
        day7[idx] = scui_ui_func_local_get_kcal_cur();
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_kcal_day7_24(uint32_t day7_idx, uint32_t day7_24[24])
{
    for (uint8_t idx = 0; idx < 24; idx++)
        day7_24[idx] = scui_ui_func_local_get_kcal_cur();
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_step_day7(uint32_t day7[7])
{
    for (uint8_t idx = 0; idx < 7; idx++)
        day7[idx] = scui_ui_func_local_get_step_cur();
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_step_day7_24(uint32_t day7_idx, uint32_t day7_24[24])
{
    for (uint8_t idx = 0; idx < 24; idx++)
        day7_24[idx] = scui_ui_func_local_get_step_cur();
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_dist_day7(uint32_t day7[7])
{
    for (uint8_t idx = 0; idx < 7; idx++)
        day7[idx] = scui_ui_func_local_get_dist_cur();
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_get_dist_day7_24(uint32_t day7_idx, uint32_t day7_24[24])
{
    for (uint8_t idx = 0; idx < 24; idx++)
        day7_24[idx] = scui_ui_func_local_get_dist_cur();
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_alarm_none(void)
{
    return false;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_alarm_near_state(void)
{
    return true;
}

/*@brief scui ui数据交互回调
 */
static uint16_t scui_ui_func_local_alarm_near_date(void)
{
    // 0x1000: 今天; 0x2000: 明天; 0x8000: 单次
    // 低七位: 0-6 标记[日,一,二,三,四,五,六]
    
    return 0b1111110;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_alarm_near_hour(void)
{
    return 10;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_alarm_near_minute(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_sleep_none(void)
{
    return false;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_sleep_hour(void)
{
    return 10;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_sleep_minute(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static void scui_ui_func_local_sleep_pct4(uint8_t pct[4])
{
    pct[0] = 23; pct[1] = 17; pct[2] = 32; pct[3] = 28;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_sport_record_none(void)
{
    return false;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_sport_record_type(void)
{
    return SCUI_MULTI_LANG_0X0254;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_sport_record_icon(void)
{
    return scui_image_prj_image_src_10_sport_01_list_icon_big_01_000_huwaipaobupng;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_sport_record_hour(void)
{
    return 10;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_sport_record_minute(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_women_health_none(void)
{
    return false;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_women_health_type(void)
{
    return SCUI_MULTI_LANG_0X0113;
}

/*@brief scui ui数据交互回调
 */
static uint32_t scui_ui_func_local_women_health_icon(void)
{
    return scui_image_prj_image_src_19_widget_period_01_fertilebmp;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_women_health_hour(void)
{
    return 10;
}

/*@brief scui ui数据交互回调
 */
static uint8_t scui_ui_func_local_women_health_minute(void)
{
    return 0;
}

/*@brief scui ui数据交互回调
 */
static bool scui_ui_func_local_compass_invalid(void)
{
    return false;
}

/*@brief scui ui数据交互回调
 */
static uint16_t scui_ui_func_local_compass_angle(void)
{
    return scui_rand(360);
}

/*@brief scui ui数据交互回调
 */
static float scui_ui_func_local_altimeter_pressure(void)
{
    return 43.21f;
}

/*@brief scui ui数据交互回调
 */
static float scui_ui_func_local_altimeter_altitude(void)
{
    return 43.21f;
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
scui_presenter_t scui_presenter = {
    /* date time: */
    .is_am          = scui_ui_func_local_is_am,
    .is_pm          = scui_ui_func_local_is_pm,
    .is_24          = scui_ui_func_local_is_24,
    .get_utc        = scui_ui_func_local_get_utc,
    .get_year       = scui_ui_func_local_get_year,
    .get_mon        = scui_ui_func_local_get_mon,
    .get_day        = scui_ui_func_local_get_day,
    .get_hour       = scui_ui_func_local_get_hour,
    .get_min        = scui_ui_func_local_get_min,
    .get_sec        = scui_ui_func_local_get_sec,
    .get_msec       = scui_ui_func_local_get_msec,
    .get_week       = scui_ui_func_local_get_week,
    /* dev data: */
    .get_hr_min         = scui_ui_func_local_get_hr_min,
    .get_hr_max         = scui_ui_func_local_get_hr_max,
    .get_hr_cur         = scui_ui_func_local_get_hr_cur,
    .get_hr_passby      = scui_ui_func_local_get_hr_passby,
    .get_spo2_min       = scui_ui_func_local_get_spo2_min,
    .get_spo2_max       = scui_ui_func_local_get_spo2_max,
    .get_spo2_cur       = scui_ui_func_local_get_spo2_cur,
    .get_spo2_passby    = scui_ui_func_local_get_spo2_passby,
    .get_stress_min     = scui_ui_func_local_get_stress_min,
    .get_stress_max     = scui_ui_func_local_get_stress_max,
    .get_stress_cur     = scui_ui_func_local_get_stress_cur,
    .get_stress_passby  = scui_ui_func_local_get_stress_passby,
    .get_kcal_min       = scui_ui_func_local_get_kcal_min,
    .get_kcal_max       = scui_ui_func_local_get_kcal_max,
    .get_kcal_cur       = scui_ui_func_local_get_kcal_cur,
    .get_step_min       = scui_ui_func_local_get_step_min,
    .get_step_max       = scui_ui_func_local_get_step_max,
    .get_step_cur       = scui_ui_func_local_get_step_cur,
    .get_batt_min       = scui_ui_func_local_get_batt_min,
    .get_batt_max       = scui_ui_func_local_get_batt_max,
    .get_batt_cur       = scui_ui_func_local_get_batt_cur,
    .get_dist_min       = scui_ui_func_local_get_dist_min,
    .get_dist_max       = scui_ui_func_local_get_dist_max,
    .get_dist_cur       = scui_ui_func_local_get_dist_cur,
    .get_temp_type      = scui_ui_func_local_get_temp_type,
    .get_temp_icon      = scui_ui_func_local_get_temp_icon,
    .get_temp_min       = scui_ui_func_local_get_temp_min,
    .get_temp_max       = scui_ui_func_local_get_temp_max,
    .get_temp_cur       = scui_ui_func_local_get_temp_cur,
    .get_temp_unit      = scui_ui_func_local_get_temp_unit,
    /* history data */
    .get_kcal_day7      = scui_ui_func_local_get_kcal_day7,
    .get_kcal_day7_24   = scui_ui_func_local_get_kcal_day7_24,
    .get_step_day7      = scui_ui_func_local_get_step_day7,
    .get_step_day7_24   = scui_ui_func_local_get_step_day7_24,
    .get_dist_day7      = scui_ui_func_local_get_dist_day7,
    .get_dist_day7_24   = scui_ui_func_local_get_dist_day7_24,
    /* dev data: */
    .alarm_none             = scui_ui_func_local_alarm_none,
    .alarm_near_state       = scui_ui_func_local_alarm_near_state,
    .alarm_near_date        = scui_ui_func_local_alarm_near_date,
    .alarm_near_hour        = scui_ui_func_local_alarm_near_hour,
    .alarm_near_minute      = scui_ui_func_local_alarm_near_minute,
    .sleep_none             = scui_ui_func_local_sleep_none,
    .sleep_hour             = scui_ui_func_local_sleep_hour,
    .sleep_minute           = scui_ui_func_local_sleep_minute,
    .sleep_pct4             = scui_ui_func_local_sleep_pct4,
    .sport_record_none      = scui_ui_func_local_sport_record_none,
    .sport_record_type      = scui_ui_func_local_sport_record_type,
    .sport_record_icon      = scui_ui_func_local_sport_record_icon,
    .sport_record_hour      = scui_ui_func_local_sport_record_hour,
    .sport_record_minute    = scui_ui_func_local_sport_record_minute,
    .women_health_none      = scui_ui_func_local_women_health_none,
    .women_health_type      = scui_ui_func_local_women_health_type,
    .women_health_icon      = scui_ui_func_local_women_health_icon,
    .women_health_hour      = scui_ui_func_local_women_health_hour,
    .women_health_minute    = scui_ui_func_local_women_health_minute,
    .compass_invalid        = scui_ui_func_local_compass_invalid,
    .compass_angle          = scui_ui_func_local_compass_angle,
    .altimeter_pressure     = scui_ui_func_local_altimeter_pressure,
    .altimeter_altitude     = scui_ui_func_local_altimeter_altitude,
    
    /* drv func: */
    .vibrate_shot   = scui_ui_func_local_vibrate_shot,
    
    /* keep adding... */
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
