
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_util.h"
#include "app_lv_ui_scene_set.h"
#include "app_lv_ui_test_clock_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *label_dtime;
    lv_obj_t *roller_h;
    lv_obj_t *roller_m;
    lv_obj_t *roller_s;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static const char  app_lv_ui_res_0_23[] = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
static const char  app_lv_ui_res_0_59[] = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";
static const char *app_lv_ui_res_week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

#define app_lv_ui_res_roller_sel_font   &lv_font_montserrat_22

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    lv_label_set_text_fmt(app_lv_ui_res_local->label_dtime,
                          "%u-%u-%u %u:%u:%u %s",
                          app_lv_ui_test_clock_presenter.get_year(),
                          app_lv_ui_test_clock_presenter.get_month(),
                          app_lv_ui_test_clock_presenter.get_day(),
                          app_lv_ui_test_clock_presenter.get_hour(),
                          app_lv_ui_test_clock_presenter.get_minute(),
                          app_lv_ui_test_clock_presenter.get_second(),
                          app_lv_ui_res_week[app_lv_ui_test_clock_presenter.get_week()]);
    
    lv_roller_set_selected(app_lv_ui_res_local->roller_h,   app_lv_ui_test_clock_presenter.get_hour(),   LV_ANIM_OFF);
    lv_roller_set_selected(app_lv_ui_res_local->roller_m, app_lv_ui_test_clock_presenter.get_minute(), LV_ANIM_OFF);
    lv_roller_set_selected(app_lv_ui_res_local->roller_s, app_lv_ui_test_clock_presenter.get_second(), LV_ANIM_OFF);
}

/*@brief 界面显示
 *@brief scene 场景
 */
static void app_lv_ui_test_clock_show(void *scene)
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
        lv_obj_set_size(label, LV_HOR_RES - 20, 40);
        lv_obj_set_style_pad_all(label, 0, 0);
        lv_obj_set_style_bg_color(label,  lv_color_black(), 0);
        lv_obj_set_style_border_side(label, 0, 0);
        lv_obj_set_style_border_width(label, 0, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_line_space(label, 5, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_text_static(label, "System Clock");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        /* 绘制日期 */
        app_lv_ui_res_local->label_dtime = lv_label_create(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->label_dtime, LV_HOR_RES - 20, 40);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->label_dtime, 0, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->label_dtime,  lv_color_black(), 0);
        lv_obj_set_style_border_side(app_lv_ui_res_local->label_dtime, 0, 0);
        lv_obj_set_style_border_width(app_lv_ui_res_local->label_dtime, 0, 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->label_dtime,  lv_color_black(), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->label_dtime, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->label_dtime, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_line_space(app_lv_ui_res_local->label_dtime, 5, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->label_dtime, LV_LABEL_LONG_WRAP);
        lv_obj_align_to(app_lv_ui_res_local->label_dtime, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制日期后三位滚轮 */
        lv_obj_t *roller_set = lv_obj_create(app_lv_ui_res_local->scene);
        lv_obj_set_width(roller_set, LV_HOR_RES);
        lv_obj_set_style_pad_all(roller_set, 0, 0);
        lv_obj_set_style_bg_color(roller_set,  lv_color_black(), 0);
        lv_obj_set_style_border_side(roller_set, 0, 0);
        lv_obj_set_style_border_width(roller_set, 0, 0);
        lv_obj_align(roller_set, LV_ALIGN_CENTER, 0, 0);
        /* 绘制分滚轮,中心对齐 */
        app_lv_ui_res_local->roller_m = lv_roller_create(roller_set);
        lv_obj_set_width(app_lv_ui_res_local->roller_m, 50);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->roller_m, LV_OPA_0, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->roller_m, LV_OPA_0, LV_PART_SELECTED);
        lv_obj_set_style_text_font(app_lv_ui_res_local->roller_m, app_lv_ui_res_roller_sel_font, LV_PART_SELECTED);
        lv_obj_set_style_text_color(app_lv_ui_res_local->roller_m, lv_palette_main(LV_PALETTE_GREEN), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->roller_m, lv_palette_main(LV_PALETTE_GREEN), LV_PART_SELECTED);
        lv_obj_set_style_text_align(app_lv_ui_res_local->roller_m, LV_TEXT_ALIGN_CENTER, 0);
        lv_roller_set_options(app_lv_ui_res_local->roller_m, app_lv_ui_res_0_59, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(app_lv_ui_res_local->roller_m, 3);
        lv_obj_align(app_lv_ui_res_local->roller_m, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->roller_m, app_lv_ui_roller_mask_event_cb, LV_EVENT_ALL, NULL);
        /* 绘制时滚轮,分滚轮左外对齐 */
        app_lv_ui_res_local->roller_h = lv_roller_create(roller_set);
        lv_obj_set_width(app_lv_ui_res_local->roller_h, 50);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->roller_h, LV_OPA_0, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->roller_h, LV_OPA_0, LV_PART_SELECTED);
        lv_obj_set_style_text_font(app_lv_ui_res_local->roller_h, app_lv_ui_res_roller_sel_font, LV_PART_SELECTED);
        lv_obj_set_style_text_color(app_lv_ui_res_local->roller_h, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->roller_h, lv_palette_main(LV_PALETTE_BLUE), LV_PART_SELECTED);
        lv_obj_set_style_text_align(app_lv_ui_res_local->roller_h, LV_TEXT_ALIGN_CENTER, 0);
        lv_roller_set_options(app_lv_ui_res_local->roller_h, app_lv_ui_res_0_23, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(app_lv_ui_res_local->roller_h, 3);
        lv_obj_align_to(app_lv_ui_res_local->roller_h, app_lv_ui_res_local->roller_m, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->roller_h, app_lv_ui_roller_mask_event_cb, LV_EVENT_ALL, NULL);
        /* 绘制秒滚轮,分滚轮右外对齐 */
        app_lv_ui_res_local->roller_s = lv_roller_create(roller_set);
        lv_obj_set_width(app_lv_ui_res_local->roller_s, 50);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->roller_s, LV_OPA_0, 0);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->roller_s, LV_OPA_0, LV_PART_SELECTED);
        lv_obj_set_style_text_font(app_lv_ui_res_local->roller_s, app_lv_ui_res_roller_sel_font, LV_PART_SELECTED);
        lv_obj_set_style_text_color(app_lv_ui_res_local->roller_s, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->roller_s, lv_palette_main(LV_PALETTE_RED), LV_PART_SELECTED);
        lv_obj_set_style_text_align(app_lv_ui_res_local->roller_s, LV_TEXT_ALIGN_CENTER, 0);
        lv_roller_set_options(app_lv_ui_res_local->roller_s, app_lv_ui_res_0_59, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(app_lv_ui_res_local->roller_s, 3);
        lv_obj_align_to(app_lv_ui_res_local->roller_s, app_lv_ui_res_local->roller_m, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->roller_s, app_lv_ui_roller_mask_event_cb, LV_EVENT_ALL, NULL);
        /* 初始化显示动画 */
        lv_anim_init(&app_lv_ui_res_local->anim);
        lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
        lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
        lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 10);
        lv_anim_set_time(&app_lv_ui_res_local->anim, 1000);
        lv_anim_start(&app_lv_ui_res_local->anim);
    }
    app_lv_scene_test_clock.self = app_lv_ui_res_local == NULL ? NULL :
                                   app_lv_ui_res_local->scene;
}

/*@brief 界面隐藏
 *@brief scene 场景
 */
static void app_lv_ui_test_clock_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 场景去除默认事件 */
        app_lv_ui_event_default_clr(app_lv_ui_res_local->scene);
        /* 反初始化场景 */
        lv_obj_remove_event_cb(app_lv_ui_res_local->roller_s, app_lv_ui_roller_mask_event_cb);
        lv_obj_remove_event_cb(app_lv_ui_res_local->roller_h,   app_lv_ui_roller_mask_event_cb);
        lv_obj_remove_event_cb(app_lv_ui_res_local->roller_m, app_lv_ui_roller_mask_event_cb);
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_test_clock.self = app_lv_ui_res_local == NULL ? NULL :
                                   app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_test_clock = {
    /* 场景资源节点 */
    .show = app_lv_ui_test_clock_show,
    .hide = app_lv_ui_test_clock_hide,
};

