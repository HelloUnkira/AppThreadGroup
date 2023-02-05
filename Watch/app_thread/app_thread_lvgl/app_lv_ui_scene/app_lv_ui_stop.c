
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_scene_set.h"

typedef struct {
    lv_style_t   style_scene;
    lv_anim_t    anim;
    lv_obj_t    *scene;
    lv_obj_t    *label;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    if (value <= 100) {
        uint8_t label_opa = (uint32_t)((float)(100 - value) * 2.55);
        lv_obj_set_style_text_opa(app_lv_ui_res_local->label, label_opa, 0);
    }
    if (value == 100) {
        app_lv_ui_scene_t scene = {0};
        app_lv_ui_scene_del(&scene);
    }
}

static void app_lv_ui_stop_show(void *scene)
{
    if (app_lv_ui_res_local == NULL)
        app_lv_ui_res_local  = app_mem_alloc(sizeof(app_lv_ui_res_local_t));
    if (app_lv_ui_res_local != NULL) {
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
        lv_label_set_text_static(app_lv_ui_res_local->label, "LVGL Watch Exit");
        lv_obj_center(app_lv_ui_res_local->label);
        /* 初始化显示动画 */
        lv_anim_init(&app_lv_ui_res_local->anim);
        lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
        lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
        lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, 0);
        lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 100);
        lv_anim_set_time(&app_lv_ui_res_local->anim, 3000);
        lv_anim_start(&app_lv_ui_res_local->anim);
    }
}

static void app_lv_ui_stop_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        lv_obj_del(app_lv_ui_res_local->scene);
    }
    if (app_lv_ui_res_local != NULL) {
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_ui_scene_t app_lv_ui_scene_stop = {
    /* 场景资源节点 */
    .presenter = NULL,
    .show = app_lv_ui_stop_show,
    .hide = app_lv_ui_stop_hide,
};

