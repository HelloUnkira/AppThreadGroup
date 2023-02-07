
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
    lv_style_t   style_bar_e;
    lv_style_t   style_bar_i;
    lv_anim_t    anim;
    lv_obj_t    *scene;
    lv_obj_t    *label;
    lv_obj_t    *bar;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    if (value <= 100) {
        uint8_t bar_value = (value);
        uint8_t label_opa = (uint32_t)((float)value * 2.55);
        lv_obj_set_style_text_opa(app_lv_ui_res_local->label, label_opa, 0);
        lv_bar_set_value(app_lv_ui_res_local->bar, bar_value, LV_ANIM_OFF);
    }
    if (value == 100) {
        app_lv_scene_t scene = {0};
        app_lv_scene_del(&scene);
    }
}

static void app_lv_ui_start_show(void *scene)
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
        lv_style_init(&app_lv_ui_res_local->style_bar_e);
        lv_style_set_pad_all(&app_lv_ui_res_local->style_bar_e, 4);
        lv_style_set_border_width(&app_lv_ui_res_local->style_bar_e, 1);
        lv_style_set_bg_opa(&app_lv_ui_res_local->style_bar_e, LV_OPA_COVER);
        lv_style_set_bg_color(&app_lv_ui_res_local->style_bar_e, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_border_color(&app_lv_ui_res_local->style_bar_e, lv_palette_main(LV_PALETTE_BLUE));
        lv_style_set_radius(&app_lv_ui_res_local->style_bar_e, 30);
        lv_style_init(&app_lv_ui_res_local->style_bar_i);
        lv_style_set_bg_opa(&app_lv_ui_res_local->style_bar_i, LV_OPA_COVER);
        lv_style_set_bg_color(&app_lv_ui_res_local->style_bar_i, lv_palette_main(LV_PALETTE_BLUE));
        lv_style_set_bg_grad_color(&app_lv_ui_res_local->style_bar_i, lv_palette_main(LV_PALETTE_GREEN));
        lv_style_set_bg_grad_dir(&app_lv_ui_res_local->style_bar_i, LV_GRAD_DIR_HOR);
        lv_style_set_radius(&app_lv_ui_res_local->style_bar_i, 30);
        /* 初始化场景 */
        app_lv_ui_res_local->scene = lv_obj_create(lv_scr_act());
        lv_obj_set_size(app_lv_ui_res_local->scene, LV_HOR_RES, LV_VER_RES);
        lv_obj_add_style(app_lv_ui_res_local->scene, &app_lv_ui_res_local->style_scene, 0);
        app_lv_ui_res_local->label = lv_label_create(app_lv_ui_res_local->scene);
        lv_label_set_long_mode(app_lv_ui_res_local->label, LV_LABEL_LONG_WRAP);
        lv_label_set_text_static(app_lv_ui_res_local->label, "LVGL Watch Enter");
        lv_obj_set_style_text_opa(app_lv_ui_res_local->label, 0, 0);
        lv_obj_center(app_lv_ui_res_local->label);
        app_lv_ui_res_local->bar = lv_bar_create(app_lv_ui_res_local->scene);
        lv_obj_remove_style_all(app_lv_ui_res_local->bar);
        lv_obj_set_size(app_lv_ui_res_local->bar, 200, 20);
        lv_bar_set_range(app_lv_ui_res_local->bar, 0, 100);
        lv_obj_add_style(app_lv_ui_res_local->bar, &app_lv_ui_res_local->style_bar_e, 0);
        lv_obj_add_style(app_lv_ui_res_local->bar, &app_lv_ui_res_local->style_bar_i, LV_PART_INDICATOR);
        lv_obj_align(app_lv_ui_res_local->bar, LV_DIR_BOTTOM, 0, 50);
        /* 初始化显示动画 */
        lv_anim_init(&app_lv_ui_res_local->anim);
        lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
        lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
        lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, 0);
        lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 100);
        lv_anim_set_time(&app_lv_ui_res_local->anim, 3000);
        lv_anim_start(&app_lv_ui_res_local->anim);
    }
    app_lv_scene_start.self = app_lv_ui_res_local == NULL ? NULL :
                              app_lv_ui_res_local->scene;
}

static void app_lv_ui_start_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_start.self = app_lv_ui_res_local == NULL ? NULL :
                              app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_start = {
    /* 场景资源节点 */
    .presenter = NULL,
    .show = app_lv_ui_start_show,
    .hide = app_lv_ui_start_hide,
};

