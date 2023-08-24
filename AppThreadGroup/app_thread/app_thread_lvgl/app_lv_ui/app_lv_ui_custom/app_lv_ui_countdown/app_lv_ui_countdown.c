
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *tick;
    lv_obj_t *btn_l;
    lv_obj_t *lbl_l;
    lv_obj_t *btn_r;
    lv_obj_t *lbl_r;
    lv_obj_t *rol_h;
    lv_obj_t *rol_m;
    lv_obj_t *rol_s;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

static const char app_lv_ui_res_0_23[] = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
static const char app_lv_ui_res_0_59[] = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";

#define app_lv_ui_res_tick_font         &lv_font_montserrat_28
#define app_lv_ui_res_rolsel_font       &lv_font_montserrat_28
#define app_lv_ui_res_rolcus_font       &lv_font_montserrat_22
#define app_lv_ui_res_btn_text_font     &lv_font_montserrat_22

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_l_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 按键响应 */
        if (app_lv_ui_countdown_presenter.get_onoff()) {
            app_lv_ui_countdown_presenter.stop();
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_YELLOW), 0);
            lv_label_set_text(app_lv_ui_res_local->lbl_l, "Run");
            lv_label_set_text(app_lv_ui_res_local->lbl_r, "Set");
        } else {
            app_lv_ui_countdown_presenter.start();
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_YELLOW), 0);
            lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_label_set_text(app_lv_ui_res_local->lbl_l, "Pause");
            lv_label_set_text(app_lv_ui_res_local->lbl_r, "Reset");
        }
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
        if (app_lv_ui_countdown_presenter.get_onoff()) {
            app_lv_ui_countdown_t countdown_0 = {0};
            app_lv_ui_countdown_presenter.stop();
            app_lv_ui_countdown_presenter.set(&countdown_0);
            app_lv_ui_countdown_t countdown;
            app_lv_ui_countdown_presenter.get(&countdown);
            lv_roller_set_selected(app_lv_ui_res_local->rol_h, countdown.hour,   LV_ANIM_ON);
            lv_roller_set_selected(app_lv_ui_res_local->rol_m, countdown.minute, LV_ANIM_ON);
            lv_roller_set_selected(app_lv_ui_res_local->rol_s, countdown.second, LV_ANIM_ON);
        } else {
            app_lv_ui_countdown_t countdown = {0};
            countdown.hour   = lv_roller_get_selected(app_lv_ui_res_local->rol_h);
            countdown.minute = lv_roller_get_selected(app_lv_ui_res_local->rol_m);
            countdown.second = lv_roller_get_selected(app_lv_ui_res_local->rol_s);
            app_lv_ui_countdown_presenter.set(&countdown);
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
    app_lv_ui_clock_presenter.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
    /* 主题计数时钟更新 */
    app_lv_ui_countdown_t countdown = {0};
    app_lv_ui_countdown_presenter.get_tick(&countdown);
    lv_label_set_text_fmt(app_lv_ui_res_local->tick, "%.2u:%.2u:%.2u.%.3u",
                          countdown.hour, countdown.minute, countdown.second, countdown.msec);
    /* 按键文本更新 */
    if (app_lv_ui_countdown_presenter.get_onoff()) {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_YELLOW), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "Pause");
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "Reset");
    } else {
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_bg_color(app_lv_ui_res_local->btn_r, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_label_set_text(app_lv_ui_res_local->lbl_l, "Run");
        lv_label_set_text(app_lv_ui_res_local->lbl_r, "Set");
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_countdown_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Countdown");
        /* 主体文本 */
        app_lv_ui_res_local->tick = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(app_lv_ui_res_local->tick, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_align_to(app_lv_ui_res_local->tick, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制日期后三位滚轮 */
        lv_obj_t *rol_set = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(rol_set);
        lv_obj_set_size(rol_set, LV_HOR_RES, app_lv_style_ver_pct(50));
        lv_obj_align_to(rol_set, app_lv_ui_res_local->tick, LV_ALIGN_OUT_BOTTOM_MID, 0, app_lv_style_ver_pct(10));
        /* 绘制分滚轮,中心对齐 */
        app_lv_ui_res_local->rol_m = app_lv_style_roller(rol_set, app_lv_style_hor_pct(30), app_lv_ui_res_0_59, 3);
        lv_obj_align(app_lv_ui_res_local->rol_m, LV_ALIGN_TOP_MID, 0, 0);
        /* 绘制时滚轮,分滚轮左外对齐 */
        app_lv_ui_res_local->rol_h = app_lv_style_roller(rol_set, app_lv_style_hor_pct(30), app_lv_ui_res_0_23, 3);
        lv_obj_align_to(app_lv_ui_res_local->rol_h, app_lv_ui_res_local->rol_m, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        /* 绘制秒滚轮,分滚轮右外对齐 */
        app_lv_ui_res_local->rol_s = app_lv_style_roller(rol_set, app_lv_style_hor_pct(30), app_lv_ui_res_0_59, 3);
        lv_obj_align_to(app_lv_ui_res_local->rol_s, app_lv_ui_res_local->rol_m, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        /* 底部双按钮组 */
        lv_obj_t *btn = app_lv_style_two_btns(app_lv_ui_res_local->scene,
                                                &app_lv_ui_res_local->btn_l,
                                                &app_lv_ui_res_local->lbl_l,
                                                &app_lv_ui_res_local->btn_r,
                                                &app_lv_ui_res_local->lbl_r);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_l, app_lv_ui_btn_l_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(app_lv_ui_res_local->btn_r, app_lv_ui_btn_r_cb, LV_EVENT_CLICKED, NULL);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
        /* 滚轮选中刷新 */
        app_lv_ui_countdown_t countdown;
        app_lv_ui_countdown_presenter.get(&countdown);
        lv_roller_set_selected(app_lv_ui_res_local->rol_h, countdown.hour,   LV_ANIM_ON);
        lv_roller_set_selected(app_lv_ui_res_local->rol_m, countdown.minute, LV_ANIM_ON);
        lv_roller_set_selected(app_lv_ui_res_local->rol_s, countdown.second, LV_ANIM_ON);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_countdown_hide(void *scene)
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

app_lv_scene_t app_lv_ui_countdown = {
    /* 场景资源节点 */
    .show = app_lv_ui_countdown_show,
    .hide = app_lv_ui_countdown_hide,
};
