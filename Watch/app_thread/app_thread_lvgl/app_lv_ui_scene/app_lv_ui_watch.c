
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_!util.h"
#include "app_lv_ui_watch.h"
#include "app_lv_ui_scene_set.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *label;
    lv_obj_t *bar;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;
static app_lv_ui_watch_status_t app_lv_ui_watch_status = app_lv_ui_watch_null;

/*@brief     显示界面更新
 *@param[in] status 界面(true:启动界面,false:结束界面)
 */
void app_lv_ui_watch_status_update(app_lv_ui_watch_status_t status)
{
    app_lv_ui_watch_status = status;
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    if (value <= 100) {
        if (app_lv_ui_watch_status == app_lv_ui_watch_start) {
            uint8_t bar_value = (value);
            uint8_t label_opa = (uint32_t)((float)value * 2.55);
            lv_obj_set_style_text_opa(app_lv_ui_res_local->label, label_opa, 0);
            lv_bar_set_value(app_lv_ui_res_local->bar, bar_value, LV_ANIM_OFF);
        }
        if (app_lv_ui_watch_status == app_lv_ui_watch_stop) {
            uint8_t label_opa = (uint32_t)((float)(100 - value) * 2.55);
            lv_obj_set_style_text_opa(app_lv_ui_res_local->label, label_opa, 0);
        }
    }
    if (value == 100) {
        if (app_lv_ui_watch_status == app_lv_ui_watch_start) {
            app_lv_scene_t scene = {0};
            app_lv_scene_del(&scene);
        }
    }
}

/*@brief 界面显示
 *@brief scene 场景
 */
static void app_lv_ui_watch_show(void *scene)
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
        lv_obj_set_style_border_color(app_lv_ui_res_local->scene, lv_color_black(), 0);
    }
    app_lv_scene_watch.self = app_lv_ui_res_local == NULL ? NULL :
                              app_lv_ui_res_local->scene;
    /* 空界面 */
    if (app_lv_ui_watch_status == app_lv_ui_watch_null) {
        if (app_lv_ui_res_local != NULL) {
            /* 初始化居中标签 */
            app_lv_ui_res_local->label = lv_label_create(app_lv_ui_res_local->scene);
            lv_label_set_long_mode(app_lv_ui_res_local->label, LV_LABEL_LONG_WRAP);
            lv_label_set_text_static(app_lv_ui_res_local->label, "NULL");
            lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_text_opa(app_lv_ui_res_local->label, LV_OPA_COVER, 0);
            lv_obj_center(app_lv_ui_res_local->label);
            /* 场景添加默认事件 */
            app_lv_ui_event_default_set(app_lv_ui_res_local->scene);
        }
    }
    /* 启动界面 */
    if (app_lv_ui_watch_status == app_lv_ui_watch_start) {
        if (app_lv_ui_res_local != NULL) {
            /* 初始化居中标签 */
            app_lv_ui_res_local->label = lv_label_create(app_lv_ui_res_local->scene);
            lv_label_set_long_mode(app_lv_ui_res_local->label, LV_LABEL_LONG_WRAP);
            lv_label_set_text_static(app_lv_ui_res_local->label, "LVGL Watch Enter");
            lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_text_opa(app_lv_ui_res_local->label, 0, 0);
            lv_obj_center(app_lv_ui_res_local->label);
            /* 初始化加载滚动条 */
            app_lv_ui_res_local->bar = lv_bar_create(app_lv_ui_res_local->scene);
            lv_obj_set_size(app_lv_ui_res_local->bar, 200, 20);
            lv_bar_set_range(app_lv_ui_res_local->bar, 0, 100);
            lv_obj_align(app_lv_ui_res_local->bar, LV_ALIGN_BOTTOM_MID, 0, -50);
            /* >滚动条主样式 */
            lv_obj_set_style_pad_all(app_lv_ui_res_local->bar, 4, 0);
            lv_obj_set_style_opa(app_lv_ui_res_local->bar, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_opa(app_lv_ui_res_local->bar, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->bar, lv_color_black(), 0);
            lv_obj_set_style_border_width(app_lv_ui_res_local->bar, 1, 0);
            lv_obj_set_style_border_color(app_lv_ui_res_local->bar, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_radius(app_lv_ui_res_local->bar, 30, 0);
            /* >滚动条指针样式 */
            lv_obj_set_style_opa(app_lv_ui_res_local->bar, LV_OPA_COVER, LV_PART_INDICATOR);
            lv_obj_set_style_bg_opa(app_lv_ui_res_local->bar, LV_OPA_COVER, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->bar, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
            lv_obj_set_style_bg_grad_color(app_lv_ui_res_local->bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
            lv_obj_set_style_bg_grad_dir(app_lv_ui_res_local->bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
            lv_obj_set_style_radius(app_lv_ui_res_local->bar, 30, LV_PART_INDICATOR);
            /* 初始化显示动画 */
            lv_anim_init(&app_lv_ui_res_local->anim);
            lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
            lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
            lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, 0);
            lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 100);
            lv_anim_set_time(&app_lv_ui_res_local->anim, 3000);
            lv_anim_start(&app_lv_ui_res_local->anim);
        }
    }
    /* 关闭界面 */
    if (app_lv_ui_watch_status == app_lv_ui_watch_stop) {
        if (app_lv_ui_res_local != NULL) {
            /* 初始化居中标签 */
            app_lv_ui_res_local->label = lv_label_create(app_lv_ui_res_local->scene);
            lv_label_set_long_mode(app_lv_ui_res_local->label, LV_LABEL_LONG_WRAP);
            lv_label_set_text_static(app_lv_ui_res_local->label, "LVGL Watch Exit");
            lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_text_opa(app_lv_ui_res_local->label, LV_OPA_COVER, 0);
            lv_obj_center(app_lv_ui_res_local->label);
            /* 初始化显示动画 */
            lv_anim_init(&app_lv_ui_res_local->anim);
            lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
            lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
            lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, 0);
            lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 100);
            lv_anim_set_time(&app_lv_ui_res_local->anim, 3000);
            lv_anim_start(&app_lv_ui_res_local->anim);
        }
    }
}

/*@brief 界面隐藏
 *@brief scene 场景
 */
static void app_lv_ui_watch_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        if (app_lv_ui_watch_status == app_lv_ui_watch_null)
            /* 场景去除默认事件 */
            app_lv_ui_event_default_clr(app_lv_ui_res_local->scene);
        if (app_lv_ui_watch_status == app_lv_ui_watch_start ||
            app_lv_ui_watch_status == app_lv_ui_watch_stop)
            /* 反初始化显示动画 */
            lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        app_lv_ui_watch_status = app_lv_ui_watch_null;
        lv_obj_del(app_lv_ui_res_local->scene);
        app_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_watch.self = app_lv_ui_res_local == NULL ? NULL :
                              app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_watch = {
    /* 场景资源节点 */
    .presenter = NULL,
    .show = app_lv_ui_watch_show,
    .hide = app_lv_ui_watch_hide,
};
