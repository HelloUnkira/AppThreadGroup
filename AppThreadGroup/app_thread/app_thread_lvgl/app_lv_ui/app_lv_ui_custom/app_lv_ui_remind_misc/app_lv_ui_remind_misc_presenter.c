
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_none(void)
{
}

app_lv_ui_remind_misc_presenter_t app_lv_ui_remind_misc_presenter = {
    .none = app_lv_ui_func_local_none,
};
