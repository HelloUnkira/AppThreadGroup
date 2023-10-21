
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t  anim;
    lv_obj_t  *scene;
    lv_obj_t **img_list;
    lv_obj_t  *lbl_txt;
    lv_anim_t  anim_rectify;
    
    uint32_t   list_num;
    app_lv_ui_theme_item_t *list;
    /*  */
    bool       move_flag;
    lv_point_t last_pos;
    /*  */
    lv_coord_t iter_way;   // = 1
    lv_coord_t iter_angle; // = 300
    /*  */
} *app_lv_ui_res_local = NULL;

/*@brief 界面默认事件响应回调
 */
static void app_lv_event_ui_img_move_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_PRESSED: {
        lv_point_t  point = {0};
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        app_lv_ui_res_local->last_pos = point;
        app_lv_ui_res_local->move_flag = true;
        /* 关闭轮盘调度(最小颗粒) */
        app_lv_wheel_event_onoff(false);
        break;
    }
    case LV_EVENT_PRESSING: {
        lv_point_t  point = {0};
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &point);
        /* 计算位移行进 */
        lv_coord_t flag = 0;
        lv_point_t last     = app_lv_ui_res_local->last_pos;
        lv_coord_t circle_x = LV_HOR_RES / 2;
        lv_coord_t circle_y = LV_VER_RES / 2;
        lv_coord_t diameter = LV_MIN(LV_HOR_RES, LV_VER_RES);
        lv_coord_t delta_x  = LV_MAX(last.x, point.x) - LV_MIN(last.x, point.x);
        lv_coord_t delta_y  = LV_MAX(last.y, point.y) - LV_MIN(last.y, point.y);
        lv_coord_t delta_p  = LV_MAX(delta_x, delta_y);
        /* 计算增量比例,以最大值做贡献 */
        /* 先判断方向(以圆心为分割线四个象限的运动) */
        if ((last.x > circle_x && point.y > last.y && delta_x <= delta_y) ||
            (last.x < circle_x && point.y < last.y && delta_x <= delta_y) ||
            (last.y < circle_y && point.x > last.x && delta_x >= delta_y) ||
            (last.y > circle_y && point.x < last.x && delta_x >= delta_y))
            app_lv_ui_res_local->iter_way = flag = +1;
        else
            app_lv_ui_res_local->iter_way = flag = -1;
        /* 位移向量映射到直径上,直径是180度,做出比值成偏转角 */
        app_lv_ui_res_local->iter_angle += flag * delta_p * 180 / diameter;
        /* 手动刷新动画 */
        static void app_lv_ui_local_anim_handler(void *para, int32_t value);
        app_lv_ui_local_anim_handler(NULL, 0);
        app_lv_ui_res_local->last_pos = point;
        app_lv_ui_res_local->move_flag = true;
        break;
    }
    case LV_EVENT_RELEASED: {
        app_lv_ui_res_local->move_flag = false;
        /* 恢复轮盘调度(最小颗粒) */
        app_lv_wheel_event_onoff(false);
        break;
    }
    /* 编码器事件(暂存档): */
    /* CLOCKWISE:
     * app_lv_ui_res_local->iter_angle += 1;
     * app_lv_ui_res_local->iter_way    = +1;
     * ANTICLOCKWISE:
     * app_lv_ui_res_local->iter_angle -= 1;
     * app_lv_ui_res_local->iter_way    = -1;
     */
    default:
        break;
    break;
    }
}

/*@brief 界面默认事件响应回调
 */
static void app_lv_event_ui_btn_txt_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        int16_t angle_bse = app_lv_ui_res_local->iter_angle / APP_LV_UI_THUMB_WHEEL_UNIT;
        #if 1
        int16_t idx = 180 / APP_LV_UI_THUMB_WHEEL_UNIT;
        #else
        int16_t idx = (int16_t)(uintptr_t)lv_event_get_user_data(e);
        #endif
        int16_t pic_ofs = app_sys_mod_abs(idx - angle_bse, (int16_t)app_lv_ui_res_local->list_num);
        app_lv_scene_add(app_lv_ui_res_local->list[pic_ofs].scene, false);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面默认事件响应回调(本地重定向)
 */
static void app_lv_event_ui_default_redirect(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    /* 手势事件重定向为空 */
    if (code == LV_EVENT_GESTURE) {
        return;
    }
    /* 其他事件不做重定向 */
    app_lv_event_default_cb(e);
}

/*@brief 界面动画定时器回调
 */
static void app_lv_anim_ui_rectify_handler(void *para, int32_t value)
{
    /* 移动时不响应动画 */
    if (app_lv_ui_res_local->move_flag)
        return;
    /* 动画是否需要执行 */
    if (app_lv_ui_res_local->iter_angle % APP_LV_UI_THUMB_WHEEL_UNIT == 0)
        return;
    /* 角度跳跃式(调整优化) */
    lv_coord_t angle_jump = APP_LV_UI_THUMB_WHEEL_UNIT / APP_LV_UI_THUMB_WHEEL_RECTIFY;
    lv_coord_t angle_abs  = app_sys_mod_abs(app_lv_ui_res_local->iter_angle, APP_LV_UI_THUMB_WHEEL_UNIT);
    lv_coord_t angle_ofs  = app_lv_ui_res_local->iter_way > 0 ? APP_LV_UI_THUMB_WHEEL_UNIT - angle_abs : angle_abs;
    angle_ofs %= angle_jump;
    angle_ofs  = angle_ofs != 0 ? angle_ofs : angle_jump;
    /* 因为是跳跃角度,所以需要有至少一次偏转补足 */
    if (app_lv_ui_res_local->iter_angle % APP_LV_UI_THUMB_WHEEL_UNIT != 0)
        app_lv_ui_res_local->iter_angle += app_lv_ui_res_local->iter_way * angle_ofs;
    /* 手动刷新动画 */
    static void app_lv_ui_local_anim_handler(void *para, int32_t value);
    app_lv_ui_local_anim_handler(NULL, 0);
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 从图片组随便取一张 */
    lv_img_header_t img_header = {0};
    const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + APP_LV_UI_THUMB_WHEEL_OFS);
    if (lv_img_decoder_get_info(img_str, &img_header) != LV_RES_OK) {
        APP_SYS_LOG_WARN("can't take pic src");
        return;
    }
    /* 获得图片的宽和高(应该是一样的长度) */
    if (img_header.w != img_header.h) {
        APP_SYS_LOG_WARN("pic src is not quadrate:<w,h><%d,%d>", img_header.w, img_header.h);
        return;
    }
    
    lv_coord_t circle_x = LV_HOR_RES / 2;
    lv_coord_t circle_y = LV_VER_RES / 2;
    lv_coord_t pic_diameter = img_header.w;
    lv_coord_t pic_distance = LV_MIN(LV_HOR_RES, LV_VER_RES) / 2 - pic_diameter / 2 - APP_LV_UI_THUMB_WHEEL_SPAN;
    int16_t angle_bse = app_lv_ui_res_local->iter_angle / APP_LV_UI_THUMB_WHEEL_UNIT;
    int16_t angle_ofs = app_lv_ui_res_local->iter_angle % APP_LV_UI_THUMB_WHEEL_UNIT;
    APP_SYS_LOG_DEBUG("pic_diameter:%d", pic_diameter);
    APP_SYS_LOG_DEBUG("pic_distance:%d", pic_distance);
    APP_SYS_LOG_DEBUG("angle_bse:%d", angle_bse);
    APP_SYS_LOG_DEBUG("angle_ofs:%d", angle_ofs);
    
    for (int16_t angle = 0; angle < 360; angle += APP_LV_UI_THUMB_WHEEL_UNIT) {
        lv_coord_t pic_x = ((int32_t)lv_trigo_cos(angle + angle_ofs) * pic_distance) >> LV_TRIGO_SHIFT;
        lv_coord_t pic_y = ((int32_t)lv_trigo_sin(angle + angle_ofs) * pic_distance) >> LV_TRIGO_SHIFT;
        APP_SYS_LOG_DEBUG("<pic_x,pic_y><%d,%d>", pic_x, pic_y);
        lv_coord_t pic_pos_x = circle_x + pic_x - pic_diameter / 2;
        lv_coord_t pic_pos_y = circle_y + pic_y - pic_diameter / 2;
        APP_SYS_LOG_DEBUG("<pic_pos_x,pic_pos_y><%d,%d>", pic_pos_x, pic_pos_y);
        
        int16_t idx = angle / APP_LV_UI_THUMB_WHEEL_UNIT;
        int16_t pic_ofs = app_sys_mod_abs(idx - angle_bse, (int16_t)app_lv_ui_res_local->list_num);
        APP_SYS_LOG_DEBUG("<idx, pic_ofs><%d, %d>", idx, pic_ofs);
        const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[pic_ofs].idx_pic + APP_LV_UI_THUMB_WHEEL_OFS);
        lv_img_set_src(app_lv_ui_res_local->img_list[idx], img_str);
        lv_obj_set_pos(app_lv_ui_res_local->img_list[idx], pic_pos_x, pic_pos_y);
        /* 画文本: */
        if (angle == 180) {
            const char *idx_str = app_lv_lang_str_find(app_lv_ui_res_local->list[pic_ofs].idx_str);
            lv_label_set_text(app_lv_ui_res_local->lbl_txt, idx_str);
        }
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
void app_lv_ui_thumb_wheel_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        app_lv_ui_theme_list_alloc(&app_lv_ui_res_local->list, &app_lv_ui_res_local->list_num);
        #if APP_LV_DEVELOPER_MODEL      //开发者模式跳过第一个条目,不需要
        app_lv_ui_res_local->list++;
        app_lv_ui_res_local->list_num--;
        #endif
        /* 禁用默认事件响应,事件重定向使用 */
        app_lv_event_default_config(NULL, false, NULL);
        app_lv_event_default_config(NULL, true,  app_lv_event_ui_default_redirect);
        /* 界面重转接事件 */
        lv_obj_add_event_cb(app_lv_ui_res_local->scene, app_lv_event_ui_img_move_cb, LV_EVENT_ALL, NULL);
        /* 创建图片资源控件组 */
        APP_SYS_ASSERT(360 % APP_LV_UI_THUMB_WHEEL_UNIT == 0);
        app_lv_ui_res_local->img_list = lv_mem_alloc(sizeof(lv_obj_t *) * (360 / APP_LV_UI_THUMB_WHEEL_UNIT));
        for (uint8_t idx = 0; idx < 360 / APP_LV_UI_THUMB_WHEEL_UNIT; idx++) {
            app_lv_ui_res_local->img_list[idx] = lv_img_create(app_lv_ui_res_local->scene);
            app_lv_style_object(app_lv_ui_res_local->img_list[idx]);
            lv_obj_set_style_bg_opa(app_lv_ui_res_local->img_list[idx], LV_OPA_TRANSP, 0);
        }
        lv_obj_t *btn = lv_btn_create(app_lv_ui_res_local->scene);
        app_lv_style_object(btn);
        lv_obj_add_event_cb(btn, app_lv_event_ui_btn_txt_cb, LV_EVENT_ALL, NULL);
        lv_obj_center(btn);
        app_lv_ui_res_local->lbl_txt = lv_label_create(btn);
        app_lv_style_object(app_lv_ui_res_local->lbl_txt);
        lv_obj_set_style_text_align(app_lv_ui_res_local->lbl_txt, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_width(app_lv_ui_res_local->lbl_txt, app_lv_style_hor_pct(50));
        lv_label_set_long_mode(app_lv_ui_res_local->lbl_txt, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_center(app_lv_ui_res_local->lbl_txt);
        /* 初始化拨正动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim_rectify,
                                 app_lv_anim_ui_rectify_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 30, 1000);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 1, 1000);
    }
    app_lv_ui_res_local->move_flag  = false;
    app_lv_ui_res_local->iter_way   = 0;
    app_lv_ui_res_local->iter_angle = 0;
    app_lv_ui_local_anim_handler(NULL, 0);
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
void app_lv_ui_thumb_wheel_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化拨正动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_anim_ui_rectify_handler);
        /* 启用默认事件响应,事件重定向取消 */
        app_lv_event_default_config(NULL, false, app_lv_event_ui_default_redirect);
        app_lv_event_default_config(NULL, true,  NULL);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        #if APP_LV_DEVELOPER_MODEL
        app_lv_ui_res_local->list--;
        #endif
        app_lv_ui_theme_list_free(app_lv_ui_res_local->list);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}
