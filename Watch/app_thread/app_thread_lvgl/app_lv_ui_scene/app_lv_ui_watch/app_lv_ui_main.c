
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_main.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *label;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    if (value <= 100) {
        uint8_t label_opa = (uint32_t)((float)value * 2.55);
        lv_obj_set_style_text_opa(app_lv_ui_res_local->label, label_opa, 0);
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_main_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        /* 初始化居中标签 */
        app_lv_ui_res_local->label = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text_static(app_lv_ui_res_local->label, "Watch Main");
        lv_obj_center(app_lv_ui_res_local->label);
        /* 初始化显示动画 */
        app_lv_ui_style_object_anim(app_lv_ui_res_local->scene,
                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    LV_ANIM_REPEAT_INFINITE, 0, 100, 1000);
    }
    app_lv_scene_main.self = app_lv_ui_res_local == NULL ? NULL :
                             app_lv_ui_res_local->scene;
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_main_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_main.self = app_lv_ui_res_local == NULL ? NULL :
                             app_lv_ui_res_local->scene;
}

app_lv_scene_set_t app_lv_scene_main_set = {
    .cross_vlist_t = &app_lv_scene_watch,
    .cross_vlist_b = &app_lv_scene_watch,
};

app_lv_scene_t app_lv_scene_main = {
    /* 场景资源节点 */
    .scene_set = &app_lv_scene_main_set,
    .show = app_lv_ui_main_show,
    .hide = app_lv_ui_main_hide,
};
