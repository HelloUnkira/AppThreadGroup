
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

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
static void app_lv_ui_countdown_remind_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 默认顶部风格 */
        lv_obj_t *title_box = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(title_box);
        lv_obj_set_size(title_box, LV_HOR_RES, app_lv_style_ver_pct(12));
        lv_obj_align(title_box, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_t *title_lab = lv_label_create(title_box);
        app_lv_style_object(title_lab);
        lv_obj_add_flag(title_lab, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(title_lab, app_lv_event_ui_backtrack_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_set_style_text_font(title_lab, app_lv_ui_font(36), 0);
        lv_label_set_text_static(title_lab, LV_SYMBOL_BACKSPACE);
        lv_obj_align(title_lab, LV_ALIGN_LEFT_MID, app_lv_style_hor_pct(5), 0);
        lv_obj_t *title_txt = lv_label_create(title_box);
        app_lv_style_object(title_txt);
        lv_obj_set_width(title_txt, app_lv_style_hor_pct(50));
        lv_obj_set_style_text_align(title_txt, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_text_static(title_txt, app_lv_lang_str_find(APP_LV_LANG_0X0139));
        lv_obj_align_to(title_txt, title_lab, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        app_lv_ui_res_local->time = lv_label_create(title_box);
        app_lv_style_object(app_lv_ui_res_local->time);
        lv_obj_align(app_lv_ui_res_local->time, LV_ALIGN_RIGHT_MID, -app_lv_style_hor_pct(5), 0);
        /* 中心文本 */
        lv_obj_t *times_up = lv_label_create(app_lv_ui_res_local->scene);
        app_lv_style_object(times_up);
        lv_obj_align(times_up, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(times_up, app_lv_lang_str_find(APP_LV_LANG_0X013e));
        /* 下部按键 */
        lv_obj_t *btn_c = lv_btn_create(app_lv_ui_res_local->scene);
        app_lv_style_object(btn_c);
        lv_obj_set_style_radius(btn_c, 45, 0);
        lv_obj_set_style_bg_color(btn_c, lv_palette_main(LV_PALETTE_YELLOW), 0);
        lv_obj_set_size(btn_c, app_lv_style_hor_pct(70), app_lv_style_ver_pct(20));
        lv_obj_add_event_cb(btn_c, app_lv_event_ui_backtrack_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(btn_c, LV_ALIGN_BOTTOM_MID, 0, -app_lv_style_ver_pct(5));
        lv_obj_t *img_c = lv_img_create(btn_c);
        app_lv_style_object(img_c);
        lv_obj_set_style_bg_opa(img_c, LV_OPA_TRANSP, 0);
        lv_obj_center(img_c);
        lv_img_set_src(img_c, app_lv_pic_str_find(APP_LV_PIC_02_COUNTDOWN_TICK_1_PNG));
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief 界面隐藏
 *@param scene 场景
 */
static void app_lv_ui_countdown_remind_hide(void *scene)
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

app_lv_scene_t app_lv_ui_countdown_remind = {
    /* 场景资源节点 */
    .show = app_lv_ui_countdown_remind_show,
    .hide = app_lv_ui_countdown_remind_hide,
};
