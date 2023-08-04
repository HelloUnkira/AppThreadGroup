
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_check_time.h"
#include "app_lv_event_ui.h"
#include "app_lv_style.h"
#include "app_lv_ui_scene.h"

/*@brief 测试指定场景
 */
void app_lv_ui_test_scene(void)
{
    /* 禁用超时回退 */
    app_lv_check_time_reset(0, 0);
    app_lv_check_time_exec(false);
    /* 做些什么:测试代码 */
    #if 0
    #elif 0
    static bool only_one = true;
    if (only_one) {
        only_one = false;
        lv_obj_t *img = lv_img_create(lv_scr_act());
        lv_img_set_src(img, "S:lvgl_ext_src/LVGL.png");
        lv_obj_center(img);
    }
    #else
    #endif
}
