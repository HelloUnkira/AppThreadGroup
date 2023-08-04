
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_list.h"

typedef struct {
    lv_obj_t *scene;
} app_lv_ui_res_local_t;

typedef struct {
    const char     *name;
    app_lv_scene_t *scene;
} app_lv_ui_res_list_t;

static const app_lv_ui_res_list_t app_lv_ui_res_list[] = {
    {"Qrcode",              &app_lv_ui_qrcode,},
    {"Null",                &app_lv_ui_null,},
    {"Data Daily",          &app_lv_ui_data_daily,},
    {"Calculator",          &app_lv_ui_calculator,},
    {"Stopwatch",           &app_lv_ui_stopwatch,},
    {"Countdown",           &app_lv_ui_countdown,},
    {"Null",                &app_lv_ui_null,},
    {"2048",                &app_lv_ui_2048,},
    {"Tetris",              &app_lv_ui_tetris,},
    {"Maze",                &app_lv_ui_maze,},
    {"Snake",               &app_lv_ui_snake,},
    {"Null",                &app_lv_ui_null,},
    {"Internal Test",       &app_lv_ui_test_list,},
};

static const uint32_t app_lv_ui_res_list_len = sizeof(app_lv_ui_res_list) / sizeof(app_lv_ui_res_list[0]);

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_list_btn_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t *scene = lv_event_get_user_data(e);
        app_lv_scene_add(scene, false);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_list_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        app_lv_ui_list.self = app_lv_ui_res_local->scene;
        /* 初始化标签,上中部 */
        lv_obj_t *label = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_label_set_text_static(label, "Watch List");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        /* 初始化列表,中部 */
        lv_obj_t *list = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 40);
        lv_obj_set_style_pad_all(list, 10, 0);
        lv_obj_set_style_pad_row(list, 10, 0);
        lv_obj_align_to(list, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 为列表批量追加按钮 */
        for (uint32_t idx = 0; idx < app_lv_ui_res_list_len; idx++) {
            lv_obj_t *btn = app_lv_style_btn(list);
            lv_obj_set_size(btn, LV_HOR_RES - 40, 30);
            lv_obj_add_event_cb(btn, app_lv_ui_list_btn_cb, LV_EVENT_CLICKED, app_lv_ui_res_list[idx].scene);
            lv_obj_t *lab = app_lv_style_label(btn);
            lv_label_set_text(lab, app_lv_ui_res_list[idx].name);
            lv_obj_align(lab, LV_ALIGN_LEFT_MID, 10, 0);
        }
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_list_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_list.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_list = {
    /* 场景资源节点 */
    .show = app_lv_ui_list_show,
    .hide = app_lv_ui_list_hide,
};
