
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_module_trace.h"

#include "app_lv_ui_trace_text.h"
#include "app_lv_ui_trace_text_presenter.h"

static char app_lv_ui_trace_text_text[APP_MODULE_TRACE_LOG_MAX + 1] = {0};

/*@brief lvgl ui数据交互回调
 */
static const char * app_lv_ui_peek_one(void)
{
    memset(app_lv_ui_trace_text_text, '\0', APP_MODULE_TRACE_LOG_MAX + 1);
    if (!app_module_trace_text_peek(app_lv_ui_trace_text_text))
        APP_SYS_LOG_WARN("can not take trace text");
        APP_SYS_LOG_INFO(app_lv_ui_trace_text_text);
    return app_lv_ui_trace_text_text;
}

/*@brief lvgl ui数据交互回调
 */
static const char * app_lv_ui_reset(void)
{
    app_module_trace_text_reset();
}

app_lv_ui_trace_text_presenter_t app_lv_ui_trace_text_presenter = {
    .peek_one = app_lv_ui_peek_one,
    .reset    = app_lv_ui_reset,
};
