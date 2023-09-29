
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *tick_main;
    lv_obj_t *tick_list;
    lv_obj_t *btn_l;
    lv_obj_t *img_l;
    lv_obj_t *btn_r;
    lv_obj_t *img_r;
} *app_lv_ui_res_local = NULL;

/*@brief 清除计数列表
*/
static void app_lv_ui_local_tick_list_clr(void)
{
    if (app_lv_ui_res_local->tick_list != NULL) {
        lv_obj_del(app_lv_ui_res_local->tick_list);
        app_lv_ui_res_local->tick_list = NULL;
    }
}

/*@brief 生成计数列表
*/
static void app_lv_ui_local_tick_list_set(bool refr)
{
    if (app_lv_ui_res_local->tick_list == NULL) {
        app_lv_ui_res_local->tick_list  = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->tick_list);
        lv_obj_set_style_radius(app_lv_ui_res_local->tick_list, 0, 0);
        lv_obj_set_size(app_lv_ui_res_local->tick_list, LV_HOR_RES, app_lv_style_ver_pct(50));
        lv_obj_align_to(app_lv_ui_res_local->tick_list, app_lv_ui_res_local->tick_main, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
        /* 生成点击时Tick */
        if (refr)
            app_lv_ui_presenter_stopwatch.refr_tick_list();
        app_lv_ui_stopwatch_t *tick_list = NULL;
        app_lv_ui_presenter_stopwatch.get_tick_list(&tick_list);
        uint8_t tick_num = app_lv_ui_presenter_stopwatch.get_tick_num();
        for (uint8_t idx = 0; idx < tick_num; idx++) {
            /* 盒子 */
            lv_obj_t *box = lv_obj_create(app_lv_ui_res_local->tick_list);
            app_lv_style_object(box);
            lv_obj_set_size(box, LV_HOR_RES, app_lv_style_ver_pct(8));
            /* 左边标号 */
            lv_obj_t *lab_1 = lv_label_create(box);
            app_lv_style_object(lab_1);
            lv_obj_set_style_text_color(lab_1, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_text_align(lab_1, LV_TEXT_ALIGN_RIGHT, 0);
            lv_label_set_text_fmt(lab_1, "%02u", tick_num - idx);
            lv_obj_align(lab_1, LV_ALIGN_LEFT_MID, app_lv_style_hor_pct(5), 0);
            /* 右边数字 */
            lv_obj_t *lab_2 = lv_label_create(box);
            app_lv_style_object(lab_2);
            lv_obj_set_style_text_color(lab_2, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_text_align(lab_2, LV_TEXT_ALIGN_RIGHT, 0);
            lv_label_set_text_fmt(lab_2, "%.2u:%.2u:%.2u.%.3u",
                                  tick_list[idx].hour, tick_list[idx].minute, tick_list[idx].second, tick_list[idx].msec);
            lv_obj_align(lab_2, LV_ALIGN_RIGHT_MID, - app_lv_style_hor_pct(5), 0);
            /* 第一个的背景色染成亮蓝色 */
            if (idx == 0) {
                lv_obj_set_style_bg_opa(box,   LV_OPA_50, 0);
                lv_obj_set_style_bg_opa(lab_1, LV_OPA_TRANSP, 0);
                lv_obj_set_style_bg_opa(lab_2, LV_OPA_TRANSP, 0);
                lv_obj_set_style_bg_color(box,   lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
                lv_obj_set_style_bg_color(lab_1, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
                lv_obj_set_style_bg_color(lab_2, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
            }
        }
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_l_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 按键响应 */
        if (app_lv_ui_presenter_stopwatch.get_onoff())
            app_lv_ui_presenter_stopwatch.stop();
        else
            app_lv_ui_presenter_stopwatch.start();
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_r_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 按键响应 */
        if (app_lv_ui_presenter_stopwatch.get_onoff()) {
            /* Tick */
            app_lv_ui_local_tick_list_clr();
            app_lv_ui_local_tick_list_set(true);
        } else {
            app_lv_ui_presenter_stopwatch.reset();
            app_lv_ui_presenter_stopwatch.clean_tick_list();
            app_lv_ui_local_tick_list_clr();
        }
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
    app_lv_ui_presenter_clock.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
    /* 主题计数时钟更新 */
    app_lv_ui_stopwatch_t stopwatch = {0};
    app_lv_ui_presenter_stopwatch.get_tick_main(&stopwatch);
    lv_color_t time_color = lv_palette_main(LV_PALETTE_BLUE);
    lv_label_set_text_fmt(app_lv_ui_res_local->tick_main, "%.2u:%.2u:%.2u.#%06x %.3u#",
                          stopwatch.hour, stopwatch.minute, stopwatch.second,
                          *(uint32_t *)&time_color & 0x00FFFFFF, stopwatch.msec);
    /* 按键文本更新 */
    if (app_lv_ui_presenter_stopwatch.get_onoff()) {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_img_set_src(app_lv_ui_res_local->img_l, app_lv_pic_str_find(APP_LV_PIC_01_STOPWATCH_PAUSE_PNG));
        lv_img_set_src(app_lv_ui_res_local->img_r, app_lv_pic_str_find(APP_LV_PIC_01_STOPWATCH_TICK_0_PNG));
    } else {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_img_set_src(app_lv_ui_res_local->img_l, app_lv_pic_str_find(APP_LV_PIC_01_STOPWATCH_CONTINUE_PNG));
        lv_img_set_src(app_lv_ui_res_local->img_r, app_lv_pic_str_find(APP_LV_PIC_01_STOPWATCH_RESET_PNG));
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_stopwatch_show(void *scene)
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
        lv_label_set_text_static(title_txt, app_lv_lang_str_find(APP_LV_LANG_0X013f));
        lv_obj_align_to(title_txt, title_lab, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        app_lv_ui_res_local->time = lv_label_create(title_box);
        app_lv_style_object(app_lv_ui_res_local->time);
        lv_obj_align(app_lv_ui_res_local->time, LV_ALIGN_RIGHT_MID, -app_lv_style_hor_pct(5), 0);
        /* 主体时间 */
        app_lv_ui_res_local->tick_main = lv_label_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->tick_main);
        lv_obj_set_style_text_font(app_lv_ui_res_local->tick_main, app_lv_ui_font(32), 0);
        lv_obj_set_width(app_lv_ui_res_local->tick_main, LV_HOR_RES);
        lv_obj_align_to(app_lv_ui_res_local->tick_main, title_box, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
        lv_label_set_recolor(app_lv_ui_res_local->tick_main, true);
        /* 计数列表(点击时更新) */
        app_lv_ui_res_local->tick_list = NULL;
        app_lv_ui_local_tick_list_set(false);
        /* 底部双按钮组 */
        app_lv_ui_res_local->btn_l = lv_btn_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->btn_l);
        lv_obj_set_size(app_lv_ui_res_local->btn_l, app_lv_style_hor_pct(35), app_lv_style_ver_pct(20));
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_l, app_lv_ui_btn_l_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(app_lv_ui_res_local->btn_l, LV_ALIGN_BOTTOM_LEFT, app_lv_style_hor_pct(10), -app_lv_style_ver_pct(5));
        app_lv_ui_res_local->img_l = lv_img_create(app_lv_ui_res_local->btn_l);
        app_lv_style_object(app_lv_ui_res_local->img_l);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->img_l, LV_OPA_TRANSP, 0);
        lv_obj_center(app_lv_ui_res_local->img_l);
        app_lv_ui_res_local->btn_r = lv_btn_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->btn_r);
        lv_obj_set_size(app_lv_ui_res_local->btn_r, app_lv_style_hor_pct(35), app_lv_style_ver_pct(20));
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_r, app_lv_ui_btn_r_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_align(app_lv_ui_res_local->btn_r, LV_ALIGN_BOTTOM_RIGHT, -app_lv_style_hor_pct(10), -app_lv_style_ver_pct(5));
        app_lv_ui_res_local->img_r = lv_img_create(app_lv_ui_res_local->btn_r);
        app_lv_style_object(app_lv_ui_res_local->img_r);
        lv_obj_set_style_bg_opa(app_lv_ui_res_local->img_r, LV_OPA_TRANSP, 0);
        lv_obj_center(app_lv_ui_res_local->img_r);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_stopwatch_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 回收列表资源 */
        app_lv_ui_local_tick_list_clr();
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_stopwatch = {
    /* 场景资源节点 */
    .show = app_lv_ui_stopwatch_show,
    .hide = app_lv_ui_stopwatch_hide,
};
