
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene_set.h"

typedef struct {
    lv_style_t   style_scene;
    lv_obj_t    *scene;
    lv_obj_t    *label;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static void app_lv_ui_null_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = app_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化风格 */
        lv_style_init(&app_lv_ui_res_local->style_scene);
        lv_style_set_pad_all(&app_lv_ui_res_local->style_scene, 10);
        lv_style_set_border_width(&app_lv_ui_res_local->style_scene, 5);
        lv_style_set_align(&app_lv_ui_res_local->style_scene, LV_DIR_ALL);
        lv_style_set_bg_opa(&app_lv_ui_res_local->style_scene, LV_OPA_COVER);
        lv_style_set_bg_color(&app_lv_ui_res_local->style_scene, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_border_color(&app_lv_ui_res_local->style_scene, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_text_color(&app_lv_ui_res_local->style_scene, lv_palette_main(LV_PALETTE_BLUE));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = lv_obj_create(lv_scr_act());
        lv_obj_set_size(app_lv_ui_res_local->scene, LV_HOR_RES, LV_VER_RES);
        lv_obj_add_style(app_lv_ui_res_local->scene, &app_lv_ui_res_local->style_scene, 0);
        app_lv_ui_res_local->label = lv_label_create(app_lv_ui_res_local->scene);
        lv_label_set_long_mode(app_lv_ui_res_local->label, LV_LABEL_LONG_WRAP);
        lv_label_set_text_static(app_lv_ui_res_local->label, "NULL");
        lv_obj_center(app_lv_ui_res_local->label);
    }
    app_lv_scene_null.self = app_lv_ui_res_local == NULL ? NULL :
                             app_lv_ui_res_local->scene;
}

static void app_lv_ui_null_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_null.self = app_lv_ui_res_local == NULL ? NULL :
                             app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_null = {
    /* 场景资源节点 */
    .presenter = NULL,
    .show = app_lv_ui_null_show,
    .hide = app_lv_ui_null_hide,
};

