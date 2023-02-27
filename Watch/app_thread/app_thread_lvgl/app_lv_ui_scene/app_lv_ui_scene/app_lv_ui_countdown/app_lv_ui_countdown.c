
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_check_time.h"
#include "app_lv_ui_countdown.h"
#include "app_lv_ui_countdown_remind.h"
#include "app_lv_ui_countdown_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *title;
    lv_obj_t *name;
    lv_obj_t *time;
    lv_obj_t *tick;
    lv_obj_t *btn_l;
    lv_obj_t *lbl_l;
    lv_obj_t *btn_r;
    lv_obj_t *lbl_r;
    lv_obj_t *rol_h;
    lv_obj_t *rol_m;
    lv_obj_t *rol_s;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static const char app_lv_ui_res_0_23[] = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
static const char app_lv_ui_res_0_59[] = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";

#define app_lv_ui_res_tick_font         &lv_font_montserrat_28
#define app_lv_ui_res_rolsel_font       &lv_font_montserrat_28
#define app_lv_ui_res_rolcus_font       &lv_font_montserrat_22
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
static void app_lv_ui_btn_l_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 按键响应 */
        if (app_lv_ui_countdown_presenter.get_onoff()) {
            app_lv_ui_countdown_presenter.stop();
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_YELLOW), 0);
            lv_label_set_text(app_lv_ui_res_local->lbl_l, "Run");
            lv_label_set_text(app_lv_ui_res_local->lbl_r, "Set");
        } else {
            app_lv_ui_countdown_presenter.start();
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_YELLOW), 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_label_set_text(app_lv_ui_res_local->lbl_l, "Pause");
            lv_label_set_text(app_lv_ui_res_local->lbl_r, "Reset");
        }
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
        /* 按键响应 */
        if (app_lv_ui_countdown_presenter.get_onoff()) {
            app_lv_ui_countdown_t countdown_0 = {0};
            app_lv_ui_countdown_presenter.stop();
            app_lv_ui_countdown_presenter.set(&countdown_0);
            app_lv_ui_countdown_t countdown;
            app_lv_ui_countdown_presenter.get(&countdown);
            lv_roller_set_selected(app_lv_ui_res_local->rol_h, countdown.hour,   LV_ANIM_OFF);
            lv_roller_set_selected(app_lv_ui_res_local->rol_m, countdown.minute, LV_ANIM_OFF);
            lv_roller_set_selected(app_lv_ui_res_local->rol_s, countdown.second, LV_ANIM_OFF);
        } else {
            app_lv_ui_countdown_t countdown = {0};
            countdown.hour   = lv_roller_get_selected(app_lv_ui_res_local->rol_h);
            countdown.minute = lv_roller_get_selected(app_lv_ui_res_local->rol_m);
            countdown.second = lv_roller_get_selected(app_lv_ui_res_local->rol_s);
            app_lv_ui_countdown_presenter.set(&countdown);
        }
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_rol_sel_cb(lv_event_t *e)
{
    
    app_lv_ui_roller_mask_event_cb(e);
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 左上角系统时钟更新 */
    lv_label_set_text_fmt(app_lv_ui_res_local->time, "%s %.2u:%.2u",
                          app_lv_ui_countdown_presenter.is_24_mode() ? "" :
                          app_lv_ui_countdown_presenter.is_am() ? "AM" :
                          app_lv_ui_countdown_presenter.is_pm() ? "PM" : "",
                          app_lv_ui_countdown_presenter.get_user_hour(),
                          app_lv_ui_countdown_presenter.get_user_minute());
    /* 主题计数时钟更新 */
    app_lv_ui_countdown_t countdown = {0};
    app_lv_ui_countdown_presenter.get_tick(&countdown);
    lv_label_set_text_fmt(app_lv_ui_res_local->tick, "%.2u:%.2u:%.2u.%.3u",
                          countdown.hour, countdown.minute, countdown.second, countdown.msec);
    /* 按键文本更新 */
    if (app_lv_ui_countdown_presenter.get_onoff()) {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_YELLOW), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "Pause");
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "Reset");
    } else {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "Run");
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "Set");
    }
}

/*@brief 界面显示
 *@brief scene 场景
 */
static void app_lv_ui_countdown_show(void *scene)
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
        lv_label_set_text(lbl_name, "< Countdown");
        lv_obj_align(lbl_name, LV_ALIGN_TOP_LEFT, 0, 0);
        /* 右上角时间 */
        app_lv_ui_res_local->time = lv_label_create(app_lv_ui_res_local->title);
        lv_obj_set_size(app_lv_ui_res_local->time, LV_HOR_RES / 2 - 20, 20);
        lv_obj_set_style_text_color(app_lv_ui_res_local->time, lv_color_white(), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->time, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(app_lv_ui_res_local->time, "00:00");
        lv_obj_align(app_lv_ui_res_local->time, LV_ALIGN_RIGHT_MID, -10, 0);
        /* 主体文本 */
        app_lv_ui_res_local->tick = lv_label_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->tick, LV_HOR_RES, 30);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->tick,  lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->tick, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->tick, 0, 0);
        lv_obj_set_style_text_font(app_lv_ui_res_local->tick, app_lv_ui_res_tick_font, 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->tick, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->tick, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(app_lv_ui_res_local->tick, "00:00:00.000");
        lv_obj_align_to(app_lv_ui_res_local->tick, app_lv_ui_res_local->title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制日期后三位滚轮 */
        lv_obj_t *rol_set = lv_obj_create(app_lv_ui_res_local->scene);
        lv_obj_add_flag(rol_set, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_width(rol_set, LV_HOR_RES);
        lv_obj_set_style_pad_all(rol_set, 0, 0);
        lv_obj_set_style_bg_color(rol_set,  lv_color_black(), 0);
        lv_obj_set_style_border_side(rol_set, 0, 0);
        lv_obj_set_style_border_width(rol_set, 0, 0);
        lv_obj_align_to(rol_set, app_lv_ui_res_local->tick, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制分滚轮,中心对齐 */
        app_lv_ui_res_local->rol_m = lv_roller_create(rol_set);
        lv_obj_add_flag(app_lv_ui_res_local->rol_m, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_width(app_lv_ui_res_local->rol_m, 50);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->rol_m, LV_OPA_0, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->rol_m, LV_OPA_0, LV_PART_SELECTED);
        lv_obj_set_style_text_font(app_lv_ui_res_local->rol_m, app_lv_ui_res_rolcus_font, 0);
        lv_obj_set_style_text_font(app_lv_ui_res_local->rol_m, app_lv_ui_res_rolsel_font, LV_PART_SELECTED);
        lv_obj_set_style_text_align(app_lv_ui_res_local->rol_m, LV_TEXT_ALIGN_CENTER, 0);
        lv_roller_set_options(app_lv_ui_res_local->rol_m, app_lv_ui_res_0_59, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(app_lv_ui_res_local->rol_m, 3);
        lv_obj_align(app_lv_ui_res_local->rol_m, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->rol_m, app_lv_ui_rol_sel_cb, LV_EVENT_ALL, NULL);
        /* 绘制时滚轮,分滚轮左外对齐 */
        app_lv_ui_res_local->rol_h = lv_roller_create(rol_set);
        lv_obj_add_flag(app_lv_ui_res_local->rol_h, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_width(app_lv_ui_res_local->rol_h, 50);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->rol_h, LV_OPA_0, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->rol_h, LV_OPA_0, LV_PART_SELECTED);
        lv_obj_set_style_text_font(app_lv_ui_res_local->rol_h, app_lv_ui_res_rolcus_font, 0);
        lv_obj_set_style_text_font(app_lv_ui_res_local->rol_h, app_lv_ui_res_rolsel_font, LV_PART_SELECTED);
        lv_obj_set_style_text_align(app_lv_ui_res_local->rol_h, LV_TEXT_ALIGN_CENTER, 0);
        lv_roller_set_options(app_lv_ui_res_local->rol_h, app_lv_ui_res_0_23, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(app_lv_ui_res_local->rol_h, 3);
        lv_obj_align_to(app_lv_ui_res_local->rol_h, app_lv_ui_res_local->rol_m, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->rol_h, app_lv_ui_rol_sel_cb, LV_EVENT_ALL, NULL);
        /* 绘制秒滚轮,分滚轮右外对齐 */
        app_lv_ui_res_local->rol_s = lv_roller_create(rol_set);
        lv_obj_add_flag(app_lv_ui_res_local->rol_s, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_width(app_lv_ui_res_local->rol_s, 50);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->rol_s, LV_OPA_0, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->rol_s, LV_OPA_0, LV_PART_SELECTED);
        lv_obj_set_style_text_font(app_lv_ui_res_local->rol_s, app_lv_ui_res_rolcus_font, 0);
        lv_obj_set_style_text_font(app_lv_ui_res_local->rol_s, app_lv_ui_res_rolsel_font, LV_PART_SELECTED);
        lv_obj_set_style_text_align(app_lv_ui_res_local->rol_s, LV_TEXT_ALIGN_CENTER, 0);
        lv_roller_set_options(app_lv_ui_res_local->rol_s, app_lv_ui_res_0_59, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(app_lv_ui_res_local->rol_s, 3);
        lv_obj_align_to(app_lv_ui_res_local->rol_s, app_lv_ui_res_local->rol_m, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->rol_s, app_lv_ui_rol_sel_cb, LV_EVENT_ALL, NULL);
        /* 下部按键 */
        lv_obj_t *btn = lv_obj_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(btn, LV_HOR_RES, 60);
        lv_obj_set_style_pad_all(btn, 10, 0);
        lv_obj_set_style_bg_color(btn,  lv_color_black(), 0);
        lv_obj_set_style_border_side(btn, 0, 0);
        lv_obj_set_style_border_width(btn, 0, 0);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
        /* 左下角按键 */
        app_lv_ui_res_local->btn_l = lv_btn_create(btn);
        lv_obj_set_size(app_lv_ui_res_local->btn_l, LV_HOR_RES / 2 - 20, 40);
        lv_obj_set_style_radius(app_lv_ui_res_local->btn_l, 45, 0);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->btn_l, 0, 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->btn_l, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->btn_l, 0, 0);
        lv_obj_set_style_shadow_width(app_lv_ui_res_local->btn_l, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_l, app_lv_ui_btn_l_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(app_lv_ui_res_local->btn_l, LV_ALIGN_LEFT_MID, 5, 0);
        app_lv_ui_res_local->lbl_l = lv_label_create(app_lv_ui_res_local->btn_l);
        lv_obj_set_style_text_font(app_lv_ui_res_local->lbl_l, app_lv_ui_res_btn_text_font, 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "L_Btn");
        lv_obj_align(app_lv_ui_res_local->lbl_l, LV_ALIGN_CENTER, 0, 0);
        /* 右下角按键 */
        app_lv_ui_res_local->btn_r = lv_btn_create(btn);
        lv_obj_set_size(app_lv_ui_res_local->btn_r, LV_HOR_RES / 2 - 20, 40);
        lv_obj_set_style_radius(app_lv_ui_res_local->btn_r, 45, 0);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->btn_r, 0, 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->btn_r, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->btn_r, 0, 0);
        lv_obj_set_style_shadow_width(app_lv_ui_res_local->btn_r, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_r, app_lv_ui_btn_r_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(app_lv_ui_res_local->btn_r, LV_ALIGN_RIGHT_MID, -5, 0);
        app_lv_ui_res_local->lbl_r = lv_label_create(app_lv_ui_res_local->btn_r);
        lv_obj_set_style_text_font(app_lv_ui_res_local->lbl_r, app_lv_ui_res_btn_text_font, 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->lbl_r, lv_color_white(), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "R_Btn");
        lv_obj_align(app_lv_ui_res_local->lbl_r, LV_ALIGN_CENTER, 0, 0);
        /* 初始化显示动画 */
        lv_anim_init(&app_lv_ui_res_local->anim);
        lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
        lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
        lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 10);
        lv_anim_set_time(&app_lv_ui_res_local->anim, 1000);
        lv_anim_start(&app_lv_ui_res_local->anim);
        /* 滚轮选中刷新 */
        app_lv_ui_countdown_t countdown;
        app_lv_ui_countdown_presenter.get(&countdown);
        lv_roller_set_selected(app_lv_ui_res_local->rol_h, countdown.hour,   LV_ANIM_OFF);
        lv_roller_set_selected(app_lv_ui_res_local->rol_m, countdown.minute, LV_ANIM_OFF);
        lv_roller_set_selected(app_lv_ui_res_local->rol_s, countdown.second, LV_ANIM_OFF);
    }
    app_lv_scene_countdown.self = app_lv_ui_res_local == NULL ? NULL :
                                  app_lv_ui_res_local->scene;
}

/*@brief 界面隐藏
 *@brief scene 场景
 */
static void app_lv_ui_countdown_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 场景去除默认事件 */
        app_lv_ui_event_default_clr(app_lv_ui_res_local->scene);
        /* 反初始化按键事件 */
        lv_obj_remove_event_cb(app_lv_ui_res_local->btn_l, app_lv_ui_btn_l_cb);
        lv_obj_remove_event_cb(app_lv_ui_res_local->btn_r, app_lv_ui_btn_r_cb);
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_countdown.self = app_lv_ui_res_local == NULL ? NULL :
                                  app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_countdown = {
    /* 场景资源节点 */
    .show = app_lv_ui_countdown_show,
    .hide = app_lv_ui_countdown_hide,
};
