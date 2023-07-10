
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_style.h"

#include "app_lv_ui_log_text.h"
#include "app_lv_ui_log_text_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *text;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_l_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 循环提出所有条目 */
        lv_label_set_text(app_lv_ui_res_local->text, "");
        app_lv_ui_log_text_presenter.peek_reset();
        char *str = NULL, *str_new = NULL;
        do {
            str = app_lv_ui_log_text_presenter.peek_one();
            uint32_t size = strlen(str) + 2;
            str_new = lv_mem_alloc(size);
            lv_memset(str_new, 0, size);
            lv_memcpy(str_new, str, size);
            str_new[size - 2] = '\n';
            lv_label_ins_text(app_lv_ui_res_local->text, strlen(lv_label_get_text(app_lv_ui_res_local->text)), str_new);
            lv_mem_free(str_new);
        } while (strlen(str) != 0);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_r_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 复位 */
        app_lv_ui_log_text_presenter.reset();
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_log_text_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        app_lv_ui_log_text.self = app_lv_ui_res_local->scene;
        /* 初始化标签,上中部 */
        lv_obj_t *label = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text_static(label, "Trace Log");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, app_lv_ui_ver_pct(2));
        /* trace_text */
        lv_obj_t *text_box = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_ui_style_object(text_box);
        lv_obj_set_style_bg_color(text_box, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
        lv_obj_set_size(text_box, app_lv_ui_hor_pct(90), app_lv_ui_ver_pct(63));
        lv_obj_align_to(text_box, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        app_lv_ui_res_local->text = app_lv_ui_style_label(text_box);
        lv_obj_set_style_text_align(app_lv_ui_res_local->text, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->text, LV_LABEL_LONG_WRAP);
        lv_label_set_text(app_lv_ui_res_local->text, "");
        /* 下部按键 */
        /* 底部双按钮组 */
        lv_obj_t *btn_l = NULL, *lbl_l = NULL, *btn_r = NULL, *lbl_r = NULL;
        lv_obj_t *btn = app_lv_ui_style_two_btns(app_lv_ui_res_local->scene, &btn_l, &lbl_l, &btn_r, &lbl_r);
        lv_obj_add_event_cb(btn_l, app_lv_ui_btn_l_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(btn_r, app_lv_ui_btn_r_cb, LV_EVENT_CLICKED, NULL);
        lv_label_set_text_static(lbl_l, "Peek");
        lv_label_set_text_static(lbl_r, "Reset");
        /* 初始化显示动画 */
        app_lv_ui_style_object_anim(app_lv_ui_res_local->scene,
                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    LV_ANIM_REPEAT_INFINITE, 0, 1, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_log_text_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_log_text.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_log_text = {
    /* 场景资源节点 */
    .show = app_lv_ui_log_text_show,
    .hide = app_lv_ui_log_text_hide,
};
