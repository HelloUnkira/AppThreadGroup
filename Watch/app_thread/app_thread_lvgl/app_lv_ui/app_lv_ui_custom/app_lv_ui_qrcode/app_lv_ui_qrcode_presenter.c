
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "app_lv_ui_qrcode.h"
#include "app_lv_ui_qrcode_presenter.h"

/*@brief lvgl ui数据交互回调
 */
static const char * app_lv_ui_qrcode_str(void)
{
    return "https://github.com/HelloUnkira";
}

app_lv_ui_qrcode_presenter_t app_lv_ui_qrcode_presenter = {
    .qrcode_str = app_lv_ui_qrcode_str,
};
