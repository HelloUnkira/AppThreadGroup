
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_data_daily.h"
#include "app_lv_ui_data_daily_presenter.h"

#include "app_lv_ui_clock_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *arc_cal;
    lv_obj_t *arc_step;
    lv_obj_t *arc_walk_cnt;
    lv_obj_t *chart_step;
    lv_obj_t *chart_step_ser;
    lv_obj_t *chart_cal;
    lv_obj_t *chart_cal_ser;
    lv_obj_t *chart_act;
    lv_obj_t *chart_act_ser;
    lv_obj_t *data_cal_cur;
    lv_obj_t *data_cal_tar;
    lv_obj_t *data_cal_unit;
    lv_obj_t *data_step_cur;
    lv_obj_t *data_step_tar;
    lv_obj_t *data_step_unit;
    lv_obj_t *data_act_cur;
    lv_obj_t *data_act_tar;
    lv_obj_t *data_act_unit;
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
    /* 数据资源刷新 */
    if (value == 0) {
        /* 三环数据刷新 */
        float f_val_cal =       (float)app_lv_ui_data_daily_presenter.get_cur_cal() /
                                (float)app_lv_ui_data_daily_presenter.get_tar_cal() + 0.005;
        float f_val_step =      (float)app_lv_ui_data_daily_presenter.get_cur_step() /
                                (float)app_lv_ui_data_daily_presenter.get_tar_step() + 0.005;
        float f_val_walk_cnt =  (float)app_lv_ui_data_daily_presenter.get_cur_walk_cnt() /
                                (float)app_lv_ui_data_daily_presenter.get_tar_walk_cnt() + 0.005;
        int16_t val_cal        = (f_val_cal         <= 0.999) ? (100.0 * f_val_cal)         : 100;
        int16_t val_step       = (f_val_step        <= 0.999) ? (100.0 * f_val_step)        : 100;
        int16_t val_walk_cnt   = (f_val_walk_cnt    <= 0.999) ? (100.0 * f_val_walk_cnt)    : 100;
        lv_arc_set_value(app_lv_ui_res_local->arc_cal, val_cal);
        lv_arc_set_value(app_lv_ui_res_local->arc_step, val_step);
        lv_arc_set_value(app_lv_ui_res_local->arc_walk_cnt, val_walk_cnt);
        /* 三环数据显示 */
        lv_label_set_text_fmt(app_lv_ui_res_local->data_cal_cur,    "%d",  app_lv_ui_data_daily_presenter.get_cur_cal());
        lv_label_set_text_fmt(app_lv_ui_res_local->data_cal_tar,    "/%d", app_lv_ui_data_daily_presenter.get_tar_cal());
        lv_label_set_text_fmt(app_lv_ui_res_local->data_cal_unit,   "Calories");
        lv_label_set_text_fmt(app_lv_ui_res_local->data_step_cur,   "%d",  app_lv_ui_data_daily_presenter.get_cur_step());
        lv_label_set_text_fmt(app_lv_ui_res_local->data_step_tar,   "/%d", app_lv_ui_data_daily_presenter.get_tar_step());
        lv_label_set_text_fmt(app_lv_ui_res_local->data_step_unit,  "Steps");
        lv_label_set_text_fmt(app_lv_ui_res_local->data_act_cur,    "%d",  app_lv_ui_data_daily_presenter.get_cur_walk_cnt());
        lv_label_set_text_fmt(app_lv_ui_res_local->data_act_tar,    "/%d", app_lv_ui_data_daily_presenter.get_tar_walk_cnt());
        lv_label_set_text_fmt(app_lv_ui_res_local->data_act_unit,   "Hours");
        
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_data_daily_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_ui_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Data Daily");
        /* 数据集合 */
        lv_obj_t *data_set = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_ui_style_object(data_set);
        lv_obj_set_size(data_set, LV_HOR_RES, LV_VER_RES - 60);
        lv_obj_align_to(data_set, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 三环每日数据 */
        app_lv_ui_res_local->arc_cal = app_lv_ui_style_arc(data_set, 120, 10, LV_PALETTE_RED, 0, 100, 0, 360, 270);
        lv_obj_align(app_lv_ui_res_local->arc_cal, LV_ALIGN_TOP_LEFT, 10, 10);
        app_lv_ui_res_local->arc_step = app_lv_ui_style_arc(data_set, 100, 10, LV_PALETTE_GREEN, 0, 100, 0, 360, 270);
        lv_obj_align(app_lv_ui_res_local->arc_step, LV_ALIGN_TOP_LEFT, 20, 20);
        app_lv_ui_res_local->arc_walk_cnt = app_lv_ui_style_arc(data_set, 80, 10, LV_PALETTE_BLUE, 0, 100, 0, 360, 270);
        lv_obj_align(app_lv_ui_res_local->arc_walk_cnt, LV_ALIGN_TOP_LEFT, 30, 30);
        /* 三环数据集合 */
        lv_obj_t *data_set_sub = lv_obj_create(data_set);
        app_lv_ui_style_object(data_set_sub);
        lv_obj_set_size(data_set_sub, LV_HOR_RES, 90);
        lv_obj_align(data_set_sub, LV_ALIGN_TOP_MID, 0, 150);
        /* 三环数据 */
        app_lv_ui_res_local->data_cal_cur  = app_lv_ui_style_label_large(data_set_sub);
        app_lv_ui_res_local->data_cal_tar  = app_lv_ui_style_label(data_set_sub);
        app_lv_ui_res_local->data_cal_unit = app_lv_ui_style_label(data_set_sub);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_cal_cur,  lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_cal_tar,  lv_palette_darken(LV_PALETTE_RED, 4), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_cal_unit, lv_palette_darken(LV_PALETTE_RED, 4), 0);
        lv_obj_align(app_lv_ui_res_local->data_cal_cur, LV_ALIGN_CENTER, 20 - LV_HOR_RES / 2, 10 - 90 / 2);
        lv_obj_align_to(app_lv_ui_res_local->data_cal_tar, app_lv_ui_res_local->data_cal_cur, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_obj_align_to(app_lv_ui_res_local->data_cal_unit, app_lv_ui_res_local->data_cal_tar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        app_lv_ui_res_local->data_step_cur  = app_lv_ui_style_label_large(data_set_sub);
        app_lv_ui_res_local->data_step_tar  = app_lv_ui_style_label(data_set_sub);
        app_lv_ui_res_local->data_step_unit = app_lv_ui_style_label(data_set_sub);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_step_cur,  lv_palette_main(LV_PALETTE_GREEN), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_step_tar,  lv_palette_darken(LV_PALETTE_GREEN, 4), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_step_unit, lv_palette_darken(LV_PALETTE_GREEN, 4), 0);
        lv_obj_align(app_lv_ui_res_local->data_step_cur, LV_ALIGN_CENTER, 20 - LV_HOR_RES / 2, 30 - 90 / 2);
        lv_obj_align_to(app_lv_ui_res_local->data_step_tar, app_lv_ui_res_local->data_step_cur, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_obj_align_to(app_lv_ui_res_local->data_step_unit, app_lv_ui_res_local->data_step_tar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        app_lv_ui_res_local->data_act_cur  = app_lv_ui_style_label_large(data_set_sub);
        app_lv_ui_res_local->data_act_tar  = app_lv_ui_style_label(data_set_sub);
        app_lv_ui_res_local->data_act_unit = app_lv_ui_style_label(data_set_sub);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_act_cur,  lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_act_tar,  lv_palette_darken(LV_PALETTE_BLUE, 4), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->data_act_unit, lv_palette_darken(LV_PALETTE_BLUE, 4), 0);
        lv_obj_align(app_lv_ui_res_local->data_act_cur, LV_ALIGN_CENTER, 20 - LV_HOR_RES / 2, 50 - 90 / 2);
        lv_obj_align_to(app_lv_ui_res_local->data_act_tar, app_lv_ui_res_local->data_act_cur, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_obj_align_to(app_lv_ui_res_local->data_act_unit, app_lv_ui_res_local->data_act_tar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        
        
        
        
        
        
        /* 初始化显示动画 */
        app_lv_ui_style_object_anim(app_lv_ui_res_local->scene,
                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
    app_lv_scene_data_daily.self = app_lv_ui_res_local == NULL ? NULL :
                                   app_lv_ui_res_local->scene;
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_data_daily_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_data_daily.self = app_lv_ui_res_local == NULL ? NULL :
                                   app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_data_daily = {
    /* 场景资源节点 */
    .show = app_lv_ui_data_daily_show,
    .hide = app_lv_ui_data_daily_hide,
};