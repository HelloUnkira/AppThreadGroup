

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief lvgl ui数据交互回调
 */
static const char * app_lv_ui_func_local_qrcode_str(void)
{
    return "https://github.com/HelloUnkira";
}

app_lv_ui_qrcode_presenter_t app_lv_ui_qrcode_presenter = {
    .qrcode_str = app_lv_ui_func_local_qrcode_str,
};
