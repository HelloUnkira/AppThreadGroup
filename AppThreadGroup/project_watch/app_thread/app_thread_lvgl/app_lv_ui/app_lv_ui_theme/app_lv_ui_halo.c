
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t  anim;
    lv_obj_t  *scene;
    lv_obj_t **img_list_max;
    lv_obj_t **img_list_min;
    lv_anim_t  anim_rectify;
    /*  */
    uint32_t   list_num;
    app_lv_ui_theme_item_t *list;
    /*  */
    lv_coord_t iter_way;
    lv_coord_t iter_skew;
    lv_coord_t iter_route;
    /*  */
} *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 从图片组随便取一张 */
    lv_img_header_t img_max_header = {0};
    lv_img_header_t img_min_header = {0};
    const char *img_max_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + APP_LV_UI_HALO_OFS_MAX);
    const char *img_min_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + APP_LV_UI_HALO_OFS_MIN);
    if (lv_img_decoder_get_info(img_max_str, &img_max_header) != LV_RES_OK ||
        lv_img_decoder_get_info(img_min_str, &img_min_header) != LV_RES_OK) {
        APP_SYS_LOG_WARN("can't take pic src");
        APP_SYS_LOG_WARN("%s", img_max_str);
        APP_SYS_LOG_WARN("%s", img_min_str);
        return;
    }
    /* 获得图片的宽和高(应该是一样的长度) */
    if (img_max_header.w != img_max_header.h ||
        img_min_header.w != img_min_header.h) {
        APP_SYS_LOG_WARN("pic is not quadrate");
        APP_SYS_LOG_WARN("%s<w:%d,h:%d>", img_max_str, img_max_header.w, img_max_header.h);
        APP_SYS_LOG_WARN("%s<w:%d,h:%d>", img_min_str, img_min_header.w, img_min_header.h);
        return;
    }
    
    lv_coord_t circle_x = LV_HOR_RES / 2;
    lv_coord_t circle_y = LV_VER_RES / 2;
    /* 俩个圈,一个大圈一个小圈交替变化 */
    lv_coord_t ring_dis_max = LV_MIN(LV_HOR_RES, LV_VER_RES) / 2 - img_max_header.w / 2 - APP_LV_UI_HALO_SPAN_MAX;
    lv_coord_t ring_dis_min = img_min_header.w / 2 + APP_LV_UI_HALO_SPAN_MIN;
    APP_SYS_LOG_INFO("ring_dis_max:%d", ring_dis_max);
    APP_SYS_LOG_INFO("ring_dis_min:%d", ring_dis_min);
    /* 小圈位置,大圈位置,交替变化的偏转起点与终点 */
    app_lv_ui_res_local->iter_route = ring_dis_max - ring_dis_min;
    APP_SYS_LOG_INFO("iter_skew:%d",  app_lv_ui_res_local->iter_skew);
    APP_SYS_LOG_INFO("iter_route:%d", app_lv_ui_res_local->iter_route);
    /* 根据起始结束之间的位移计算图片映射,确认每隔多少个单位图片变化一次 */
    lv_coord_t ring_ofs_skew = app_sys_mabs(app_lv_ui_res_local->iter_skew, ring_dis_max - ring_dis_min);
    lv_coord_t ring_dis_span = app_lv_ui_res_local->iter_route / (APP_LV_UI_HALO_OFS_MAX - APP_LV_UI_HALO_OFS_MIN);
    APP_SYS_LOG_INFO("ring_dis_span:%d", ring_dis_span);
    /* 当次的skew需要选择的图片尺寸以及对应尺寸的直径 */
    lv_coord_t pic_ofs_max  = APP_LV_UI_HALO_OFS_MAX - ring_ofs_skew / ring_dis_span;
    lv_coord_t pic_ofs_min  = APP_LV_UI_HALO_OFS_MIN + ring_ofs_skew / ring_dis_span;
    pic_ofs_max = pic_ofs_max >= APP_LV_UI_HALO_OFS_MIN ? pic_ofs_max : APP_LV_UI_HALO_OFS_MIN;
    pic_ofs_min = pic_ofs_min <= APP_LV_UI_HALO_OFS_MAX ? pic_ofs_min : APP_LV_UI_HALO_OFS_MAX;
    APP_SYS_LOG_INFO("pic_ofs_max:%d", pic_ofs_max);
    APP_SYS_LOG_INFO("pic_ofs_min:%d", pic_ofs_min);
    const char *img_max_cur_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + pic_ofs_max);
    const char *img_min_cur_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + pic_ofs_min);
    if (lv_img_decoder_get_info(img_max_cur_str, &img_max_header) != LV_RES_OK ||
        lv_img_decoder_get_info(img_min_cur_str, &img_min_header) != LV_RES_OK) {
        APP_SYS_LOG_WARN("can't take pic src");
        APP_SYS_LOG_WARN("%s", img_max_cur_str);
        APP_SYS_LOG_WARN("%s", img_min_cur_str);
        return;
    }
    /* 获得图片的宽和高(应该是一样的长度) */
    if (img_max_header.w != img_max_header.h ||
        img_min_header.w != img_min_header.h) {
        APP_SYS_LOG_WARN("pic src is not quadrate");
        APP_SYS_LOG_WARN("%s<w:%d,h:%d>", img_max_cur_str, img_max_header.w, img_max_header.h);
        APP_SYS_LOG_WARN("%s<w:%d,h:%d>", img_min_cur_str, img_min_header.w, img_min_header.h);
        return;
    }
    lv_coord_t pic_dis_max = img_max_header.w;
    lv_coord_t pic_dis_min = img_min_header.w;
    APP_SYS_LOG_INFO("pic_dis_max:%d", pic_dis_max);
    APP_SYS_LOG_INFO("pic_dis_min:%d", pic_dis_min);
    /* 先计算理论的max和min */
    ring_dis_max -= ring_ofs_skew;  // 这是理论上的max
    ring_dis_min += ring_ofs_skew;  // 这是理论上的min
    APP_SYS_LOG_INFO("ring_dis_max:%d", ring_dis_max);
    APP_SYS_LOG_INFO("ring_dis_min:%d", ring_dis_min);
    /* skew影响的当次显示图片组的偏转 */
    int16_t skew_bse = (int16_t)LV_ABS(app_lv_ui_res_local->iter_skew) / (int16_t)app_lv_ui_res_local->iter_route;
    int16_t skew_ofs = (int16_t)LV_ABS(app_lv_ui_res_local->iter_skew) % (int16_t)app_lv_ui_res_local->iter_route;
    APP_SYS_LOG_INFO("skew_bse:%d", skew_bse);
    APP_SYS_LOG_INFO("skew_ofs:%d", skew_ofs);
    /* 这里加一个映射让它内外动的时候做一次环形偏转 */
    int16_t angle_bse = skew_bse * (360 / APP_LV_UI_HALO_UNIT);
    int16_t angle_ofs = app_sys_map(skew_ofs, 0, (int16_t)app_lv_ui_res_local->iter_route, 0, APP_LV_UI_HALO_UNIT / 2);
    APP_SYS_LOG_INFO("angle_bse:%d", angle_bse);
    APP_SYS_LOG_INFO("angle_ofs:%d", angle_ofs);
    int16_t angle_ofs1 = angle_ofs;
    int16_t angle_ofs2 = APP_LV_UI_HALO_UNIT / 2 - angle_ofs;
    APP_SYS_LOG_INFO("angle_ofs1:%d", angle_ofs1);
    APP_SYS_LOG_INFO("angle_ofs2:%d", angle_ofs2);
    APP_SYS_LOG_INFO("pic_off:%d, pic_min:%d", APP_LV_UI_HALO_OFS_MAX - skew_bse / ring_dis_span, APP_LV_UI_HALO_OFS_MIN);
    APP_SYS_LOG_INFO("pic_off:%d, pic_max:%d", APP_LV_UI_HALO_OFS_MIN + skew_bse / ring_dis_span, APP_LV_UI_HALO_OFS_MAX);
    /* 先画外部大圈圈,它跟随偏移量向内部收缩 */
    /* 外部大圈圈是向正迭代,顺时钟为+ */
    for (int16_t angle = 0; angle < 360; angle += APP_LV_UI_HALO_UNIT) {
        lv_coord_t pic_x = ((lv_coord_t)lv_trigo_cos(angle + angle_ofs1) * ring_dis_max) >> LV_TRIGO_SHIFT;
        lv_coord_t pic_y = ((lv_coord_t)lv_trigo_sin(angle + angle_ofs1) * ring_dis_max) >> LV_TRIGO_SHIFT;
        APP_SYS_LOG_DEBUG("<pic_x,pic_y><%d,%d>", pic_x, pic_y);
        lv_coord_t pic_pos_x = circle_x + pic_x - pic_dis_max / 2;
        lv_coord_t pic_pos_y = circle_y + pic_y - pic_dis_max / 2;
        APP_SYS_LOG_DEBUG("<pic_pos_x,pic_pos_y><%d,%d>", pic_pos_x, pic_pos_y);
        
        int16_t idx = angle / APP_LV_UI_HALO_UNIT;
        int16_t ofs = app_lv_ui_res_local->iter_skew >= 0 ? 0 : (360 / APP_LV_UI_HALO_UNIT);
        int16_t pic_ofs = app_sys_mabs(idx - angle_bse - ofs, (int16_t)app_lv_ui_res_local->list_num);
        APP_SYS_LOG_DEBUG("<idx, pic_ofs><%d, %d>", idx, pic_ofs);
        const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[pic_ofs].idx_pic + pic_ofs_max);
        lv_img_set_src(app_lv_ui_res_local->img_list_max[idx], img_str);
        lv_obj_set_pos(app_lv_ui_res_local->img_list_max[idx], pic_pos_x, pic_pos_y);
    }
    /* 再画内部小圈圈,它跟随偏移量向外部扩展 */
    /* 内部小圈圈是向逆迭代,逆时钟为- */
    for (int16_t angle = 0; angle < 360; angle += APP_LV_UI_HALO_UNIT) {
        lv_coord_t pic_x = ((lv_coord_t)lv_trigo_cos(angle + angle_ofs2) * ring_dis_min) >> LV_TRIGO_SHIFT;
        lv_coord_t pic_y = ((lv_coord_t)lv_trigo_sin(angle + angle_ofs2) * ring_dis_min) >> LV_TRIGO_SHIFT;
        APP_SYS_LOG_DEBUG("<pic_x,pic_y><%d,%d>", pic_x, pic_y);
        lv_coord_t pic_pos_x = circle_x + pic_x - pic_dis_min / 2;
        lv_coord_t pic_pos_y = circle_y + pic_y - pic_dis_min / 2;
        APP_SYS_LOG_DEBUG("<pic_pos_x,pic_pos_y><%d,%d>", pic_pos_x, pic_pos_y);
        
        int16_t idx = angle / APP_LV_UI_HALO_UNIT;
        int16_t ofs = app_lv_ui_res_local->iter_skew >= 0 ? (360 / APP_LV_UI_HALO_UNIT) : 0;
        int16_t pic_ofs = app_sys_mabs(idx - angle_bse - ofs, (int16_t)app_lv_ui_res_local->list_num);
        APP_SYS_LOG_DEBUG("<idx, pic_ofs><%d, %d>", idx, pic_ofs);
        const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[pic_ofs].idx_pic + pic_ofs_min);
        lv_img_set_src(app_lv_ui_res_local->img_list_min[idx], img_str);
        lv_obj_set_pos(app_lv_ui_res_local->img_list_min[idx], pic_pos_x, pic_pos_y);
    }
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_anim_rectify_handler(void *para, int32_t value)
{
    /* 动画是否需要执行 */
    if (app_lv_ui_res_local->iter_route == 0)
        return;
    if (app_lv_ui_res_local->iter_skew % app_lv_ui_res_local->iter_route == 0)
        return;
    /* 角度跳跃式(调整优化) */
    lv_coord_t skew_jump = app_sys_map(APP_LV_UI_HALO_UNIT / APP_LV_UI_HALO_RECTIFY, 0, APP_LV_UI_HALO_UNIT, 0, app_lv_ui_res_local->iter_route);
    lv_coord_t skew_abs  = app_sys_mabs(app_lv_ui_res_local->iter_skew, app_lv_ui_res_local->iter_route);
    lv_coord_t skew_ofs  = app_lv_ui_res_local->iter_way > 0 ? app_lv_ui_res_local->iter_route - skew_abs : skew_abs;
    skew_ofs %= skew_jump;
    skew_ofs  = skew_ofs != 0 ? skew_ofs : skew_jump;
    /* 因为是跳跃角度,所以需要有至少一次偏转补足 */
    if (app_lv_ui_res_local->iter_skew  % app_lv_ui_res_local->iter_route != 0)
        app_lv_ui_res_local->iter_skew += app_lv_ui_res_local->iter_way * skew_ofs;
    /* 手动刷新动画 */
    app_lv_ui_local_anim_handler(NULL, 0);
}

/*@brief 界面默认事件响应回调
 */
static void app_lv_ui_event_img_move_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_PRESSED: {
        /* 关闭轮盘调度(最小颗粒) */
        app_lv_wheel_event_onoff(false);
        break;
    }
    case LV_EVENT_PRESSING: {
        break;
    }
    case LV_EVENT_RELEASED: {
        /* 恢复轮盘调度(最小颗粒) */
        app_lv_wheel_event_onoff(true);
        break;
    }
    default:
        break;
    }
}

/*@brief 界面默认事件响应回调
 */
static void app_lv_ui_event_img_list_max_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        int16_t idx = (int16_t)(uintptr_t)lv_event_get_user_data(e);
        int16_t ofs = app_lv_ui_res_local->iter_skew >= 0 ? 0 : (360 / APP_LV_UI_HALO_UNIT);
        int16_t skew_bse  = (int16_t)LV_ABS(app_lv_ui_res_local->iter_skew) / (int16_t)app_lv_ui_res_local->iter_route;
        int16_t angle_bse = skew_bse * (360 / APP_LV_UI_HALO_UNIT);
        int16_t pic_ofs = app_sys_mabs(idx - angle_bse - ofs, (int16_t)app_lv_ui_res_local->list_num);
        app_lv_scene_add(app_lv_ui_res_local->list[pic_ofs].scene, false);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面默认事件响应回调
 */
static void app_lv_ui_event_img_list_min_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        int16_t idx = (int16_t)(uintptr_t)lv_event_get_user_data(e);
        int16_t ofs = app_lv_ui_res_local->iter_skew >= 0 ? (360 / APP_LV_UI_HALO_UNIT) : 0;
        int16_t skew_bse  = (int16_t)LV_ABS(app_lv_ui_res_local->iter_skew) / (int16_t)app_lv_ui_res_local->iter_route;
        int16_t angle_bse = skew_bse * (360 / APP_LV_UI_HALO_UNIT);
        int16_t pic_ofs = app_sys_mabs(idx - angle_bse - ofs, (int16_t)app_lv_ui_res_local->list_num);
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
static void app_lv_ui_event_default_redirect(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    /* 手势事件重定向 */
    case LV_EVENT_GESTURE: {
        lv_indev_wait_release(lv_event_get_indev(e));
        /* 一点点小小的拨动,让动画跑起来 */
        app_lv_ui_res_local->iter_skew += 1;
        app_lv_ui_res_local->iter_way   = +1;
        /* 手动刷新动画 */
        app_lv_ui_local_anim_handler(NULL, 0);
        return;
    }
    /* 编码器事件: */
    case LV_EVENT_KEY: {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        
        if (key == APP_LV_EVENT_CLOCKWISE ||
            key == APP_LV_EVENT_CLOCKWISE_ANTI) {
            app_lv_ui_res_local->iter_skew += 1;
            app_lv_ui_res_local->iter_way   = +1;
            /* 手动刷新动画 */
            app_lv_ui_local_anim_handler(NULL, 0);
        }
        break;
    }
    default:
        break;
    }
    /* 其他事件不做重定向 */
    app_lv_event_default_cb(e);
}

/*@brief 界面显示
 *@param scene 场景
 */
void app_lv_ui_halo_show(void *scene)
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
        app_lv_event_default_config(NULL, true,  app_lv_ui_event_default_redirect);
        /* 界面重转接事件 */
        lv_obj_add_event_cb(app_lv_ui_res_local->scene, app_lv_ui_event_img_move_cb, LV_EVENT_ALL, NULL);
        /* 创建图片资源控件组 */
        APP_SYS_ASSERT(360 % APP_LV_UI_HALO_UNIT == 0);
        app_lv_ui_res_local->img_list_max = lv_mem_alloc(sizeof(lv_obj_t *) * (360 / APP_LV_UI_HALO_UNIT));
        app_lv_ui_res_local->img_list_min = lv_mem_alloc(sizeof(lv_obj_t *) * (360 / APP_LV_UI_HALO_UNIT));
        for (uint8_t idx = 0; idx < 360 / APP_LV_UI_HALO_UNIT; idx++) {
            app_lv_ui_res_local->img_list_max[idx] = lv_img_create(app_lv_ui_res_local->scene);
            app_lv_style_object(app_lv_ui_res_local->img_list_max[idx]);
            lv_obj_set_style_bg_opa(app_lv_ui_res_local->img_list_max[idx], LV_OPA_TRANSP, 0);
            lv_obj_add_flag(app_lv_ui_res_local->img_list_max[idx], LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(app_lv_ui_res_local->img_list_max[idx], app_lv_ui_event_img_list_max_cb, LV_EVENT_ALL, (void *)(uintptr_t)idx);
            app_lv_ui_res_local->img_list_min[idx] = lv_img_create(app_lv_ui_res_local->scene);
            app_lv_style_object(app_lv_ui_res_local->img_list_min[idx]);
            lv_obj_set_style_bg_opa(app_lv_ui_res_local->img_list_min[idx], LV_OPA_TRANSP, 0);
            lv_obj_add_flag(app_lv_ui_res_local->img_list_min[idx], LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(app_lv_ui_res_local->img_list_min[idx], app_lv_ui_event_img_list_min_cb, LV_EVENT_ALL, (void *)(uintptr_t)idx);
        }
        /* 初始化拨正动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim_rectify,
                                 app_lv_ui_anim_rectify_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 30, 1000);
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 1, 1000);
    }
    app_lv_ui_res_local->iter_way   = 0;
    app_lv_ui_res_local->iter_skew  = 0;
    app_lv_ui_res_local->iter_route = 0;
    app_lv_ui_local_anim_handler(NULL, 0);
}

/*@brief 界面隐藏
 *@param scene 场景
 */
void app_lv_ui_halo_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化拨正动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_anim_rectify_handler);
        /* 启用默认事件响应,事件重定向取消 */
        app_lv_event_default_config(NULL, false, app_lv_ui_event_default_redirect);
        app_lv_event_default_config(NULL, true,  NULL);
        #if APP_LV_DEVELOPER_MODEL
        app_lv_ui_res_local->list--;
        #endif
        app_lv_ui_theme_list_free(app_lv_ui_res_local->list);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}
