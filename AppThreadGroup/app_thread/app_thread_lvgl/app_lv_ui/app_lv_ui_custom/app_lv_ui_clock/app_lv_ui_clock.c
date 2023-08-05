
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_clock.h"
#include "app_lv_ui_clock_presenter.h"

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

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    lv_color_t time_color[7] = {
        lv_palette_main(LV_PALETTE_RED),
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_GREEN),
        lv_palette_main(LV_PALETTE_YELLOW),
        lv_palette_main(LV_PALETTE_PINK),
        lv_palette_main(LV_PALETTE_BROWN),
        lv_palette_main(LV_PALETTE_ORANGE),
    };
    
    char time_color_0x[7][20] = {0};
    
    for (uint8_t idx = 0; idx < 7; idx++)
        sprintf(time_color_0x[idx], "%06x", (*(uint32_t *)(lv_color_t *)&time_color[idx]) % 0xFF000000);
    
    lv_label_set_text_fmt(app_lv_ui_res_local->label_dtime,
                          "#%s %u#-#%s %u#-#%s %u# #%s %u#:#%s %u#:#%s %u# #%s %s#",
                          time_color_0x[0], app_lv_ui_clock_presenter.get_year(),
                          time_color_0x[1], app_lv_ui_clock_presenter.get_month(),
                          time_color_0x[2], app_lv_ui_clock_presenter.get_day(),
                          time_color_0x[3], app_lv_ui_clock_presenter.get_hour(),
                          time_color_0x[4], app_lv_ui_clock_presenter.get_minute(),
                          time_color_0x[5], app_lv_ui_clock_presenter.get_second(),
                          time_color_0x[6], app_lv_ui_res_week[app_lv_ui_clock_presenter.get_week()]);
    
    lv_roller_set_selected(app_lv_ui_res_local->roller_h, app_lv_ui_clock_presenter.get_hour(),   LV_ANIM_ON);
    lv_roller_set_selected(app_lv_ui_res_local->roller_m, app_lv_ui_clock_presenter.get_minute(), LV_ANIM_ON);
    lv_roller_set_selected(app_lv_ui_res_local->roller_s, app_lv_ui_clock_presenter.get_second(), LV_ANIM_ON);
    
    const lv_color_t roller_color[3] = {lv_palette_main(LV_PALETTE_BLUE),
                                        lv_palette_main(LV_PALETTE_GREEN),
                                        lv_palette_main(LV_PALETTE_RED)};
    lv_obj_set_style_text_color(app_lv_ui_res_local->roller_h, roller_color[(value + 0) % 3], 0);
    lv_obj_set_style_text_color(app_lv_ui_res_local->roller_h, roller_color[(value + 0) % 3], LV_PART_SELECTED);
    lv_obj_set_style_text_color(app_lv_ui_res_local->roller_m, roller_color[(value + 1) % 3], 0);
    lv_obj_set_style_text_color(app_lv_ui_res_local->roller_m, roller_color[(value + 1) % 3], LV_PART_SELECTED);
    lv_obj_set_style_text_color(app_lv_ui_res_local->roller_s, roller_color[(value + 2) % 3], 0);
    lv_obj_set_style_text_color(app_lv_ui_res_local->roller_s, roller_color[(value + 2) % 3], LV_PART_SELECTED);
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_clock_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        /* 初始化标签,上中部 */
        lv_obj_t *label = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_label_set_text_static(label, "System Clock");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        /* 绘制日期 */
        app_lv_ui_res_local->label_dtime = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_label_set_recolor(app_lv_ui_res_local->label_dtime, true);
        lv_obj_align_to(app_lv_ui_res_local->label_dtime, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制日期后三位滚轮 */
        lv_obj_t *roller_set = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(roller_set);
        lv_obj_set_size(roller_set, LV_HOR_RES, app_lv_style_ver_pct(50));
        lv_obj_align(roller_set, LV_ALIGN_CENTER, 0, 0);
        /* 绘制分滚轮,中心对齐 */
        app_lv_ui_res_local->roller_m = app_lv_style_roller(roller_set, app_lv_style_hor_pct(30), app_lv_ui_res_0_59, 3);
        lv_obj_align(app_lv_ui_res_local->roller_m, LV_ALIGN_TOP_MID, 0, 0);
        /* 绘制时滚轮,分滚轮左外对齐 */
        app_lv_ui_res_local->roller_h = app_lv_style_roller(roller_set, app_lv_style_hor_pct(30), app_lv_ui_res_0_23, 3);
        lv_obj_align_to(app_lv_ui_res_local->roller_h, app_lv_ui_res_local->roller_m, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        /* 绘制秒滚轮,分滚轮右外对齐 */
        app_lv_ui_res_local->roller_s = app_lv_style_roller(roller_set, app_lv_style_hor_pct(30), app_lv_ui_res_0_59, 3);
        lv_obj_align_to(app_lv_ui_res_local->roller_s, app_lv_ui_res_local->roller_m, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 3, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_clock_hide(void *scene)
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

app_lv_scene_t app_lv_ui_clock = {
    /* 场景资源节点 */
    .show = app_lv_ui_clock_show,
    .hide = app_lv_ui_clock_hide,
};
