
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/* 使用控件绘制时钟 */
#define APP_LV_UI_RES_USE_BASE      1

static struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    #if APP_LV_UI_RES_USE_BASE
    lv_obj_t *dtime;
    lv_obj_t *meter;
    lv_meter_indicator_t *meter_indec_h;
    lv_meter_indicator_t *meter_indec_m;
    lv_meter_indicator_t *meter_indec_s;
    #else
    #endif
} *app_lv_ui_res_local = NULL;

static const char *app_lv_ui_res_week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    #if APP_LV_UI_RES_USE_BASE
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
    
    lv_label_set_text_fmt(app_lv_ui_res_local->dtime,
                          "#%s %u#-#%s %u#-#%s %u# #%s %u#:#%s %u#:#%s %u# #%s %s#",
                          time_color_0x[0], app_lv_ui_presenter_clock.get_year(),
                          time_color_0x[1], app_lv_ui_presenter_clock.get_month(),
                          time_color_0x[2], app_lv_ui_presenter_clock.get_day(),
                          time_color_0x[3], app_lv_ui_presenter_clock.get_hour(),
                          time_color_0x[4], app_lv_ui_presenter_clock.get_minute(),
                          time_color_0x[5], app_lv_ui_presenter_clock.get_second(),
                          time_color_0x[6], app_lv_ui_res_week[app_lv_ui_presenter_clock.get_week()]);
    
    uint8_t hour = app_lv_ui_presenter_clock.get_hour();
    uint8_t minute = app_lv_ui_presenter_clock.get_minute();
    uint8_t second = app_lv_ui_presenter_clock.get_second();
    hour = hour > 12 ? hour - 12 : hour == 0 ? 12 : hour;
    
    lv_meter_set_indicator_value(app_lv_ui_res_local->meter, app_lv_ui_res_local->meter_indec_h, hour);
    lv_meter_set_indicator_value(app_lv_ui_res_local->meter, app_lv_ui_res_local->meter_indec_m, minute);
    lv_meter_set_indicator_value(app_lv_ui_res_local->meter, app_lv_ui_res_local->meter_indec_s, second);
    #else
    #endif
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_watch_face_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 绘制时钟 */
        #if APP_LV_UI_RES_USE_BASE
        app_lv_ui_res_local->dtime = lv_label_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->dtime);
        lv_obj_set_width(app_lv_ui_res_local->dtime, LV_HOR_RES);
        lv_obj_align(app_lv_ui_res_local->dtime, LV_ALIGN_TOP_MID, 0, app_lv_style_ver_pct(5));
        lv_label_set_recolor(app_lv_ui_res_local->dtime, true);
        /*  */
        app_lv_ui_res_local->meter = lv_meter_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->meter);
        lv_obj_set_size(app_lv_ui_res_local->meter, 270, 270);
        lv_obj_align_to(app_lv_ui_res_local->meter, app_lv_ui_res_local->dtime, LV_ALIGN_OUT_BOTTOM_MID, 0, app_lv_style_ver_pct(5));
        /*  */
        lv_meter_scale_t *meter_scale_s = lv_meter_add_scale(app_lv_ui_res_local->meter);
        lv_meter_scale_t *meter_scale_m = lv_meter_add_scale(app_lv_ui_res_local->meter);
        lv_meter_scale_t *meter_scale_h = lv_meter_add_scale(app_lv_ui_res_local->meter);
        lv_meter_set_scale_range(app_lv_ui_res_local->meter, meter_scale_s, 0, 60, 360, 270);
        lv_meter_set_scale_range(app_lv_ui_res_local->meter, meter_scale_m, 0, 60, 360, 270);
        lv_meter_set_scale_range(app_lv_ui_res_local->meter, meter_scale_h, 1, 12, 330, 300);
        lv_meter_set_scale_ticks(app_lv_ui_res_local->meter, meter_scale_s, 61, 1, 10, lv_color_white());
        lv_meter_set_scale_ticks(app_lv_ui_res_local->meter, meter_scale_m, 61, 1, 10, lv_color_white());
        lv_meter_set_scale_ticks(app_lv_ui_res_local->meter, meter_scale_h, 12, 1, 10, lv_color_white());
        lv_meter_set_scale_major_ticks(app_lv_ui_res_local->meter, meter_scale_h, 1, 3, 20, lv_color_white(), 15);
        app_lv_ui_res_local->meter_indec_s = lv_meter_add_needle_line(app_lv_ui_res_local->meter, meter_scale_s, 1, lv_palette_main(LV_PALETTE_GREEN), -10);
        app_lv_ui_res_local->meter_indec_m = lv_meter_add_needle_line(app_lv_ui_res_local->meter, meter_scale_m, 3, lv_palette_main(LV_PALETTE_BLUE),  -20);
        app_lv_ui_res_local->meter_indec_h = lv_meter_add_needle_line(app_lv_ui_res_local->meter, meter_scale_h, 3, lv_palette_main(LV_PALETTE_RED),   -50);
        /*  */
        #else
        #endif
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_watch_face_hide(void *scene)
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

app_lv_wheel_t app_lv_ui_watch_face_wheel = {
    .self       = &app_lv_ui_watch_face,
    .sibling[0] = &app_lv_ui_log_text,
    .sibling[1] = &app_lv_ui_fs_info,
    .style[0]   = app_lv_wheel_style_rotate,
    .style[1]   = app_lv_wheel_style_rotate,
    .sibling[2] = &app_lv_ui_qrcode,
    .sibling[3] = &app_lv_ui_null,
    .style[2]   = app_lv_wheel_style_float,
    .style[3]   = app_lv_wheel_style_float,
};

app_lv_scene_t app_lv_ui_watch_face = {
    .show  =  app_lv_ui_watch_face_show,
    .hide  =  app_lv_ui_watch_face_hide,
    .wheel = &app_lv_ui_watch_face_wheel,
};
