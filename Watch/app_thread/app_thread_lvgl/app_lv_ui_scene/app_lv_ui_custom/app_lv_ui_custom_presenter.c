
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "app_lv_ui_custom_presenter.h"

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

app_lv_ui_custom_presenter_t app_lv_ui_custom_presenter = {
    .is_am      = app_lv_ui_is_am,
    .is_pm      = app_lv_ui_is_pm,
    .is_24_mode = app_lv_ui_is_24_mode,
};
