
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_event_ui.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_temperature.h"
#include "app_lv_ui_temperature_presenter.h"

#include "app_lv_ui_clock_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *temp;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_click_btn_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        if (!app_lv_ui_temperature_presenter.measure_status())
             app_lv_ui_temperature_presenter.measure();
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
    char format_clock_1[20] = {0};
    app_lv_ui_clock_presenter.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
    
    lv_obj_set_style_text_color(app_lv_ui_res_local->temp,
                                app_lv_ui_temperature_presenter.measure_result() ?
                                lv_palette_main(LV_PALETTE_BLUE) : lv_palette_main(LV_PALETTE_GREY), 0);
    
    char temp_digit[20] = {0};
    sprintf(temp_digit, "%.2f", app_lv_ui_temperature_presenter.get_value());
    APP_SYS_LOG_INFO("temp_digit:%s", temp_digit);
    lv_label_set_text_fmt(app_lv_ui_res_local->temp, "Temperature:%s", temp_digit);
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_temperature_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        app_lv_ui_temperature.self = app_lv_ui_res_local->scene;
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Temperature");
        /* 中心文本 */
        app_lv_ui_res_local->temp = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(app_lv_ui_res_local->temp, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_align(app_lv_ui_res_local->temp, LV_ALIGN_CENTER, 0, 0);
        /* 下部按键 */
        lv_obj_t *btn = NULL, *btn_c = NULL, *lbl_c = NULL;
        btn = app_lv_style_one_btn(app_lv_ui_res_local->scene, &btn_c, &lbl_c);
        lv_obj_add_event_cb(btn_c, app_lv_ui_click_btn_cb, LV_EVENT_CLICKED, NULL);
        lv_label_set_text(lbl_c, "Measure");
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_temperature_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_temperature.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_temperature = {
    /* 场景资源节点 */
    .show = app_lv_ui_temperature_show,
    .hide = app_lv_ui_temperature_hide,
};
