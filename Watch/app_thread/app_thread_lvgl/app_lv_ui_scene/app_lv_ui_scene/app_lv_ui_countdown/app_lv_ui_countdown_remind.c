
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_time_check.h"
#include "app_lv_ui_!util.h"
#include "app_lv_ui_scene_set.h"
#include "app_lv_ui_countdown.h"
#include "app_lv_ui_countdown_remind.h"
#include "app_lv_ui_countdown_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *title;
    lv_obj_t *name;
    lv_obj_t *time;
    lv_obj_t *btn_c;
    lv_obj_t *lbl_c;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

#define app_lv_ui_res_btn_text_font     &lv_font_montserrat_22

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_name_btn_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t scene = {0};
        app_lv_scene_del(&scene);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_c_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t scene = {0};
        app_lv_scene_del(&scene);
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
    /* 此界面禁用超时息屏及主界面回退 */
    app_lv_scene_time_check_reset();
    /* 左上角系统时钟更新 */
    lv_label_set_text_fmt(app_lv_ui_res_local->time, "%s %.2u:%.2u",
                          app_lv_ui_countdown_presenter.is_24_mode() ? "" :
                          app_lv_ui_countdown_presenter.is_am() ? "AM" :
                          app_lv_ui_countdown_presenter.is_pm() ? "PM" : "",
                          app_lv_ui_countdown_presenter.get_user_hour(),
                          app_lv_ui_countdown_presenter.get_user_minute());
}

/*@brief 界面显示
 *@brief scene 场景
 */
static void app_lv_ui_countdown_remind_show(void *scene)
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
        /* 上部文本 */
        app_lv_ui_res_local->title = lv_obj_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->title, LV_HOR_RES, 40);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->title, 10, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->title,  lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->title, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->title, 0, 0);
        /* 左上角标题 */
        app_lv_ui_res_local->name = lv_btn_create(app_lv_ui_res_local->title);
        lv_obj_set_size(app_lv_ui_res_local->name, LV_HOR_RES / 2 - 20, 20);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->name, 0, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->name, lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->name, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->name, 0, 0);
        lv_obj_set_style_shadow_width(app_lv_ui_res_local->name, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->name, app_lv_ui_name_btn_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(app_lv_ui_res_local->name, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_t *lbl_name = lv_label_create(app_lv_ui_res_local->name);
        lv_obj_set_style_text_color(lbl_name, lv_color_white(), 0);
        lv_obj_set_style_text_align(lbl_name, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_text(lbl_name, "< Stopwatch");
        lv_obj_align(lbl_name, LV_ALIGN_TOP_LEFT, 0, 0);
        /* 右上角时间 */
        app_lv_ui_res_local->time = lv_label_create(app_lv_ui_res_local->title);
        lv_obj_set_size(app_lv_ui_res_local->time, LV_HOR_RES / 2 - 20, 20);
        lv_obj_set_style_text_color(app_lv_ui_res_local->time, lv_color_white(), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->time, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(app_lv_ui_res_local->time, "00:00");
        lv_obj_align(app_lv_ui_res_local->time, LV_ALIGN_RIGHT_MID, -10, 0);
        /* 主体文本 */
        lv_obj_t *times_up = lv_label_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(times_up, LV_HOR_RES, 30);
        lv_obj_set_style_bg_color(times_up, lv_color_black(), 0);
        lv_obj_set_style_border_side(times_up, 0, 0);
        lv_obj_set_style_border_width(times_up, 0, 0);
        lv_obj_set_style_text_color(times_up, lv_color_white(), 0);
        lv_obj_set_style_text_align(times_up, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(times_up, "Countdown Times Up");
        lv_obj_align(times_up, LV_ALIGN_CENTER, 0, 0);
        /* 下部按键 */
        lv_obj_t *btn = lv_obj_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(btn, LV_HOR_RES, 60);
        lv_obj_set_style_pad_all(btn, 10, 0);
        lv_obj_set_style_bg_color(btn,  lv_color_black(), 0);
        lv_obj_set_style_border_side(btn, 0, 0);
        lv_obj_set_style_border_width(btn, 0, 0);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
        /* 中心按键 */
        app_lv_ui_res_local->btn_c = lv_btn_create(btn);
        lv_obj_set_size(app_lv_ui_res_local->btn_c, LV_HOR_RES - 20, 40);
        lv_obj_set_style_radius(app_lv_ui_res_local->btn_c, 45, 0);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->btn_c, 0, 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->btn_c, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->btn_c, 0, 0);
        lv_obj_set_style_shadow_width(app_lv_ui_res_local->btn_c, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_c, app_lv_ui_btn_c_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(app_lv_ui_res_local->btn_c, LV_ALIGN_CENTER, 0, 0);
        app_lv_ui_res_local->lbl_c = lv_label_create(app_lv_ui_res_local->btn_c);
        lv_obj_set_style_text_font(app_lv_ui_res_local->lbl_c, app_lv_ui_res_btn_text_font, 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->lbl_c, lv_color_white(), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_c, "C_Btn");
        lv_obj_align(app_lv_ui_res_local->lbl_c, LV_ALIGN_CENTER, 0, 0);
        /* 初始化显示动画 */
        lv_anim_init(&app_lv_ui_res_local->anim);
        lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
        lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
        lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 10);
        lv_anim_set_time(&app_lv_ui_res_local->anim, 1000);
        lv_anim_start(&app_lv_ui_res_local->anim);
    }
    app_lv_scene_countdown_remind.self = app_lv_ui_res_local == NULL ? NULL :
                                         app_lv_ui_res_local->scene;
}

/*@brief 界面隐藏
 *@brief scene 场景
 */
static void app_lv_ui_countdown_remind_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 场景去除默认事件 */
        app_lv_ui_event_default_clr(app_lv_ui_res_local->scene);
        /* 反初始化按键事件 */
        lv_obj_remove_event_cb(app_lv_ui_res_local->btn_c, app_lv_ui_btn_c_cb);
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_countdown_remind.self = app_lv_ui_res_local == NULL ? NULL :
                                         app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_countdown_remind = {
    /* 场景资源节点 */
    .show = app_lv_ui_countdown_remind_show,
    .hide = app_lv_ui_countdown_remind_hide,
};






