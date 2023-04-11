
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_style.h"

#include "app_lv_ui_qrcode.h"
#include "app_lv_ui_qrcode_presenter.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *qrcode;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    const char *str = app_lv_ui_qrcode_presenter.qrcode_str();
    lv_qrcode_update(app_lv_ui_res_local->qrcode, str, strlen(str));
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_qrcode_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        app_lv_ui_qrcode.self = app_lv_ui_res_local->scene;
        /* 初始化标签,上中部 */
        lv_obj_t *label = app_lv_ui_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text_static(label, "QR Code");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        /* qrcode */
        app_lv_ui_res_local->qrcode = lv_qrcode_create(app_lv_ui_res_local->scene, 200, lv_color_black(), lv_color_white());
        app_lv_ui_style_object(app_lv_ui_res_local->qrcode);
        lv_obj_align(app_lv_ui_res_local->qrcode, LV_ALIGN_CENTER, 0, 0);
        /* 初始化显示动画 */
        app_lv_ui_style_object_anim(app_lv_ui_res_local->scene,
                                   &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                    LV_ANIM_REPEAT_INFINITE, 0, 1, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_qrcode_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* qrcode */
        lv_qrcode_delete(app_lv_ui_res_local->qrcode);
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        app_lv_ui_qrcode.self = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_qrcode = {
    /* 场景资源节点 */
    .show = app_lv_ui_qrcode_show,
    .hide = app_lv_ui_qrcode_hide,
};
