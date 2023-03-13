
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_watch.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *spinner;
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
        if (app_lv_ui_watch_status == app_lv_ui_watch_start ||
            app_lv_ui_watch_status == app_lv_ui_watch_stop) {
            uint8_t val = 0;
            uint8_t opa = 0;
            if (app_lv_ui_watch_status == app_lv_ui_watch_start) {
                val = (value);
                opa = (uint32_t)((float)value * 2.55);
            }
            if (app_lv_ui_watch_status == app_lv_ui_watch_stop) {
                val = (100 - value);
                opa = (uint32_t)((float)(100 - value) * 2.55);
            }
            lv_obj_set_style_opa(app_lv_ui_res_local->spinner, opa, 0);
            lv_obj_set_style_text_opa(app_lv_ui_res_local->label, opa, 0);
            lv_obj_set_style_opa(app_lv_ui_res_local->bar, opa, 0);
            lv_obj_set_style_opa(app_lv_ui_res_local->bar, opa, LV_PART_INDICATOR);
            lv_bar_set_value(app_lv_ui_res_local->bar, val, LV_ANIM_OFF);
        }
    }
    if (value == 100) {
        if (app_lv_ui_watch_status == app_lv_ui_watch_start) {
            app_lv_scene_t scene = {0};
            app_lv_scene_del(&scene);
        }
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_watch_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        /* DLPS界面 */
        if (app_lv_ui_watch_status == app_lv_ui_watch_dlps);
        /* 空界面 */
        if (app_lv_ui_watch_status == app_lv_ui_watch_null) {
            if (app_lv_ui_res_local != NULL) {
                /* 初始化居中标签 */
                app_lv_ui_res_local->label = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
                lv_obj_set_style_text_opa(app_lv_ui_res_local->label, LV_OPA_COVER, 0);
                lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
                lv_label_set_text_static(app_lv_ui_res_local->label, "NULL");
                lv_obj_center(app_lv_ui_res_local->label);
            }
        }
        /* 启动界面,关闭界面 */
        if (app_lv_ui_watch_status == app_lv_ui_watch_start ||
            app_lv_ui_watch_status == app_lv_ui_watch_stop) {
            if (app_lv_ui_res_local != NULL) {
                /* 禁用默认事件响应 */
                app_lv_ui_event_default_config(false);
                /* 初始化加载圆环 */
                app_lv_ui_res_local->spinner = app_lv_ui_style_loading_spinner(app_lv_ui_res_local->scene, 30, 5);
                lv_obj_align(app_lv_ui_res_local->spinner, LV_ALIGN_TOP_LEFT, 20, 20);
                /* 初始化居中标签 */
                app_lv_ui_res_local->label = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
                lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
                lv_label_set_text_static(app_lv_ui_res_local->label, "ATG LVGL Watch");
                lv_obj_center(app_lv_ui_res_local->label);
                /* 初始化加载进度条 */
                app_lv_ui_res_local->bar = app_lv_ui_style_loading_bar(app_lv_ui_res_local->scene, 200, 20, LV_GRAD_DIR_HOR);
                lv_obj_align_to(app_lv_ui_res_local->bar, app_lv_ui_res_local->label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
                /* 初始化显示动画 */
                lv_anim_init(&app_lv_ui_res_local->anim);
                lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
                lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
                lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, 0);
                lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 100);
                lv_anim_set_time(&app_lv_ui_res_local->anim, 2000);
                lv_anim_start(&app_lv_ui_res_local->anim);
            }
        }
    }
    app_lv_scene_watch.self = app_lv_ui_res_local == NULL ? NULL :
                              app_lv_ui_res_local->scene;
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_watch_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 启动界面,关闭界面 */
        if (app_lv_ui_watch_status == app_lv_ui_watch_start ||
            app_lv_ui_watch_status == app_lv_ui_watch_stop) {
            /* 反初始化显示动画 */
            lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
            /* 启用默认事件响应 */
            app_lv_ui_event_default_config(true);
        }
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
        app_lv_ui_watch_status = app_lv_ui_watch_null;
    }
    app_lv_scene_watch.self = app_lv_ui_res_local == NULL ? NULL :
                              app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_watch = {
    /* 场景资源节点 */
    .show = app_lv_ui_watch_show,
    .hide = app_lv_ui_watch_hide,
};
