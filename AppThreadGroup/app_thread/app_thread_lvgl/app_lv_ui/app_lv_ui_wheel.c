
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_wheel.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *label;
} app_lv_ui_res_local_t;

static app_lv_ui_wheel_t     *app_lv_ui_wheel_cfg = NULL;
static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_wheel_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 初始化居中标签 */
        app_lv_ui_res_local->label = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(app_lv_ui_res_local->label, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text_static(app_lv_ui_res_local->label, "Wheel");
        lv_obj_center(app_lv_ui_res_local->label);
        
        
        
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 0, 0, 100, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_wheel_hide(void *scene)
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

app_lv_scene_t app_lv_ui_wheel = {
    .show = app_lv_ui_wheel_show,
    .hide = app_lv_ui_wheel_hide,
};

/*@brief    配置轮盘子窗口信息
 *param[in] wheel 轮盘参数
 */
void app_lv_ui_wheel_event(app_lv_ui_wheel_t *wheel)
{
    APP_SYS_ASSERT(wheel != NULL);
    /* 轮盘信息在场景栈show前创建,那么它就刚好在下一show事件之前 */
    app_lv_ui_wheel_t *wheel_sched = lv_mem_alloc(sizeof(app_lv_ui_wheel_t));
    lv_memcpy(wheel_sched, wheel, sizeof(app_lv_ui_wheel_t));
    wheel_sched->scene = &app_lv_ui_wheel;
    /* 向线程发送配置轮盘子窗口信息事件 */
    app_thread_package_t package = {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui_scene,
        .event  = app_thread_lvgl_ui_scene_wheel,
        .data   = wheel_sched,
        .size   = sizeof(app_lv_ui_wheel_t),
    };
    app_thread_package_notify(&package);
}

/*@brief    配置轮盘子窗口信息
 *param[in] wheel 轮盘参数
 */
void app_lv_ui_wheel_sched(void *data, uint32_t size)
{
    APP_SYS_ASSERT(size == sizeof(app_lv_ui_wheel_t));
    app_lv_ui_wheel_cfg = (app_lv_ui_wheel_t *)data;
}
