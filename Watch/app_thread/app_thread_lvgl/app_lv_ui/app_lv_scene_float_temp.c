/*
 *实现目的:
 *    lvgl扩展封装特效:游离的浮动窗口
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_scene_float_temp.h"

typedef struct {
    lv_obj_t *scene_t;
    lv_obj_t *scene_b;
    lv_obj_t *scene_l;
    lv_obj_t *scene_r;
    lv_obj_t *scene_t_vis;
    lv_obj_t *scene_b_vis;
    lv_obj_t *scene_l_vis;
    lv_obj_t *scene_r_vis;
    lv_style_t style;
} app_lv_res_local_t;

typedef struct {
    app_lv_scene_t *scene_t;
    app_lv_scene_t *scene_b;
    app_lv_scene_t *scene_l;
    app_lv_scene_t *scene_r;
} app_lv_res_scene_t;

static app_lv_res_scene_t app_lv_res_scene = {0}; 
static app_lv_res_local_t app_lv_res_local = {0};

static void app_lv_scene_float_event(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_SCROLL_BEGIN:
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_BEGIN\n");
        break;
    case LV_EVENT_SCROLL:
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL\n");
        break;
    case LV_EVENT_SCROLL_END:
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_END\n");
        lv_obj_set_style_opa(lv_event_get_param(e), LV_OPA_COVER, 0);
        break;
    case LV_EVENT_GESTURE:
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE\n");
        break;
    case LV_EVENT_PRESSED:
        APP_SYS_LOG_INFO("LV_EVENT_PRESSED\n");
        break;
    case LV_EVENT_PRESSING:
        APP_SYS_LOG_INFO("LV_EVENT_PRESSING\n");
        break;
    case LV_EVENT_PRESS_LOST:
        APP_SYS_LOG_INFO("LV_EVENT_PRESS_LOST\n");
        break;
    default:
        break;
    }
}

/*@brief     刷新浮动窗口所需场景资源
 *           注意:它调用了敏感资源
 *           对它的递归调用不能到lv_timer_handler
 *@param[in] t 上浮动窗口
 *@param[in] b 下浮动窗口
 *@param[in] l 左浮动窗口
 *@param[in] r 右浮动窗口
 */
static void app_lv_scene_float_refr(app_lv_scene_t *t, app_lv_scene_t *b,
                                    app_lv_scene_t *l, app_lv_scene_t *r)
{
    /* 先回收场景 */
    if (app_lv_res_scene.scene_t != NULL) {
        app_lv_res_scene.scene_t->event = app_lv_scene_need_hide;
        app_lv_scene_sched(app_lv_res_scene.scene_t);
        app_lv_res_scene.scene_t  = NULL;
    }
    if (app_lv_res_scene.scene_b != NULL) {
        app_lv_res_scene.scene_b->event = app_lv_scene_need_hide;
        app_lv_scene_sched(app_lv_res_scene.scene_b);
        app_lv_res_scene.scene_b  = NULL;
    }
    if (app_lv_res_scene.scene_l != NULL) {
        app_lv_res_scene.scene_l->event = app_lv_scene_need_hide;
        app_lv_scene_sched(app_lv_res_scene.scene_l);
        app_lv_res_scene.scene_l  = NULL;
    }
    if (app_lv_res_scene.scene_r != NULL) {
        app_lv_res_scene.scene_r->event = app_lv_scene_need_hide;
        app_lv_scene_sched(app_lv_res_scene.scene_r);
        app_lv_res_scene.scene_r  = NULL;
    }
    /* 再生成场景 */
    if (t != NULL) {
        app_lv_res_scene.scene_t = t;
        app_lv_res_scene.scene_t->event = app_lv_scene_need_show;
        app_lv_scene_sched(app_lv_res_scene.scene_t);
    }
    if (b != NULL) {
        app_lv_res_scene.scene_b = b;
        app_lv_res_scene.scene_b->event = app_lv_scene_need_show;
        app_lv_scene_sched(app_lv_res_scene.scene_b);
    }
    if (l != NULL) {
        app_lv_res_scene.scene_l = l;
        app_lv_res_scene.scene_l->event = app_lv_scene_need_show;
        app_lv_scene_sched(app_lv_res_scene.scene_l);
    }
    if (r != NULL) {
        app_lv_res_scene.scene_r = r;
        app_lv_res_scene.scene_r->event = app_lv_scene_need_show;
        app_lv_scene_sched(app_lv_res_scene.scene_r);
    }
}

/*@brief 浮动窗口内部场景资源移除
 */
static void app_lv_scene_float_del(void)
{
    /* 各个子窗口认祖归宗 */
    if (app_lv_res_scene.scene_t != NULL)
    if (app_lv_res_scene.scene_t->self != NULL)
        lv_obj_set_parent(app_lv_res_scene.scene_t->self, lv_scr_act());
    if (app_lv_res_scene.scene_b != NULL)
    if (app_lv_res_scene.scene_b->self != NULL)
        lv_obj_set_parent(app_lv_res_scene.scene_b->self, lv_scr_act());
    if (app_lv_res_scene.scene_l != NULL)
    if (app_lv_res_scene.scene_l->self != NULL)
        lv_obj_set_parent(app_lv_res_scene.scene_l->self, lv_scr_act());
    if (app_lv_res_scene.scene_r != NULL)
    if (app_lv_res_scene.scene_r->self != NULL)
        lv_obj_set_parent(app_lv_res_scene.scene_r->self, lv_scr_act());
    /* 处理浮动窗口剩下部分 */
    if (app_lv_res_local.scene_t_vis != NULL) {
        lv_obj_remove_event_cb(app_lv_res_local.scene_t_vis, app_lv_scene_float_event);
        app_lv_res_local.scene_t_vis  = NULL;
    }
    if (app_lv_res_local.scene_b_vis != NULL) {
        lv_obj_remove_event_cb(app_lv_res_local.scene_b_vis, app_lv_scene_float_event);
        app_lv_res_local.scene_b_vis  = NULL;
    }
    if (app_lv_res_local.scene_l_vis != NULL) {
        lv_obj_remove_event_cb(app_lv_res_local.scene_l_vis, app_lv_scene_float_event);
        app_lv_res_local.scene_l_vis  = NULL;
    }
    if (app_lv_res_local.scene_r_vis != NULL) {
        lv_obj_remove_event_cb(app_lv_res_local.scene_r_vis, app_lv_scene_float_event);
        app_lv_res_local.scene_r_vis  = NULL;
    }
    if (app_lv_res_local.scene_t != NULL) {
        lv_obj_del(app_lv_res_local.scene_t);
        app_lv_res_local.scene_t  = NULL;
    }
    if (app_lv_res_local.scene_b != NULL) {
        lv_obj_del(app_lv_res_local.scene_b);
        app_lv_res_local.scene_b  = NULL;
    }
    if (app_lv_res_local.scene_l != NULL) {
        lv_obj_del(app_lv_res_local.scene_l);
        app_lv_res_local.scene_l  = NULL;
    }
    if (app_lv_res_local.scene_r != NULL) {
        lv_obj_del(app_lv_res_local.scene_r);
        app_lv_res_local.scene_r  = NULL;
    }
}

/*@brief 浮动窗口内部场景资源生成
 */
static void app_lv_scene_float_new(void)
{
    /* 设置默认风格 */
    if (1) {
        /* 初始化风格 */
        lv_style_init(&app_lv_res_local.style);
        lv_style_set_pad_all(&app_lv_res_local.style, 0);
        lv_style_set_opa(&app_lv_res_local.style, APP_LV_SCENE_FLOAT_OPA);
        lv_style_set_bg_opa(&app_lv_res_local.style, 0);
        lv_style_set_bg_color(&app_lv_res_local.style, lv_color_black());
        lv_style_set_border_side(&app_lv_res_local.style, 0);
        lv_style_set_border_width(&app_lv_res_local.style, 0);
        lv_style_set_border_color(&app_lv_res_local.style, lv_color_black());
    }
    /* 上浮动窗口 */
    if (app_lv_res_scene.scene_t != NULL)
    if (app_lv_res_scene.scene_t->self != NULL) {
        app_lv_res_local.scene_t = lv_obj_create(lv_scr_act());
        #if 1
        /* 构建浮动主窗 */
        lv_obj_t *f_win = app_lv_res_local.scene_t;
        lv_obj_remove_style_all(f_win);
        lv_obj_add_style(f_win, &app_lv_res_local.style, 0);
        lv_obj_set_style_opa(f_win, 0, 0);
        lv_obj_clear_flag(f_win, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(f_win, LV_OBJ_FLAG_SCROLL_ELASTIC);
        lv_obj_set_scroll_snap_y(f_win, LV_SCROLL_SNAP_CENTER);
        lv_obj_set_size(f_win, LV_HOR_RES, LV_VER_RES);
        /* 显示域:用于挂载显示窗口(浮动子窗口) */
        lv_obj_t *f_win_vis = lv_obj_create(f_win);
        lv_obj_add_style(f_win_vis, &app_lv_res_local.style, 0);
        lv_obj_add_flag(f_win_vis, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(f_win_vis, LV_OBJ_FLAG_SCROLL_ELASTIC);
        lv_obj_clear_flag(f_win_vis, LV_OBJ_FLAG_SNAPABLE);
        lv_obj_set_scroll_dir(f_win_vis, LV_DIR_VER);
        lv_obj_set_style_opa(f_win_vis, 0, 0);
        lv_obj_set_size(f_win_vis, LV_HOR_RES, LV_VER_RES + APP_LV_SCENE_FLOAT_PAD * 2);
        lv_obj_set_y(f_win_vis, -APP_LV_SCENE_FLOAT_PAD);
        /* 下拉域: */
        lv_obj_t *f_ph = lv_obj_create(f_win);
        lv_obj_remove_style_all(f_ph);
        // lv_obj_set_style_bg_color(f_ph, lv_color_make(0xff, 0x00, 0x00), 0);
        // lv_obj_set_style_bg_opa(f_ph, LV_OPA_50, 0);
        // lv_obj_set_style_opa(f_ph, LV_OPA_50, 0);
        lv_obj_clear_flag(f_ph, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_size(f_ph, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_y(f_ph, 0);
        /* 回退域: */
        lv_obj_t *f_cb = lv_obj_create(f_win);
        lv_obj_remove_style_all(f_cb);
        // lv_obj_set_style_bg_color(f_cb, lv_color_make(0x00, 0xff, 0x00), 0);
        // lv_obj_set_style_bg_opa(f_cb, LV_OPA_50, 0);
        // lv_obj_set_style_opa(f_cb, LV_OPA_50, 0);
        lv_obj_clear_flag(f_cb, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_size(f_cb, LV_HOR_RES, LV_VER_RES - APP_LV_SCENE_FLOAT_PAD * 2);
        lv_obj_set_y(f_cb, LV_VER_RES + APP_LV_SCENE_FLOAT_PAD);
        lv_obj_scroll_to_view(f_cb, LV_ANIM_OFF);
        /* 将目标窗口降级并挂入到浮动子窗口中 */
        lv_obj_t *subwin = app_lv_res_scene.scene_t->self; 
        lv_obj_set_parent(subwin, f_win_vis);
        lv_obj_set_style_opa(subwin, APP_LV_SCENE_FLOAT_OPA, 0);
        lv_obj_center(subwin);
        /* 挂载一个响应,当结束时还原透明度 */
        app_lv_res_local.scene_t_vis = f_win_vis;
        lv_obj_add_event_cb(f_win_vis, app_lv_scene_float_event, LV_EVENT_ALL, subwin);
        #endif
    }
    /* 下浮动窗口 */
    if (app_lv_res_scene.scene_b != NULL) {
        app_lv_res_local.scene_b = lv_obj_create(lv_scr_act());
        #if 0 /* 问题点:当指针点击时,f_win_vis自动偏移到最下方,这不是预期 */
        /* 构建浮动主窗 */
        lv_obj_t *f_win = app_lv_res_local.scene_b;
        lv_obj_remove_style_all(f_win);
        lv_obj_add_style(f_win, &app_lv_res_local.style, 0);
        lv_obj_set_style_opa(f_win, 0, 0);
        lv_obj_clear_flag(f_win, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(f_win, LV_OBJ_FLAG_SCROLL_ELASTIC);
        lv_obj_set_scroll_snap_y(f_win, LV_SCROLL_SNAP_CENTER);
        lv_obj_set_size(f_win, LV_HOR_RES, LV_VER_RES);
        /* 显示域:用于挂载显示窗口(浮动子窗口) */
        lv_obj_t *f_win_vis = lv_obj_create(f_win);
        lv_obj_add_style(f_win_vis, &app_lv_res_local.style, 0);
        lv_obj_add_flag(f_win_vis, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(f_win_vis, LV_OBJ_FLAG_SCROLL_ELASTIC);
        lv_obj_clear_flag(f_win_vis, LV_OBJ_FLAG_SNAPABLE);
        lv_obj_set_scroll_dir(f_win_vis, LV_DIR_VER);
        lv_obj_set_style_opa(f_win_vis, 0, 0);
        lv_obj_set_size(f_win_vis, LV_HOR_RES, LV_VER_RES + APP_LV_SCENE_FLOAT_PAD * 2);
        lv_obj_set_y(f_win_vis, -APP_LV_SCENE_FLOAT_PAD);
        /* 上拉域: */
        lv_obj_t *f_ph = lv_obj_create(f_win);
        lv_obj_remove_style_all(f_ph);
        // lv_obj_set_style_bg_color(f_ph, lv_color_make(0xff, 0x00, 0x00), 0);
        // lv_obj_set_style_bg_opa(f_ph, LV_OPA_50, 0);
        // lv_obj_set_style_opa(f_ph, LV_OPA_50, 0);
        lv_obj_clear_flag(f_ph, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_size(f_ph, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_y(f_ph, 0);
        /* 回退域: */
        lv_obj_t *f_cb = lv_obj_create(f_win);
        lv_obj_remove_style_all(f_cb);
        // lv_obj_set_style_bg_color(f_cb, lv_color_make(0x00, 0xff, 0x00), 0);
        // lv_obj_set_style_bg_opa(f_cb, LV_OPA_50, 0);
        // lv_obj_set_style_opa(f_cb, LV_OPA_50, 0);
        lv_obj_clear_flag(f_cb, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_size(f_cb, LV_HOR_RES, LV_VER_RES - APP_LV_SCENE_FLOAT_PAD * 2);
        lv_obj_set_y(f_cb, - LV_VER_RES + APP_LV_SCENE_FLOAT_PAD);
        lv_obj_scroll_to_view(f_cb, LV_ANIM_OFF);
        /* 将目标窗口降级并挂入到浮动子窗口中 */
        lv_obj_t *subwin = app_lv_res_scene.scene_b->self; 
        lv_obj_set_parent(subwin, f_win_vis);
        lv_obj_set_style_opa(subwin, APP_LV_SCENE_FLOAT_OPA, 0);
        lv_obj_center(subwin);
        /* 挂载一个响应,当结束时还原透明度 */
        app_lv_res_local.scene_t_vis = f_win_vis;
        lv_obj_add_event_cb(f_win_vis, app_lv_scene_float_event, LV_EVENT_ALL, subwin);
        #endif
    }
    /* 左浮动窗口 */
    if (app_lv_res_scene.scene_l != NULL) {
        app_lv_res_local.scene_l = lv_obj_create(lv_scr_act());
    }
    /* 右浮动窗口 */
    if (app_lv_res_scene.scene_r != NULL) {
        app_lv_res_local.scene_r = lv_obj_create(lv_scr_act());
    }
}

/*@brief     准备并就绪浮动窗口
 *           注意:它调用了敏感资源
 *           对它的调用不能递归到lv_timer_handler
 *@param[in] t 上浮动窗口
 *@param[in] b 下浮动窗口
 *@param[in] l 左浮动窗口
 *@param[in] r 右浮动窗口
 */
void app_lv_scene_float_show(app_lv_scene_t *t, app_lv_scene_t *b,
                             app_lv_scene_t *l, app_lv_scene_t *r)
{
    app_lv_scene_float_del();
    app_lv_scene_float_refr(t, b, l ,r);
    app_lv_scene_float_new();
}

/*@brief 准备并就绪浮动窗口
 *       注意:它调用了敏感资源
 *       对它的调用不能递归到lv_timer_handler
 */
void app_lv_scene_float_hide(void)
{
    app_lv_scene_float_del();
    app_lv_scene_float_refr(NULL, NULL, NULL, NULL);
}
