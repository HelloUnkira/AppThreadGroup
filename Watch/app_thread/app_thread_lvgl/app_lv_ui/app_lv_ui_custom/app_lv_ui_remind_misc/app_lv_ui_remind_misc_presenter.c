
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_module_vibrate.h"

#include "app_lv_ui_check_time.h"
#include "app_lv_ui_remind_misc_presenter.h"

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_remind_custom_start(void)
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
static void app_lv_ui_remind_custom_end(void)
{
    if (true/* 重设超时息屏事件 */)
        app_lv_ui_check_time_reset(0, 0);
    if (true/* 关闭震动 */) {
        app_module_vibrate_stop();
    }
}

app_lv_ui_remind_misc_presenter_t app_lv_ui_remind_misc_presenter = {
    .remind_custom_start = app_lv_ui_remind_custom_start,
    .remind_custom_end   = app_lv_ui_remind_custom_end,
};
