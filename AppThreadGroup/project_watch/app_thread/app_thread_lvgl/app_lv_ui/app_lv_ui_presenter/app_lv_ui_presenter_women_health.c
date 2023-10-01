
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_is_valid(void)
{
    #if APP_LV_DEVELOPER_MODEL
    return true;
    #else
    #endif
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_get_dtime(uint16_t *year, uint8_t *month)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    *year  = clock.year;
    *month = clock.month;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_get_menstrual(uint8_t *day_s, uint8_t *day_e)
{
    #if APP_LV_DEVELOPER_MODEL
    *day_s = 7;
    *day_e = 13;
    #else
    #endif
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_get_pregnancy(uint8_t *day_s, uint8_t *day_e)
{
    #if APP_LV_DEVELOPER_MODEL
    *day_s = 17;
    *day_e = 23;
    #else
    #endif
}

app_lv_ui_presenter_women_health_t app_lv_ui_presenter_women_health = {
    .is_valid           = app_lv_ui_func_local_is_valid,
    .get_dtime          = app_lv_ui_func_local_get_dtime,
    .get_menstrual      = app_lv_ui_func_local_get_menstrual,
    .get_pregnancy      = app_lv_ui_func_local_get_pregnancy,
};
