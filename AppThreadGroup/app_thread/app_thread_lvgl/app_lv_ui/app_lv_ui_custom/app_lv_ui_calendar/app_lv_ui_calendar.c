
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_calendar.h"
#include "app_lv_ui_clock_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *label_dtime;
    lv_obj_t *calendar;
    lv_obj_t *btnmatrix;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static const char *app_lv_ui_res_week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_calendar_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        lv_calendar_date_t date = {0};
        if (lv_calendar_get_pressed_date(app_lv_ui_res_local->calendar, &date) == LV_RES_OK)
            lv_calendar_set_highlighted_dates(app_lv_ui_res_local->calendar, &date, 1);
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
    
//    lv_calendar_date_t date = {
//        .year  = app_lv_ui_clock_presenter.get_year(),
//        .month = app_lv_ui_clock_presenter.get_month(),
//        .day   = app_lv_ui_clock_presenter.get_day(),
//    };
//    lv_calendar_set_showed_date(app_lv_ui_res_local->calendar, date.year, date.month);
//    lv_calendar_set_today_date(app_lv_ui_res_local->calendar, date.year, date.month, date.day);
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_calendar_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        app_lv_ui_calendar.self = app_lv_ui_res_local->scene;
        /* 初始化标签,上中部 */
        lv_obj_t *label = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
        lv_label_set_text_static(label, "Calendar");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        /* 绘制日期 */
        app_lv_ui_res_local->label_dtime = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_label_set_recolor(app_lv_ui_res_local->label_dtime, true);
        lv_obj_align_to(app_lv_ui_res_local->label_dtime, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制日历表 */
        app_lv_ui_res_local->calendar = lv_calendar_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->calendar);
        lv_obj_add_event_cb(app_lv_ui_res_local->calendar, app_lv_ui_calendar_cb, LV_EVENT_ALL, NULL);
        lv_obj_align_to(app_lv_ui_res_local->calendar, app_lv_ui_res_local->label_dtime, LV_ALIGN_OUT_BOTTOM_MID, 0, app_lv_style_ver_pct(10));
        lv_obj_set_style_border_width(app_lv_ui_res_local->calendar, 0, 0);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 3, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_calendar_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_calendar.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_calendar = {
    /* 场景资源节点 */
    .show = app_lv_ui_calendar_show,
    .hide = app_lv_ui_calendar_hide,
};
