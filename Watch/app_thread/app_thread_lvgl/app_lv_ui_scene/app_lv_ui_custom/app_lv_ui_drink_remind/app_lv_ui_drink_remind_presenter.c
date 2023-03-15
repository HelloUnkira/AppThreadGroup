
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "app_lv_ui_drink_remind.h"
#include "app_lv_ui_drink_remind_presenter.h"

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_null(void)
{
}

app_lv_ui_drink_remind_presenter_t app_lv_ui_drink_remind_presenter = {
    .null = app_lv_ui_null,
};
