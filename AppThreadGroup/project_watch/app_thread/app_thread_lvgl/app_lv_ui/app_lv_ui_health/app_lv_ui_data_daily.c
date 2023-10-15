
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t  anim;
    lv_obj_t  *scene;
    lv_font_t *font_24;
    lv_font_t *font_36;
    /* page1 */
    lv_obj_t *arc_cal;
    lv_obj_t *arc_step;
    lv_obj_t *arc_walk_cnt;
    /* page2 */
    lv_obj_t  *data_cal_spans;
    lv_span_t *data_cal_cur;
    lv_span_t *data_cal_tar;
    lv_span_t *data_cal_unit;
    lv_chart_series_t *chart_cal_ser;
    lv_obj_t  *chart_cal_lb;
    lv_obj_t  *chart_cal_rb;
    lv_obj_t  *chart_cal;
    /*  */
    lv_obj_t  *data_step_spans;
    lv_span_t *data_step_cur;
    lv_span_t *data_step_tar;
    lv_span_t *data_step_unit;
    lv_chart_series_t *chart_step_ser;
    lv_obj_t  *chart_step_lb;
    lv_obj_t  *chart_step_rb;
    lv_obj_t  *chart_step;
    /*  */
    lv_obj_t  *data_act_spans;
    lv_span_t *data_act_cur;
    lv_span_t *data_act_tar;
    lv_span_t *data_act_unit;
    lv_chart_series_t *chart_act_ser;
    lv_obj_t  *chart_act_lb;
    lv_obj_t  *chart_act_rb;
    lv_obj_t  *chart_act;
    /*  */
} *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 数据资源刷新 */
    if (value == 0) {
        /* 三环数据刷新 */
        float f_val_cal =       (float)app_lv_ui_presenter_data_daily.get_cur_cal() /
                                (float)app_lv_ui_presenter_data_daily.get_tar_cal() + 0.005;
        float f_val_step =      (float)app_lv_ui_presenter_data_daily.get_cur_step() /
                                (float)app_lv_ui_presenter_data_daily.get_tar_step() + 0.005;
        float f_val_walk_cnt =  (float)app_lv_ui_presenter_data_daily.get_cur_walk_cnt() /
                                (float)app_lv_ui_presenter_data_daily.get_tar_walk_cnt() + 0.005;
        int16_t val_cal        = (f_val_cal         <= 0.999) ? (100.0 * f_val_cal)         : 100;
        int16_t val_step       = (f_val_step        <= 0.999) ? (100.0 * f_val_step)        : 100;
        int16_t val_walk_cnt   = (f_val_walk_cnt    <= 0.999) ? (100.0 * f_val_walk_cnt)    : 100;
        lv_arc_set_value(app_lv_ui_res_local->arc_cal, val_cal);
        lv_arc_set_value(app_lv_ui_res_local->arc_step, val_step);
        lv_arc_set_value(app_lv_ui_res_local->arc_walk_cnt, val_walk_cnt);
        /* 三环数据显示 */
        char data_str1[20] = {0}, data_str2[20] = {0}, data_str3[20] = {0};
        sprintf(data_str1, "%d",  app_lv_ui_presenter_data_daily.get_cur_cal());
        sprintf(data_str2, "/%d", app_lv_ui_presenter_data_daily.get_tar_cal());
        sprintf(data_str3, "%s",  "Calories");
        lv_span_set_text(app_lv_ui_res_local->data_cal_cur,  data_str1);
        lv_span_set_text(app_lv_ui_res_local->data_cal_tar,  data_str2);
        lv_span_set_text(app_lv_ui_res_local->data_cal_unit, data_str3);
        sprintf(data_str1, "%d",  app_lv_ui_presenter_data_daily.get_cur_step());
        sprintf(data_str2, "/%d", app_lv_ui_presenter_data_daily.get_tar_step());
        sprintf(data_str3, "%s",  "Steps");
        lv_span_set_text(app_lv_ui_res_local->data_step_cur,  data_str1);
        lv_span_set_text(app_lv_ui_res_local->data_step_tar,  data_str2);
        lv_span_set_text(app_lv_ui_res_local->data_step_unit, data_str3);
        sprintf(data_str1, "%d",  app_lv_ui_presenter_data_daily.get_cur_walk_cnt());
        sprintf(data_str2, "/%d", app_lv_ui_presenter_data_daily.get_tar_walk_cnt());
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
        table_max = app_lv_ui_presenter_data_daily.get_tab_step_max();
        app_lv_ui_presenter_data_daily.get_tab_step(table);
        for (uint32_t idx = 0; idx < 24; idx++)
            lv_chart_set_next_value(app_lv_ui_res_local->chart_step, app_lv_ui_res_local->chart_step_ser, table[idx]);
        table_max = app_lv_ui_presenter_data_daily.get_tab_cal_max();
        app_lv_ui_presenter_data_daily.get_tab_cal(table);
        for (uint32_t idx = 0; idx < 24; idx++)
            lv_chart_set_next_value(app_lv_ui_res_local->chart_cal, app_lv_ui_res_local->chart_cal_ser, table[idx]);
        table_max = app_lv_ui_presenter_data_daily.get_tab_act_max();
        app_lv_ui_presenter_data_daily.get_tab_act(table);
        for (uint32_t idx = 0; idx < 24; idx++)
            lv_chart_set_next_value(app_lv_ui_res_local->chart_act, app_lv_ui_res_local->chart_act_ser, table[idx]);
        /* 三图表文本刷新 */
        lv_label_set_text(app_lv_ui_res_local->chart_step_lb, app_lv_ui_presenter_clock.is_24() ? "00:00" : "12:00 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_step_rb, app_lv_ui_presenter_clock.is_24() ? "23:59" : "11:59 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_cal_lb,  app_lv_ui_presenter_clock.is_24() ? "00:00" : "12:00 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_cal_rb,  app_lv_ui_presenter_clock.is_24() ? "23:59" : "11:59 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_act_lb,  app_lv_ui_presenter_clock.is_24() ? "00:00" : "12:00 AM");
        lv_label_set_text(app_lv_ui_res_local->chart_act_rb,  app_lv_ui_presenter_clock.is_24() ? "23:59" : "11:59 AM");
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
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 获取字体 */
        app_lv_ui_res_local->font_24 = app_lv_ui_font(24);
        app_lv_ui_res_local->font_36 = app_lv_ui_font(36);
        /* 滑动界面 */
        lv_obj_t *list = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
        lv_obj_center(list);
        /* (第一页)三环每日数据 */
        lv_obj_t *page1 = lv_obj_create(list);
        app_lv_style_object(page1);
        lv_obj_set_size(page1, LV_HOR_RES, LV_VER_RES);
        lv_obj_center(page1);
        /* 三环每日数据 */
        app_lv_ui_res_local->arc_cal = lv_arc_create(page1);
        app_lv_style_object(app_lv_ui_res_local->arc_cal);
        lv_obj_remove_style(app_lv_ui_res_local->arc_cal, NULL, LV_PART_KNOB);
        lv_obj_clear_flag(app_lv_ui_res_local->arc_cal, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->arc_cal, LV_OPA_TRANSP, 0);
        lv_obj_set_style_arc_width(app_lv_ui_res_local->arc_cal, 15, 0);
        lv_obj_set_style_arc_width(app_lv_ui_res_local->arc_cal, 15, LV_PART_INDICATOR);
        lv_arc_set_mode(app_lv_ui_res_local->arc_cal, LV_ARC_MODE_NORMAL);
        lv_arc_set_range(app_lv_ui_res_local->arc_cal, 0, 100);
        lv_arc_set_bg_angles(app_lv_ui_res_local->arc_cal, 0 + 15, 360 - 15);
        lv_arc_set_rotation(app_lv_ui_res_local->arc_cal, 180);
        lv_obj_center(app_lv_ui_res_local->arc_cal);
        app_lv_ui_res_local->arc_step = lv_arc_create(page1);
        app_lv_style_object(app_lv_ui_res_local->arc_step);
        lv_obj_remove_style(app_lv_ui_res_local->arc_step, NULL, LV_PART_KNOB);
        lv_obj_clear_flag(app_lv_ui_res_local->arc_step, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->arc_step, LV_OPA_TRANSP, 0);
        lv_obj_set_style_arc_width(app_lv_ui_res_local->arc_step, 15, 0);
        lv_obj_set_style_arc_width(app_lv_ui_res_local->arc_step, 15, LV_PART_INDICATOR);
        lv_arc_set_mode(app_lv_ui_res_local->arc_step, LV_ARC_MODE_NORMAL);
        lv_arc_set_range(app_lv_ui_res_local->arc_step, 0, 100);
        lv_arc_set_bg_angles(app_lv_ui_res_local->arc_step, 0 + 15, 360 - 15);
        lv_arc_set_rotation(app_lv_ui_res_local->arc_step, 180);
        lv_obj_center(app_lv_ui_res_local->arc_step);
        app_lv_ui_res_local->arc_walk_cnt = lv_arc_create(page1);
        app_lv_style_object(app_lv_ui_res_local->arc_walk_cnt);
        lv_obj_remove_style(app_lv_ui_res_local->arc_walk_cnt, NULL, LV_PART_KNOB);
        lv_obj_clear_flag(app_lv_ui_res_local->arc_walk_cnt, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->arc_walk_cnt, LV_OPA_TRANSP, 0);
        lv_obj_set_style_arc_width(app_lv_ui_res_local->arc_walk_cnt, 15, 0);
        lv_obj_set_style_arc_width(app_lv_ui_res_local->arc_walk_cnt, 15, LV_PART_INDICATOR);
        lv_arc_set_mode(app_lv_ui_res_local->arc_walk_cnt, LV_ARC_MODE_NORMAL);
        lv_arc_set_range(app_lv_ui_res_local->arc_walk_cnt, 0, 100);
        lv_arc_set_bg_angles(app_lv_ui_res_local->arc_walk_cnt, 0 + 15, 360 - 15);
        lv_arc_set_rotation(app_lv_ui_res_local->arc_walk_cnt, 180);
        lv_obj_center(app_lv_ui_res_local->arc_walk_cnt);
        /* 边界 */
        lv_coord_t arc_radius = LV_MIN(LV_HOR_RES - app_lv_style_hor_pct(10), LV_VER_RES - app_lv_style_ver_pct(10));
        /*  */
        lv_obj_set_size(app_lv_ui_res_local->arc_cal, arc_radius - 50 * 0, arc_radius - 50 * 0);
        lv_obj_set_style_arc_color(app_lv_ui_res_local->arc_cal, lv_palette_darken(LV_PALETTE_RED, 4),  0);
        lv_obj_set_style_arc_color(app_lv_ui_res_local->arc_cal, lv_palette_lighten(LV_PALETTE_RED, 1), LV_PART_INDICATOR);
        /*  */
        lv_obj_set_size(app_lv_ui_res_local->arc_step, arc_radius - 50 * 1, arc_radius - 50 * 1);
        lv_obj_set_style_arc_color(app_lv_ui_res_local->arc_step, lv_palette_darken(LV_PALETTE_GREEN, 4),  0);
        lv_obj_set_style_arc_color(app_lv_ui_res_local->arc_step, lv_palette_lighten(LV_PALETTE_GREEN, 1), LV_PART_INDICATOR);
        /*  */
        lv_obj_set_size(app_lv_ui_res_local->arc_walk_cnt, arc_radius - 50 * 2, arc_radius - 50 * 2);
        lv_obj_set_style_arc_color(app_lv_ui_res_local->arc_walk_cnt, lv_palette_darken(LV_PALETTE_BLUE, 4),  0);
        lv_obj_set_style_arc_color(app_lv_ui_res_local->arc_walk_cnt, lv_palette_lighten(LV_PALETTE_BLUE, 1), LV_PART_INDICATOR);
        /* (第一页)三图表数据 */
        lv_obj_t *page2 = lv_obj_create(list);
        app_lv_style_object(page2);
        lv_obj_set_size(page2, LV_HOR_RES, app_lv_style_ver_pct(160));
        lv_obj_center(page2);
        /* 三图表数据 */
        app_lv_ui_res_local->chart_step = lv_chart_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_step);
        lv_obj_set_size(app_lv_ui_res_local->chart_step, app_lv_style_hor_pct(90), app_lv_style_ver_pct(30));
        lv_obj_add_event_cb(app_lv_ui_res_local->chart_step, app_lv_event_ui_chart_fade_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_style_size(app_lv_ui_res_local->chart_step, 1, LV_PART_INDICATOR);
        lv_chart_set_type(app_lv_ui_res_local->chart_step, LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(app_lv_ui_res_local->chart_step, 24);
        lv_chart_set_div_line_count(app_lv_ui_res_local->chart_step, 5, 0);
        lv_chart_set_update_mode(app_lv_ui_res_local->chart_step, LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_obj_align(app_lv_ui_res_local->chart_step, LV_ALIGN_CENTER, 0, -app_lv_style_ver_pct(50));
        app_lv_ui_res_local->chart_cal = lv_chart_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_cal);
        lv_obj_set_size(app_lv_ui_res_local->chart_cal, app_lv_style_hor_pct(90), app_lv_style_ver_pct(30));
        lv_obj_add_event_cb(app_lv_ui_res_local->chart_cal, app_lv_event_ui_chart_fade_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_style_size(app_lv_ui_res_local->chart_cal, 1, LV_PART_INDICATOR);
        lv_chart_set_type(app_lv_ui_res_local->chart_cal, LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(app_lv_ui_res_local->chart_cal, 24);
        lv_chart_set_div_line_count(app_lv_ui_res_local->chart_cal, 5, 0);
        lv_chart_set_update_mode(app_lv_ui_res_local->chart_cal, LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_obj_align(app_lv_ui_res_local->chart_cal, LV_ALIGN_CENTER, 0, 0);
        app_lv_ui_res_local->chart_act = lv_chart_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_act);
        lv_obj_set_size(app_lv_ui_res_local->chart_act, app_lv_style_hor_pct(90), app_lv_style_ver_pct(30));
        lv_obj_add_event_cb(app_lv_ui_res_local->chart_act, app_lv_event_ui_chart_fade_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_style_size(app_lv_ui_res_local->chart_act, 1, LV_PART_INDICATOR);
        lv_chart_set_type(app_lv_ui_res_local->chart_act, LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(app_lv_ui_res_local->chart_act, 24);
        lv_chart_set_div_line_count(app_lv_ui_res_local->chart_act, 5, 0);
        lv_chart_set_update_mode(app_lv_ui_res_local->chart_act, LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_obj_align(app_lv_ui_res_local->chart_act, LV_ALIGN_CENTER, 0,  +app_lv_style_ver_pct(50));
        app_lv_ui_res_local->chart_step_ser = lv_chart_add_series(app_lv_ui_res_local->chart_step, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
        app_lv_ui_res_local->chart_cal_ser  = lv_chart_add_series(app_lv_ui_res_local->chart_cal,  lv_palette_main(LV_PALETTE_RED),   LV_CHART_AXIS_PRIMARY_Y);
        app_lv_ui_res_local->chart_act_ser  = lv_chart_add_series(app_lv_ui_res_local->chart_act,  lv_palette_main(LV_PALETTE_BLUE),  LV_CHART_AXIS_PRIMARY_Y);
        /* 三图表数据 */
        app_lv_ui_res_local->data_cal_spans = lv_spangroup_create(page2);
        app_lv_style_object(app_lv_ui_res_local->data_cal_spans);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->data_cal_spans, LV_OPA_TRANSP, 0);
        lv_spangroup_set_align(app_lv_ui_res_local->data_cal_spans, LV_TEXT_ALIGN_LEFT);
        lv_spangroup_set_mode(app_lv_ui_res_local->data_cal_spans, LV_SPAN_MODE_EXPAND);
        lv_spangroup_set_overflow(app_lv_ui_res_local->data_cal_spans, LV_SPAN_OVERFLOW_ELLIPSIS);
        lv_spangroup_set_indent(app_lv_ui_res_local->data_cal_spans, 0);
        lv_obj_set_width(app_lv_ui_res_local->data_cal_spans, LV_HOR_RES);
        app_lv_ui_res_local->data_step_spans = lv_spangroup_create(page2);
        app_lv_style_object(app_lv_ui_res_local->data_step_spans);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->data_step_spans, LV_OPA_TRANSP, 0);
        lv_spangroup_set_align(app_lv_ui_res_local->data_step_spans, LV_TEXT_ALIGN_LEFT);
        lv_spangroup_set_mode(app_lv_ui_res_local->data_step_spans, LV_SPAN_MODE_EXPAND);
        lv_spangroup_set_overflow(app_lv_ui_res_local->data_step_spans, LV_SPAN_OVERFLOW_ELLIPSIS);
        lv_spangroup_set_indent(app_lv_ui_res_local->data_step_spans, 0);
        lv_obj_set_width(app_lv_ui_res_local->data_step_spans, LV_HOR_RES);
        app_lv_ui_res_local->data_act_spans = lv_spangroup_create(page2);
        app_lv_style_object(app_lv_ui_res_local->data_act_spans);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->data_act_spans, LV_OPA_TRANSP, 0);
        lv_spangroup_set_align(app_lv_ui_res_local->data_act_spans, LV_TEXT_ALIGN_LEFT);
        lv_spangroup_set_mode(app_lv_ui_res_local->data_act_spans, LV_SPAN_MODE_EXPAND);
        lv_spangroup_set_overflow(app_lv_ui_res_local->data_act_spans, LV_SPAN_OVERFLOW_ELLIPSIS);
        lv_spangroup_set_indent(app_lv_ui_res_local->data_act_spans, 0);
        lv_obj_set_width(app_lv_ui_res_local->data_act_spans, LV_HOR_RES);
        /* 三环数据 */
        app_lv_ui_res_local->data_cal_cur  = lv_spangroup_new_span(app_lv_ui_res_local->data_cal_spans);
        app_lv_ui_res_local->data_cal_tar  = lv_spangroup_new_span(app_lv_ui_res_local->data_cal_spans);
        app_lv_ui_res_local->data_cal_unit = lv_spangroup_new_span(app_lv_ui_res_local->data_cal_spans);
        lv_style_set_text_font(&app_lv_ui_res_local->data_cal_cur->style,   app_lv_ui_res_local->font_36);
        lv_style_set_text_color(&app_lv_ui_res_local->data_cal_cur->style,  lv_palette_main(LV_PALETTE_RED));
        lv_style_set_text_color(&app_lv_ui_res_local->data_cal_tar->style,  lv_palette_darken(LV_PALETTE_RED, 4));
        lv_style_set_text_color(&app_lv_ui_res_local->data_cal_unit->style, lv_palette_darken(LV_PALETTE_RED, 4));
        lv_obj_align_to(app_lv_ui_res_local->data_cal_spans,  app_lv_ui_res_local->chart_cal,  LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        app_lv_ui_res_local->data_step_cur  = lv_spangroup_new_span(app_lv_ui_res_local->data_step_spans);
        app_lv_ui_res_local->data_step_tar  = lv_spangroup_new_span(app_lv_ui_res_local->data_step_spans);
        app_lv_ui_res_local->data_step_unit = lv_spangroup_new_span(app_lv_ui_res_local->data_step_spans);
        lv_style_set_text_font(&app_lv_ui_res_local->data_step_cur->style,   app_lv_ui_res_local->font_36);
        lv_style_set_text_color(&app_lv_ui_res_local->data_step_cur->style,  lv_palette_main(LV_PALETTE_GREEN));
        lv_style_set_text_color(&app_lv_ui_res_local->data_step_tar->style,  lv_palette_darken(LV_PALETTE_GREEN, 4));
        lv_style_set_text_color(&app_lv_ui_res_local->data_step_unit->style, lv_palette_darken(LV_PALETTE_GREEN, 4));
        lv_obj_align_to(app_lv_ui_res_local->data_step_spans,  app_lv_ui_res_local->chart_step, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        app_lv_ui_res_local->data_act_cur  = lv_spangroup_new_span(app_lv_ui_res_local->data_act_spans);
        app_lv_ui_res_local->data_act_tar  = lv_spangroup_new_span(app_lv_ui_res_local->data_act_spans);
        app_lv_ui_res_local->data_act_unit = lv_spangroup_new_span(app_lv_ui_res_local->data_act_spans);
        lv_style_set_text_font(&app_lv_ui_res_local->data_act_cur->style,   app_lv_ui_res_local->font_36);
        lv_style_set_text_color(&app_lv_ui_res_local->data_act_cur->style,  lv_palette_main(LV_PALETTE_GREEN));
        lv_style_set_text_color(&app_lv_ui_res_local->data_act_tar->style,  lv_palette_darken(LV_PALETTE_GREEN, 4));
        lv_style_set_text_color(&app_lv_ui_res_local->data_act_unit->style, lv_palette_darken(LV_PALETTE_GREEN, 4));
        lv_obj_align_to(app_lv_ui_res_local->data_act_spans,  app_lv_ui_res_local->chart_act,  LV_ALIGN_OUT_TOP_LEFT, 0, 0);
        /* 三环图表追加文本信息 */
        app_lv_ui_res_local->chart_step_lb = lv_label_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_step_lb);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->chart_step_lb, LV_OPA_TRANSP, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->chart_step_lb, LV_LABEL_LONG_SCROLL_CIRCULAR);
        app_lv_ui_res_local->chart_step_rb = lv_label_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_step_rb);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->chart_step_rb, LV_OPA_TRANSP, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->chart_step_rb, LV_LABEL_LONG_SCROLL_CIRCULAR);
        app_lv_ui_res_local->chart_cal_lb = lv_label_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_cal_lb);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->chart_cal_lb, LV_OPA_TRANSP, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->chart_cal_lb, LV_LABEL_LONG_SCROLL_CIRCULAR);
        app_lv_ui_res_local->chart_cal_rb = lv_label_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_cal_rb);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->chart_cal_rb, LV_OPA_TRANSP, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->chart_cal_rb, LV_LABEL_LONG_SCROLL_CIRCULAR);
        app_lv_ui_res_local->chart_act_lb = lv_label_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_act_lb);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->chart_act_lb, LV_OPA_TRANSP, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->chart_act_lb, LV_LABEL_LONG_SCROLL_CIRCULAR);
        app_lv_ui_res_local->chart_act_rb = lv_label_create(page2);
        app_lv_style_object(app_lv_ui_res_local->chart_act_rb);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->chart_act_rb, LV_OPA_TRANSP, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->chart_act_rb, LV_LABEL_LONG_SCROLL_CIRCULAR);
        /* ...可以继续添加... */
        /* ...   ... */
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
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_data_daily = {
    /* 场景资源节点 */
    .show = app_lv_ui_data_daily_show,
    .hide = app_lv_ui_data_daily_hide,
};
