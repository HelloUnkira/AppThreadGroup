
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_multi_font.h"
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
    lv_obj_t  *data_cal_spans;
    lv_span_t *data_cal_cur;
    lv_span_t *data_cal_tar;
    lv_span_t *data_cal_unit;
    lv_obj_t  *data_step_spans;
    lv_span_t *data_step_cur;
    lv_span_t *data_step_tar;
    lv_span_t *data_step_unit;
    lv_obj_t  *data_act_spans;
    lv_span_t *data_act_cur;
    lv_span_t *data_act_tar;
    lv_span_t *data_act_unit;
    lv_obj_t *chart_step;
    lv_chart_series_t *chart_step_ser;
    lv_obj_t *chart_step_lb;
    lv_obj_t *chart_step_rb;
    lv_obj_t *chart_cal;
    lv_chart_series_t *chart_cal_ser;
    lv_obj_t *chart_cal_lb;
    lv_obj_t *chart_cal_rb;
    lv_obj_t *chart_act;
    lv_chart_series_t *chart_act_ser;
    lv_obj_t *chart_act_lb;
    lv_obj_t *chart_act_rb;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static void app_lv_ui_local_style_span_data(lv_obj_t *spans, lv_span_t **cur, lv_span_t **tar, lv_span_t **unit, lv_palette_t palette)
{
    *cur  = lv_spangroup_new_span(spans);
    *tar  = lv_spangroup_new_span(spans);
    *unit = lv_spangroup_new_span(spans);
    lv_style_set_text_font(&(*cur)->style,   app_lv_multi_font(app_lv_multi_font_28));
    lv_style_set_text_font(&(*tar)->style,   app_lv_multi_font(app_lv_multi_font_24));
    lv_style_set_text_font(&(*unit)->style,  app_lv_multi_font(app_lv_multi_font_20));
    lv_style_set_text_color(&(*cur)->style,  lv_palette_main(palette));
    lv_style_set_text_color(&(*tar)->style,  lv_palette_darken(palette, 4));
    lv_style_set_text_color(&(*unit)->style, lv_palette_darken(palette, 4));
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 左上角系统时钟更新 */
    char format_clock_1[20] = {0};
    app_lv_ui_clock_presenter.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
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
        char data_str1[20] = {0}, data_str2[20] = {0}, data_str3[20] = {0};
        sprintf(data_str1, "%d",  app_lv_ui_data_daily_presenter.get_cur_cal());
        sprintf(data_str2, "/%d", app_lv_ui_data_daily_presenter.get_tar_cal());
        sprintf(data_str3, "%s",  "Calories");
        lv_span_set_text(app_lv_ui_res_local->data_cal_cur,  data_str1);
        lv_span_set_text(app_lv_ui_res_local->data_cal_tar,  data_str2);
        lv_span_set_text(app_lv_ui_res_local->data_cal_unit, data_str3);
        sprintf(data_str1, "%d",  app_lv_ui_data_daily_presenter.get_cur_step());
        sprintf(data_str2, "/%d", app_lv_ui_data_daily_presenter.get_tar_step());
        sprintf(data_str3, "%s",  "Steps");
        lv_span_set_text(app_lv_ui_res_local->data_step_cur,  data_str1);
        lv_span_set_text(app_lv_ui_res_local->data_step_tar,  data_str2);
        lv_span_set_text(app_lv_ui_res_local->data_step_unit, data_str3);
        sprintf(data_str1, "%d",  app_lv_ui_data_daily_presenter.get_cur_walk_cnt());
        sprintf(data_str2, "/%d", app_lv_ui_data_daily_presenter.get_tar_walk_cnt());
        sprintf(data_str3, "%s",  "Hours");
        lv_span_set_text(app_lv_ui_res_local->data_act_cur,  data_str1);
        lv_span_set_text(app_lv_ui_res_local->data_act_tar,  data_str2);
        lv_span_set_text(app_lv_ui_res_local->data_act_unit, data_str3);
        lv_spangroup_refr_mode(app_lv_ui_res_local->data_cal_spans);
        lv_spangroup_refr_mode(app_lv_ui_res_local->data_step_spans);
        lv_spangroup_refr_mode(app_lv_ui_res_local->data_act_spans);
        /* 三图表数据表示 */
        uint32_t table_max = 0;
        uint32_t table[24] = {0};
        table_max = app_lv_ui_data_daily_presenter.get_tab_step_max();
        app_lv_ui_data_daily_presenter.get_tab_step(table);
        for (uint32_t idx = 0; idx < 24; idx++)
            lv_chart_set_next_value(app_lv_ui_res_local->chart_step, app_lv_ui_res_local->chart_step_ser, table[idx]);
        table_max = app_lv_ui_data_daily_presenter.get_tab_cal_max();
        app_lv_ui_data_daily_presenter.get_tab_cal(table);
        for (uint32_t idx = 0; idx < 24; idx++)
            lv_chart_set_next_value(app_lv_ui_res_local->chart_cal, app_lv_ui_res_local->chart_cal_ser, table[idx]);
        table_max = app_lv_ui_data_daily_presenter.get_tab_act_max();
        app_lv_ui_data_daily_presenter.get_tab_act(table);
        for (uint32_t idx = 0; idx < 24; idx++)
            lv_chart_set_next_value(app_lv_ui_res_local->chart_act, app_lv_ui_res_local->chart_act_ser, table[idx]);
        /* 三图表文本刷新 */
        lv_label_set_text(app_lv_ui_res_local->chart_step_lb, app_lv_ui_clock_presenter.is_24() ? "00:00" : "12:00 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_step_rb, app_lv_ui_clock_presenter.is_24() ? "23:59" : "11:59 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_cal_lb,  app_lv_ui_clock_presenter.is_24() ? "00:00" : "12:00 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_cal_rb,  app_lv_ui_clock_presenter.is_24() ? "23:59" : "11:59 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_act_lb,  app_lv_ui_clock_presenter.is_24() ? "00:00" : "12:00 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_act_rb,  app_lv_ui_clock_presenter.is_24() ? "23:59" : "11:59 AM");
        lv_obj_align_to(app_lv_ui_res_local->chart_step_lb, app_lv_ui_res_local->chart_step, LV_ALIGN_OUT_BOTTOM_LEFT,  0, 5);
        lv_obj_align_to(app_lv_ui_res_local->chart_step_rb, app_lv_ui_res_local->chart_step, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
        lv_obj_align_to(app_lv_ui_res_local->chart_cal_lb, app_lv_ui_res_local->chart_cal, LV_ALIGN_OUT_BOTTOM_LEFT,    0, 5);
        lv_obj_align_to(app_lv_ui_res_local->chart_cal_rb, app_lv_ui_res_local->chart_cal, LV_ALIGN_OUT_BOTTOM_RIGHT,   0, 5);
        lv_obj_align_to(app_lv_ui_res_local->chart_act_lb, app_lv_ui_res_local->chart_act, LV_ALIGN_OUT_BOTTOM_LEFT,    0, 5);
        lv_obj_align_to(app_lv_ui_res_local->chart_act_rb, app_lv_ui_res_local->chart_act, LV_ALIGN_OUT_BOTTOM_RIGHT,   0, 5);
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
        app_lv_ui_res_local->scene = app_lv_style_scene();
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Data Daily");
        /* 数据集合 */
        lv_obj_t *data_set = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(data_set);
        lv_obj_set_size(data_set, LV_HOR_RES, app_lv_style_ver_pct(90));
        lv_obj_align_to(data_set, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 三环每日数据 */
        app_lv_ui_res_local->arc_cal = app_lv_style_arc(data_set, 80, 15, LV_PALETTE_RED, 0, 100, 0, 360, 270);
        lv_obj_align(app_lv_ui_res_local->arc_cal, LV_ALIGN_TOP_LEFT, 40, 40);
        app_lv_ui_res_local->arc_step = app_lv_style_arc(data_set, 140, 15, LV_PALETTE_GREEN, 0, 100, 0 + 15, 180 - 15, 180);
        lv_obj_align(app_lv_ui_res_local->arc_step, LV_ALIGN_TOP_LEFT, 10, 10);
        app_lv_ui_res_local->arc_walk_cnt = app_lv_style_arc(data_set, 140, 15, LV_PALETTE_BLUE, 0, 100, 0 + 15, 180 - 15, 0);
        lv_obj_align(app_lv_ui_res_local->arc_walk_cnt, LV_ALIGN_TOP_LEFT, 10, 10);
        /* 三图表数据集合 */
        lv_obj_t *chart_set = lv_obj_create(data_set);
        app_lv_style_object(chart_set);
        lv_obj_set_size(chart_set, LV_HOR_RES, app_lv_style_ver_pct(160));
        lv_obj_align(chart_set, LV_ALIGN_TOP_MID, 0, app_lv_style_ver_pct(50));
        /* 三图表数据 */
        app_lv_ui_res_local->chart_step = app_lv_style_fade_chart(chart_set);
        app_lv_ui_res_local->chart_step_ser = lv_chart_add_series(app_lv_ui_res_local->chart_step, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
        lv_obj_align(app_lv_ui_res_local->chart_step, LV_ALIGN_CENTER, 0, -app_lv_style_ver_pct(50));
        app_lv_ui_res_local->chart_cal = app_lv_style_fade_chart(chart_set);
        app_lv_ui_res_local->chart_cal_ser = lv_chart_add_series(app_lv_ui_res_local->chart_cal, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
        lv_obj_align(app_lv_ui_res_local->chart_cal, LV_ALIGN_CENTER, 0, 0);
        app_lv_ui_res_local->chart_act = app_lv_style_fade_chart(chart_set);
        app_lv_ui_res_local->chart_act_ser = lv_chart_add_series(app_lv_ui_res_local->chart_act, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
        lv_obj_align(app_lv_ui_res_local->chart_act, LV_ALIGN_CENTER, 0,  +app_lv_style_ver_pct(50));
        /* 三环数据 */
        app_lv_ui_res_local->data_cal_spans  = app_lv_style_spangroup(chart_set);
        app_lv_ui_res_local->data_step_spans = app_lv_style_spangroup(chart_set);
        app_lv_ui_res_local->data_act_spans  = app_lv_style_spangroup(chart_set);
        lv_obj_set_size(app_lv_ui_res_local->data_cal_spans,  LV_HOR_RES, app_lv_style_ver_pct(10));
        lv_obj_set_size(app_lv_ui_res_local->data_step_spans, LV_HOR_RES, app_lv_style_ver_pct(10));
        lv_obj_set_size(app_lv_ui_res_local->data_act_spans,  LV_HOR_RES, app_lv_style_ver_pct(10));
        lv_obj_align_to(app_lv_ui_res_local->data_cal_spans,  app_lv_ui_res_local->chart_cal,  LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        lv_obj_align_to(app_lv_ui_res_local->data_step_spans, app_lv_ui_res_local->chart_step, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        lv_obj_align_to(app_lv_ui_res_local->data_act_spans,  app_lv_ui_res_local->chart_act,  LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        app_lv_ui_local_style_span_data(app_lv_ui_res_local->data_cal_spans,
                                       &app_lv_ui_res_local->data_cal_cur,
                                       &app_lv_ui_res_local->data_cal_tar,
                                       &app_lv_ui_res_local->data_cal_unit, LV_PALETTE_RED);
        app_lv_ui_local_style_span_data(app_lv_ui_res_local->data_step_spans,
                                       &app_lv_ui_res_local->data_step_cur,
                                       &app_lv_ui_res_local->data_step_tar,
                                       &app_lv_ui_res_local->data_step_unit, LV_PALETTE_GREEN);
        app_lv_ui_local_style_span_data(app_lv_ui_res_local->data_act_spans,
                                       &app_lv_ui_res_local->data_act_cur,
                                       &app_lv_ui_res_local->data_act_tar,
                                       &app_lv_ui_res_local->data_act_unit, LV_PALETTE_GREEN);
        /* 三环图表追加文本信息 */
        app_lv_ui_res_local->chart_step_lb = app_lv_style_label(chart_set);
        app_lv_ui_res_local->chart_step_rb = app_lv_style_label(chart_set);
        app_lv_ui_res_local->chart_cal_lb  = app_lv_style_label(chart_set);
        app_lv_ui_res_local->chart_cal_rb  = app_lv_style_label(chart_set);
        app_lv_ui_res_local->chart_act_lb  = app_lv_style_label(chart_set);
        app_lv_ui_res_local->chart_act_rb  = app_lv_style_label(chart_set);
        lv_obj_set_height(app_lv_ui_res_local->chart_step_lb, app_lv_style_ver_pct(10));
        lv_obj_set_height(app_lv_ui_res_local->chart_step_rb, app_lv_style_ver_pct(10));
        lv_obj_set_height(app_lv_ui_res_local->chart_cal_lb,  app_lv_style_ver_pct(10));
        lv_obj_set_height(app_lv_ui_res_local->chart_cal_rb,  app_lv_style_ver_pct(10));
        lv_obj_set_height(app_lv_ui_res_local->chart_act_lb,  app_lv_style_ver_pct(10));
        lv_obj_set_height(app_lv_ui_res_local->chart_act_rb,  app_lv_style_ver_pct(10));
        /* ...可以继续添加... */
        /* ...这里略过... */
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
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
}

app_lv_scene_t app_lv_ui_data_daily = {
    /* 场景资源节点 */
    .show = app_lv_ui_data_daily_show,
    .hide = app_lv_ui_data_daily_hide,
};
