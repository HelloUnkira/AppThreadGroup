
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_remind_drink.h"
#include "app_lv_ui_remind_drink_presenter.h"

#include "app_lv_ui_clock_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

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
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_remind_drink_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_ui_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Drink Remind");
        /* 中心文本 */
        lv_obj_t *times_up = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(times_up, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_align(times_up, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(times_up, "Drink Remind Times Up");
        /* 下部按键 */
        lv_obj_t *btn = NULL, *btn_c = NULL, *lbl_c = NULL;
        btn = app_lv_ui_style_one_btn(app_lv_ui_res_local->scene, &btn_c, &lbl_c);
        lv_obj_add_event_cb(btn_c, app_lv_ui_event_click_turn_back_cb, LV_EVENT_CLICKED, NULL);
        lv_label_set_text(lbl_c, "Confirm");
        /* 初始化显示动画 */
        app_lv_ui_style_object_anim(app_lv_ui_res_local->scene,
                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
    app_lv_scene_remind_drink.self = app_lv_ui_res_local == NULL ? NULL :
                                     app_lv_ui_res_local->scene;
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_remind_drink_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_remind_drink.self = app_lv_ui_res_local == NULL ? NULL :
                                     app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_remind_drink = {
    /* 场景资源节点 */
    .show = app_lv_ui_remind_drink_show,
    .hide = app_lv_ui_remind_drink_hide,
};