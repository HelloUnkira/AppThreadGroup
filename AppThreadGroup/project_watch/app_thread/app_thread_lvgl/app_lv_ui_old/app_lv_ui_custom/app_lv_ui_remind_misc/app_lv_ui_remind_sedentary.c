
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
} *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 左上角系统时钟更新 */
    char format_clock_1[20] = {0};
    app_lv_ui_presenter_clock.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
}

/*@brief 界面显示
 *@param scene 场景
 */
static void app_lv_ui_remind_sedentary_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Sedentary Remind");
        /* 中心文本 */
        lv_obj_t *times_up = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(times_up, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_align(times_up, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(times_up, "Sedentary Remind Times Up");
        /* 下部按键 */
        lv_obj_t *btn = NULL, *btn_c = NULL, *lbl_c = NULL;
        btn = app_lv_style_one_btn(app_lv_ui_res_local->scene, &btn_c, &lbl_c);
        lv_obj_add_event_cb(btn_c, app_lv_event_ui_backtrack_cb, LV_EVENT_CLICKED, NULL);
        lv_label_set_text(lbl_c, "Confirm");
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief 界面隐藏
 *@param scene 场景
 */
static void app_lv_ui_remind_sedentary_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_remind_sedentary = {
    /* 场景资源节点 */
    .show = app_lv_ui_remind_sedentary_show,
    .hide = app_lv_ui_remind_sedentary_hide,
};
