
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_event_object.h"
#include "app_lv_ui_event_scene.h"
#include "app_lv_ui_check_time.h"


/*@brief 测试指定场景
 */
void app_lv_ui_test_scene(void)
{
    /* 禁用超时回退 */
    app_lv_ui_check_time_reset(0, 0);
    app_lv_ui_check_time_exec(false);
    /* 做些什么:测试代码 */
    #if 0
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "S:test_find_phone.png");
    lv_obj_center(img);
    #else
    #endif
}