
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_check_time.h"
#include "app_lv_ui_developer_mode.h"

/*@brief 开发者模式
 *       绕过一部分调度机制更方便UI的开发
 */
#if APP_LV_UI_DEVELOPER_MODE
void app_lv_ui_developer_mode(void)
{
    /* 做些什么... */
    /* 例如: app_lv_scene_add(&app_lv_scene_xxx); */
}
#endif
