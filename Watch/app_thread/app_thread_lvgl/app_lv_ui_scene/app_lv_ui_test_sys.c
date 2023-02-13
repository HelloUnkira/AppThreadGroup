
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_!util.h"
#include "app_lv_ui_scene_set.h"

typedef struct {
    lv_obj_t *scene;
    lv_obj_t *title;
    lv_obj_t *label;
    lv_obj_t *list;
} app_lv_ui_res_local_t;

typedef struct {
    const char     *scene_name;
    app_lv_scene_t *scene;
} app_lv_ui_res_list_t;

static const app_lv_ui_res_list_t app_lv_ui_res_list[] = {
    // {"sys clock",   &app_lv_scene_test_sys_clock,},
    {"null",        &app_lv_scene_watch,},
    {"null",        &app_lv_scene_watch,},
    {"null",        &app_lv_scene_watch,},
    {"null",        &app_lv_scene_watch,},
};

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static void app_lv_ui_test_sys_list_btn_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t *scene = lv_event_get_user_data(e);
        app_lv_scene_add(scene);
        break;
    }
    default:
        break;
    break;
    }
}

static void app_lv_ui_test_sys_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = app_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->scene, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->scene, 0, 0);
        lv_obj_set_style_opa(app_lv_ui_res_local->scene, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->scene, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->scene, lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->scene, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->scene, 0, 0);
        /* 场景添加默认事件 */
        app_lv_ui_event_default_set(app_lv_ui_res_local->scene);
        /* 初始化标签,上中部 */
        app_lv_ui_res_local->title = lv_obj_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->title, LV_HOR_RES - 20, 40);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->title, 0, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->title,  lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->title, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->title, 0, 0);
        lv_obj_align(app_lv_ui_res_local->title, LV_ALIGN_TOP_MID, 0, 0);
        app_lv_ui_res_local->label = lv_label_create(app_lv_ui_res_local->title);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->label,  lv_color_black(), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_label_set_text_static(app_lv_ui_res_local->label, "internal test");
        lv_obj_align(app_lv_ui_res_local->label, LV_ALIGN_CENTER, 0, 0);
        /* 初始化列表,中部 */
        app_lv_ui_res_local->list = lv_list_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->list, LV_HOR_RES, LV_VER_RES - 40);
        lv_obj_set_style_pad_row(app_lv_ui_res_local->list, 10, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->list, lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->list, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->list, 0, 0);
        lv_obj_align_to(app_lv_ui_res_local->list, app_lv_ui_res_local->title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 为列表批量追加按钮 */
        for (uint32_t idx = 0; idx < sizeof(app_lv_ui_res_list) / sizeof(app_lv_ui_res_list[0]); idx++) {
            lv_obj_t *btn = lv_btn_create(app_lv_ui_res_local->list);
            lv_obj_add_event_cb(btn, app_lv_ui_test_sys_list_btn_cb, LV_EVENT_CLICKED, app_lv_ui_res_list[idx].scene);
            lv_obj_set_size(btn, LV_HOR_RES - 40, 30);
            lv_obj_set_style_bg_color(btn,  lv_palette_main(LV_PALETTE_GREY), 0);
            lv_obj_t *lab = lv_label_create(btn);
            lv_label_set_text(lab, app_lv_ui_res_list[idx].scene_name);
            lv_obj_align(lab, LV_ALIGN_LEFT_MID, 10, 0);
        }
    }
    app_lv_scene_test_sys.self = app_lv_ui_res_local == NULL ? NULL :
                                 app_lv_ui_res_local->scene;
}

static void app_lv_ui_test_sys_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 场景去除默认事件 */
        app_lv_ui_event_default_clr(app_lv_ui_res_local->scene);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_test_sys.self = app_lv_ui_res_local == NULL ? NULL :
                                 app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_test_sys = {
    /* 场景资源节点 */
    .presenter = NULL,
    .show = app_lv_ui_test_sys_show,
    .hide = app_lv_ui_test_sys_hide,
};

