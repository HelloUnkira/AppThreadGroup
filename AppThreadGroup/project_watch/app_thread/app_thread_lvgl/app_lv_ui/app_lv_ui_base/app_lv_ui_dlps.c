
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_obj_t *scene;
} *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
}

/*@brief 界面显示
 *@param scene 场景
 */
static void app_lv_ui_dlps_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
    }
}

/*@brief 界面隐藏
 *@param scene 场景
 */
static void app_lv_ui_dlps_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_wheel_t app_lv_ui_dlps_wheel = {
    .self = &app_lv_ui_dlps,
};

app_lv_scene_t app_lv_ui_dlps = {
    /* 场景资源节点 */
    .show  =  app_lv_ui_dlps_show,
    .hide  =  app_lv_ui_dlps_hide,
    .wheel = &app_lv_ui_dlps_wheel,
};
