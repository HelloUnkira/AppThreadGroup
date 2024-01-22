
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static char app_lv_ui_log_text_str[APP_SYS_LOG_TEXT_LIMIT + 1] = {0};

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_peek_reset(void)
{
    app_sys_log_text_peek_reset();
}

/*@brief lvgl ui数据交互回调
 */
static const char * app_lv_ui_func_local_peek_one(void)
{
    memset(app_lv_ui_log_text_str, '\0', APP_SYS_LOG_TEXT_LIMIT + 1);
    if (!app_sys_log_text_peek(app_lv_ui_log_text_str))
        APP_SYS_LOG_WARN("can not take trace text");
        APP_SYS_LOG_INFO(app_lv_ui_log_text_str);
    return app_lv_ui_log_text_str;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_reset(void)
{
    app_sys_log_text_reset();
}

app_lv_ui_log_text_presenter_t app_lv_ui_log_text_presenter = {
    .peek_reset = app_lv_ui_func_local_peek_reset,
    .peek_one   = app_lv_ui_func_local_peek_one,
    .reset      = app_lv_ui_func_local_reset,
};
