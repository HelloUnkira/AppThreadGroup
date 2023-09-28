
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t anim;
    lv_obj_t *scene;
} *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
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

        /* 暂定此文本 */
        lv_obj_t *label = lv_label_create(app_lv_ui_res_local->scene);
        app_lv_style_object(label);
        lv_label_set_text(label, app_lv_lang_str_find(APP_LV_LANG_0X013a));
        lv_obj_center(label);
        
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 0, 0, 100, 1000);
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
    .sibling[0] = &app_lv_ui_null,
    .sibling[1] = &app_lv_ui_null,
    .style[0]   = app_lv_wheel_style_rotate,
    .style[1]   = app_lv_wheel_style_rotate,
    .sibling[2] = &app_lv_ui_null,
    .sibling[3] = &app_lv_ui_null,
    .style[2]   = app_lv_wheel_style_float,
    .style[3]   = app_lv_wheel_style_float,
};

app_lv_scene_t app_lv_ui_watch_face = {
    .show  =  app_lv_ui_watch_face_show,
    .hide  =  app_lv_ui_watch_face_hide,
    .wheel = &app_lv_ui_watch_face_wheel,
};
