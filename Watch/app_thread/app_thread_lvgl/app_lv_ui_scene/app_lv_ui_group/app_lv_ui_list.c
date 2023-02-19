
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
} app_lv_ui_res_local_t;

typedef struct {
    const char     *name;
    app_lv_scene_t *scene;
    lv_obj_t       *btn;
} app_lv_ui_res_list_t;

static app_lv_ui_res_list_t app_lv_ui_res_list[] = {
    {"Null",                &app_lv_scene_watch,},
    {"Null",                &app_lv_scene_watch,},
    {"Stopwatch",           &app_lv_scene_stopwatch,},
    {"Countdown",           &app_lv_scene_countdown,},
    {"Internal Test",       &app_lv_scene_test_list,},
};

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_list_btn_cb(lv_event_t *e)
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

/*@brief 界面显示
 *@brief scene 场景
 */
static void app_lv_ui_list_show(void *scene)
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
        lv_obj_t *label = lv_label_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(label, LV_HOR_RES - 20, 30);
        lv_obj_set_style_pad_all(label, 0, 0);
        lv_obj_set_style_bg_color(label,  lv_color_black(), 0);
        lv_obj_set_style_border_side(label, 0, 0);
        lv_obj_set_style_border_width(label, 0, 0);
        lv_obj_set_style_bg_color(label,  lv_color_black(), 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_line_space(label, 5, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_text_static(label, "Watch List");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        /* 初始化列表,中部 */
        lv_obj_t *list = lv_list_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 40);
        lv_obj_set_style_pad_all(list, 10, 0);
        lv_obj_set_style_pad_row(list, 10, 0);
        lv_obj_set_style_bg_color(list, lv_color_black(), 0);
        lv_obj_set_style_border_side(list, 0, 0);
        lv_obj_set_style_border_width(list, 0, 0);
        lv_obj_align_to(list, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        /* 为列表批量追加按钮 */
        for (uint32_t idx = 0; idx < sizeof(app_lv_ui_res_list) / sizeof(app_lv_ui_res_list[0]); idx++) {
            lv_obj_t *btn = lv_btn_create(list);
            lv_obj_set_size(btn, LV_HOR_RES - 40, 30);
            lv_obj_set_style_pad_all(btn, 0, 0);
            lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_border_side(btn, 0, 0);
            lv_obj_set_style_border_width(btn, 0, 0);
            lv_obj_set_style_shadow_width(btn, 0, 0);
            lv_obj_add_event_cb(btn, app_lv_ui_list_btn_cb, LV_EVENT_CLICKED, app_lv_ui_res_list[idx].scene);
            lv_obj_t *lab = lv_label_create(btn);
            lv_obj_set_style_text_color(lab, lv_color_white(), 0);
            lv_label_set_text(lab, app_lv_ui_res_list[idx].name);
            lv_obj_align(lab, LV_ALIGN_LEFT_MID, 10, 0);
            app_lv_ui_res_list[idx].btn = btn;
        }
    }
    app_lv_scene_list.self = app_lv_ui_res_local == NULL ? NULL :
                             app_lv_ui_res_local->scene;
}

/*@brief 界面隐藏
 *@brief scene 场景
 */
static void app_lv_ui_list_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        for (uint32_t idx = 0; idx < sizeof(app_lv_ui_res_list) / sizeof(app_lv_ui_res_list[0]); idx++) {
            lv_obj_remove_event_cb(app_lv_ui_res_list[idx].btn, app_lv_ui_list_btn_cb);
            app_lv_ui_res_list[idx].btn = NULL;
        }
        /* 场景去除默认事件 */
        app_lv_ui_event_default_clr(app_lv_ui_res_local->scene);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_list.self = app_lv_ui_res_local == NULL ? NULL :
                             app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_list = {
    /* 场景资源节点 */
    .show = app_lv_ui_list_show,
    .hide = app_lv_ui_list_hide,
};
