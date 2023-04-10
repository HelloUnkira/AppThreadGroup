
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_style.h"

#include "app_lv_ui_stopwatch.h"
#include "app_lv_ui_stopwatch_presenter.h"

#include "app_lv_ui_clock_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *tick_main;
    lv_obj_t *tick_list;
    lv_obj_t *btn_l;
    lv_obj_t *lbl_l;
    lv_obj_t *btn_r;
    lv_obj_t *lbl_r;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 清除计数列表
*/
static void app_lv_ui_local_tick_list_clr(void)
{
    if (app_lv_ui_res_local->tick_list != NULL) {
        lv_obj_del(app_lv_ui_res_local->tick_list);
        app_lv_ui_res_local->tick_list = NULL;
    }
}

/*@brief 生成计数列表
*/
static void app_lv_ui_local_tick_list_set(bool refr)
{
    if (app_lv_ui_res_local->tick_list == NULL) {
        app_lv_ui_res_local->tick_list  = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_ui_style_object(app_lv_ui_res_local->tick_list);
        lv_obj_set_size(app_lv_ui_res_local->tick_list, LV_HOR_RES, LV_VER_RES - 160);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->tick_list, 10, 0);
        lv_obj_set_style_pad_row(app_lv_ui_res_local->tick_list,  5, 0);
        lv_obj_align_to(app_lv_ui_res_local->tick_list, app_lv_ui_res_local->tick_main, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        /* 生成点击时Tick */
        if (refr)
            app_lv_ui_stopwatch_presenter.refr_tick_list();
        app_lv_ui_stopwatch_t *tick_list = NULL;
        app_lv_ui_stopwatch_presenter.get_tick_list(&tick_list);
        uint8_t tick_num = app_lv_ui_stopwatch_presenter.get_tick_num();
        for (uint8_t idx = 0; idx < tick_num; idx++) {
            lv_obj_t *label = app_lv_ui_style_label(app_lv_ui_res_local->tick_list);
            lv_obj_set_size(label, LV_HOR_RES - 40, 20);
            lv_obj_set_style_text_color(label, lv_color_black(), 0);
            lv_label_set_text(label, "");
            lv_obj_t *label1 = app_lv_ui_style_label(label);
            lv_obj_set_width(label1, 30);
            lv_obj_set_style_text_color(label1, lv_palette_main(LV_PALETTE_GREY), 0);
            lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_RIGHT, 0);
            lv_label_set_text_fmt(label1, "%u", tick_num - idx);
            lv_obj_align(label1, LV_ALIGN_LEFT_MID, 0, 0);
            lv_obj_t *label2 = lv_label_create(label);
            lv_obj_set_style_text_color(label2, lv_palette_main(LV_PALETTE_GREY), 0);
            lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_RIGHT, 0);
            lv_label_set_text_fmt(label2, "%.2u:%.2u:%.2u.%.3u",
                                  tick_list[idx].hour, tick_list[idx].minute, tick_list[idx].second, tick_list[idx].msec);
            lv_obj_align(label2, LV_ALIGN_RIGHT_MID, 0, 0);
            /* 第一个的背景色染成亮蓝色 */
            if (idx == 0)
            {
                lv_obj_set_style_bg_opa(label,  LV_OPA_50, 0);
                lv_obj_set_style_bg_opa(label1, LV_OPA_50, 0);
                lv_obj_set_style_bg_opa(label2, LV_OPA_50, 0);
                lv_obj_set_style_bg_color(label,  lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
                lv_obj_set_style_bg_color(label1, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
                lv_obj_set_style_bg_color(label2, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
            }
        }
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_l_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 按键响应 */
        if (app_lv_ui_stopwatch_presenter.get_onoff()) {
            app_lv_ui_stopwatch_presenter.stop();
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_YELLOW), 0);
            lv_label_set_text(app_lv_ui_res_local->lbl_l, "Run");
            lv_label_set_text(app_lv_ui_res_local->lbl_r, "Reset");
        } else {
            app_lv_ui_stopwatch_presenter.start();
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_YELLOW), 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_label_set_text(app_lv_ui_res_local->lbl_l, "Pause");
            lv_label_set_text(app_lv_ui_res_local->lbl_r, "Tick");
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
        if (app_lv_ui_stopwatch_presenter.get_onoff()) {
            /* Tick */
            app_lv_ui_local_tick_list_clr();
            app_lv_ui_local_tick_list_set(true);
        } else {
            app_lv_ui_stopwatch_presenter.reset();
            app_lv_ui_stopwatch_presenter.clean_tick_list();
            app_lv_ui_local_tick_list_clr();
        }
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
    /* 左上角系统时钟更新 */
    lv_label_set_text_fmt(app_lv_ui_res_local->time, "%s %.2u:%.2u",
                          app_lv_ui_clock_presenter.is_24() ? "" :
                          app_lv_ui_clock_presenter.is_am() ? "AM" :
                          app_lv_ui_clock_presenter.is_pm() ? "PM" : "",
                          app_lv_ui_clock_presenter.get_hour(),
                          app_lv_ui_clock_presenter.get_minute());
    /* 主题计数时钟更新 */
    app_lv_ui_stopwatch_t stopwatch = {0};
    app_lv_ui_stopwatch_presenter.get_tick_main(&stopwatch);
    lv_label_set_text_fmt(app_lv_ui_res_local->tick_main, "%.2u:%.2u:%.2u.%.3u",
                          stopwatch.hour, stopwatch.minute, stopwatch.second, stopwatch.msec);
    /* 按键文本更新 */
    if (app_lv_ui_stopwatch_presenter.get_onoff()) {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_YELLOW), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "Pause");
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "Tick");
    } else {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_YELLOW), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "Run");
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "Reset");
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_stopwatch_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        app_lv_ui_stopwatch.self = app_lv_ui_res_local->scene;
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_ui_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Stopwatch");
        /* 主体时间 */
        app_lv_ui_res_local->tick_main = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(app_lv_ui_res_local->tick_main, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_align_to(app_lv_ui_res_local->tick_main, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 计数列表(点击时更新) */
        app_lv_ui_res_local->tick_list = NULL;
        app_lv_ui_local_tick_list_set(false);
        /* 底部双按钮组 */
        lv_obj_t *btn = app_lv_ui_style_two_btns(app_lv_ui_res_local->scene,
                                                &app_lv_ui_res_local->btn_l,
                                                &app_lv_ui_res_local->lbl_l,
                                                &app_lv_ui_res_local->btn_r,
                                                &app_lv_ui_res_local->lbl_r);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_l, app_lv_ui_btn_l_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_r, app_lv_ui_btn_r_cb, LV_EVENT_CLICKED, NULL);
        /* 初始化显示动画 */
        app_lv_ui_style_object_anim(app_lv_ui_res_local->scene,

                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_stopwatch_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 回收列表资源 */
        app_lv_ui_local_tick_list_clr();
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_stopwatch.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_stopwatch = {
    /* 场景资源节点 */
    .show = app_lv_ui_stopwatch_show,
    .hide = app_lv_ui_stopwatch_hide,
};
