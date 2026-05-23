

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static char app_lv_ui_log_text_str[128 + 1] = {0};

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
    memset(app_lv_ui_log_text_str, '\0', sizeof(app_lv_ui_log_text_str));
    if (!app_sys_log_text_peek(app_lv_ui_log_text_str))
        LV_LOG_WARN("can not take trace text");
        LV_LOG_INFO(app_lv_ui_log_text_str);
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
