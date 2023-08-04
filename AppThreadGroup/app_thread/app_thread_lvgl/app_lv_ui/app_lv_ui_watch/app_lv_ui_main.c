
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_float.h"

#include "app_lv_ui_main.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *label;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    static uint32_t count = 0;
    if (count++ < 100)
    if (value <= 100) {
        uint8_t label_opa = (uint32_t)((float)value * 2.55);
        lv_obj_set_style_text_opa(app_lv_ui_res_local->label, label_opa, 0);
    }
    #if 0
    /* 浮动子窗口检查 */
    app_lv_scene_t *scene = NULL;
    scene = app_lv_ui_main.float_t;
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(scene->self != NULL);
    APP_SYS_LOG_INFO("float_t(x:%d,y:%d,w:%d,h:%d)\n", lv_obj_get_x(scene->self), lv_obj_get_y(scene->self), lv_obj_get_width(scene->self), lv_obj_get_height(scene->self));
    scene = app_lv_ui_main.float_b;
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(scene->self != NULL);
    APP_SYS_LOG_INFO("float_b(x:%d,y:%d,w:%d,h:%d)\n", lv_obj_get_x(scene->self), lv_obj_get_y(scene->self), lv_obj_get_width(scene->self), lv_obj_get_height(scene->self));
    scene = app_lv_ui_main.float_l;
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(scene->self != NULL);
    APP_SYS_LOG_INFO("float_l(x:%d,y:%d,w:%d,h:%d)\n", lv_obj_get_x(scene->self), lv_obj_get_y(scene->self), lv_obj_get_width(scene->self), lv_obj_get_height(scene->self));
    scene = app_lv_ui_main.float_r;
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(scene->self != NULL);
    APP_SYS_LOG_INFO("float_r(x:%d,y:%d,w:%d,h:%d)\n", lv_obj_get_x(scene->self), lv_obj_get_y(scene->self), lv_obj_get_width(scene->self), lv_obj_get_height(scene->self));
    #endif
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_main_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        app_lv_ui_main.self = app_lv_ui_res_local->scene;
        /* 初始化居中标签 */
        app_lv_ui_res_local->label = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text_static(app_lv_ui_res_local->label, "Watch Main");
        lv_obj_center(app_lv_ui_res_local->label);
        /* 浮动子窗口 */
        app_lv_ui_float_active(&app_lv_ui_main);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    0, 0, 100, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_main_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 浮动子窗口 */
        app_lv_ui_float_hidden(&app_lv_ui_main);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_main.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_main = {
    /* 场景资源节点 */
    .float_l = &app_lv_ui_list,
    .float_r = &app_lv_ui_test_list,
    .float_t = &app_lv_ui_clock,
    .float_b = &app_lv_ui_calendar,
    /* 什么意思???坐标不能控制??? */
//    .float_t = &app_lv_ui_qrcode,
//    .float_t = &app_lv_ui_watch_null,
    /*  */
    .show = app_lv_ui_main_show,
    .hide = app_lv_ui_main_hide,
};
