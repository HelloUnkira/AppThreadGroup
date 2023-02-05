
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_scene_set.h"

static void app_lv_ui_main_show(void *scene)
{
}

static void app_lv_ui_main_hide(void *scene)
{
}

app_lv_ui_scene_t app_lv_ui_scene_null = {
    /* 场景资源节点 */
    .presenter = NULL,
    .show = app_lv_ui_main_show,
    .hide = app_lv_ui_main_hide,
};

