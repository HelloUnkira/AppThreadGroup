
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_measure(void)
{
    app_module_temperature_measure();
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_measure_status(void)
{
    return app_module_temperature_measure_status();
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_measure_result(void)
{
    return app_module_temperature_measure_result();
}

/*@brief lvgl ui数据交互回调
 */
static float app_lv_ui_func_local_get_value(void)
{
    return app_module_temperature_measure_value();
}

app_lv_ui_temperature_presenter_t app_lv_ui_temperature_presenter = {
    .measure        = app_lv_ui_func_local_measure,
    .measure_status = app_lv_ui_func_local_measure_status,
    .measure_result = app_lv_ui_func_local_measure_result,
    .get_value      = app_lv_ui_func_local_get_value,
};
