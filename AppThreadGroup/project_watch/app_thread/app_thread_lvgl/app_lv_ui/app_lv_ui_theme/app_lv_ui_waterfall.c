
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t  anim;
    lv_obj_t  *scene;
    /*  */
    lv_obj_t  *obj_list;
    /*  */
    uint32_t   list_num;
    app_lv_ui_theme_item_t *list;
    /*  */
    bool       refr_one;
    lv_coord_t ofs_base;
    /*  */
} *app_lv_ui_res_local = NULL;

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 从图片组随便取一张 */
    lv_img_header_t img_max_header = {0};
    lv_img_header_t img_min_header = {0};
    const char *img_max_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + APP_LV_UI_WATERFALL_OFS_MAX);
    const char *img_min_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + APP_LV_UI_WATERFALL_OFS_MIN);
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
    
    uint32_t child_cnt = lv_obj_get_child_cnt(app_lv_ui_res_local->obj_list);
    
    /* 更新一次布局 */
    if (app_lv_ui_res_local->refr_one) {
        app_lv_ui_res_local->refr_one = false;
        for (uint32_t idx = 0; idx < child_cnt; idx++) {
            lv_obj_t *obj = lv_obj_get_child(app_lv_ui_res_local->obj_list, idx);
            lv_obj_set_size(obj, img_max_header.w, img_max_header.h);
            if (idx % 3 != 1)
                lv_obj_set_width(obj, (LV_HOR_RES - img_max_header.w) / 2 - app_lv_style_hor_pct(6));
        }
        /* 计算居中对齐的初始偏移量,且居中对齐 */
        APP_SYS_ASSERT(APP_LV_UI_WATERFALL_NUM % 2 != 0);
        APP_SYS_ASSERT(img_max_header.h * APP_LV_UI_WATERFALL_NUM >= LV_VER_RES);
        app_lv_ui_res_local->ofs_base = (img_max_header.h * APP_LV_UI_WATERFALL_NUM - LV_VER_RES) / 2 + app_lv_style_ver_pct(2);
        lv_obj_scroll_to_y(app_lv_ui_res_local->obj_list, app_lv_ui_res_local->ofs_base, LV_ANIM_OFF);
        APP_SYS_LOG_DEBUG("ofs_base:%d", app_lv_ui_res_local->ofs_base);
        return;
    }

    lv_coord_t center_x = LV_HOR_RES / 2;
    lv_coord_t center_y = LV_VER_RES / 2;
    
    lv_sqrt_res_t sqrt_res = {0};
    lv_sqrt(center_x * center_x + center_y * center_y, &sqrt_res, 0x8000);
    lv_coord_t roll_ofs = lv_obj_get_scroll_y(app_lv_ui_res_local->obj_list) - app_lv_ui_res_local->ofs_base;
    lv_coord_t distance = sqrt_res.i;
    APP_SYS_LOG_DEBUG("roll_ofs:%d", roll_ofs);
    APP_SYS_LOG_DEBUG("distance:%d", distance);
    
    /* 垂直调节,计算图片中心到显示中心的位置映射 */
    for (uint32_t idx = 0; idx < child_cnt; idx++) {
        lv_obj_t  *obj = lv_obj_get_child(app_lv_ui_res_local->obj_list, idx);
        lv_obj_t  *img = lv_obj_get_child(obj, 0);
        lv_coord_t ofs = img_max_header.h * (idx / 3) + img_max_header.h / 2 - roll_ofs;
        lv_coord_t dis = LV_MAX(ofs, center_y) - LV_MIN(ofs, center_y);
        /* 中心点不在显示内,设置最小尺寸 */
        if (ofs < 0 || ofs > LV_VER_RES) {
            const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[idx].idx_pic + APP_LV_UI_WATERFALL_OFS_MIN);
            lv_img_set_src(img, img_str);
            lv_obj_center(img);
            continue;
        }
        /* 列标记 */
        enum {
            app_lv_ui_local_list_left   = 0,
            app_lv_ui_local_list_center = 1,
            app_lv_ui_local_list_right  = 2,
        } type = idx % 3;
        /* 如果,最后一行 */
        if (child_cnt % 3 != 0)
        if (child_cnt / 3 == idx / 3) {
            if (child_cnt % 3 == 1)
                type = app_lv_ui_local_list_center;
            if (child_cnt % 3 == 2) {
                if (idx % 3 == 1)
                    type = app_lv_ui_local_list_left;
                if (idx % 3 == 2)
                    type = app_lv_ui_local_list_right;
            }
        }
        /* 计算图片中心到显示中心的距离 */
        if (type != app_lv_ui_local_list_center) {
            /* 水平位移固定,有一点误差但是影响可忽略 */
            lv_coord_t ofs_x = (LV_HOR_RES - img_max_header.w) / 2 / 2 + img_max_header.w / 2;
            ofs_x = LV_MAX(ofs_x, center_x) - LV_MIN(ofs_x, center_x);
            lv_sqrt(dis * dis + ofs_x * ofs_x, &sqrt_res, 0x8000);
            dis = sqrt_res.i;
            lv_obj_center(img);
        }
        APP_SYS_LOG_DEBUG("idx:%d ofs:%d dis:%d", idx, ofs, dis);
        /* 中心点在显示内,进行比例映射 */
        int32_t pic_ofs = app_sys_map(dis, (int32_t)distance, 0, APP_LV_UI_WATERFALL_OFS_MIN, APP_LV_UI_WATERFALL_OFS_MAX);
        APP_SYS_LOG_DEBUG("idx:%d pic_ofs:%d", idx, pic_ofs);
        /* 更新图片资源 */
        lv_img_header_t img_header = {0};
        const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[idx].idx_pic + pic_ofs);
        if (lv_img_decoder_get_info(img_str, &img_header) != LV_RES_OK) {
            APP_SYS_LOG_WARN("can't take pic src");
            APP_SYS_LOG_WARN("%s", img_str);
            continue;
        }
        /* 获得图片的宽和高(应该是一样的长度) */
        if (img_header.w != img_header.h) {
            APP_SYS_LOG_WARN("pic is not quadrate");
            APP_SYS_LOG_WARN("%s<w:%d,h:%d>", img_str, img_header.w, img_header.h);
            continue;
        }
        lv_img_set_src(img, img_str);
        lv_obj_center(img);
        /* 中心点在显示内,进行比例映射 */
        if (type == app_lv_ui_local_list_left) {
            lv_coord_t obj_w_half = (LV_HOR_RES - img_max_header.w) / 2 / 2;
            lv_coord_t pic_x_ofs = app_sys_map(dis, (int32_t)distance, 0, 0, obj_w_half);
            APP_SYS_LOG_DEBUG("idx:%d, obj_w_half:%d pic_x_ofs:%d", idx, obj_w_half, pic_x_ofs);
            lv_obj_align(img, LV_ALIGN_RIGHT_MID, -pic_x_ofs, 0);
        }
        if (type == app_lv_ui_local_list_right) {
            lv_coord_t obj_w_half = (LV_HOR_RES - img_max_header.w) / 2 / 2;
            lv_coord_t pic_x_ofs = app_sys_map(dis, (int32_t)distance, 0, 0, obj_w_half);
            APP_SYS_LOG_DEBUG("idx:%d, obj_w_half:%d pic_x_ofs:%d", idx, obj_w_half, pic_x_ofs);
            lv_obj_align(img, LV_ALIGN_LEFT_MID,  +pic_x_ofs, 0);
        }
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_list_scroll_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_SCROLL_BEGIN:
    case LV_EVENT_SCROLL_END:
    case LV_EVENT_SCROLL:
    case LV_EVENT_PRESSING: {
        app_lv_ui_local_anim_handler(NULL, 0);
        break;
    }
    case LV_EVENT_PRESSED: {
        break;
    }
    case LV_EVENT_RELEASED: {
        #if 0
        /* 从图片组随便取一张 */
        lv_img_header_t img_max_header = {0};
        const char *img_max_str = app_lv_pic_str_find(app_lv_ui_res_local->list[0].idx_pic + APP_LV_UI_WATERFALL_OFS_MAX);
        if (lv_img_decoder_get_info(img_max_str, &img_max_header) != LV_RES_OK) {
            APP_SYS_LOG_WARN("can't take pic src");
            APP_SYS_LOG_WARN("%s", img_max_str);
            break;
        }
        /* 获得图片的宽和高(应该是一样的长度) */
        if (img_max_header.w != img_max_header.h) {
            APP_SYS_LOG_WARN("pic is not quadrate");
            APP_SYS_LOG_WARN("%s<w:%d,h:%d>", img_max_str, img_max_header.w, img_max_header.h);
            break;
        }
        lv_coord_t roll_ofs = lv_obj_get_scroll_y(app_lv_ui_res_local->obj_list) - app_lv_ui_res_local->ofs_base;
        lv_coord_t unit_ofs = roll_ofs / img_max_header.h * img_max_header.h + img_max_header.h * (APP_LV_UI_WATERFALL_NUM + 1);
        lv_coord_t item_ofs = roll_ofs % img_max_header.h;
        item_ofs = item_ofs < img_max_header.h / 2 ? 0 : img_max_header.h;
        lv_obj_scroll_to_y(app_lv_ui_res_local->obj_list, unit_ofs + item_ofs + app_lv_ui_res_local->ofs_base, LV_ANIM_ON);
        #endif
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_list_btn_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t *scene = lv_event_get_user_data(e);
        app_lv_scene_add(scene, false);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面显示
 *@param scene 场景
 */
void app_lv_ui_waterfall_show(void *scene)
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
        /*  */
        /* 初始化列表 */
        app_lv_ui_res_local->obj_list = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->obj_list);
        // lv_obj_set_style_pad_hor(app_lv_ui_res_local->obj_list, app_lv_style_hor_pct(0), 0);
        lv_obj_set_style_pad_ver(app_lv_ui_res_local->obj_list, app_lv_style_ver_pct(2), 0);
        lv_obj_set_style_pad_row(app_lv_ui_res_local->obj_list, app_lv_style_ver_pct(0), 0);
        lv_obj_set_style_layout(app_lv_ui_res_local->obj_list, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(app_lv_ui_res_local->obj_list, LV_FLEX_FLOW_ROW_WRAP, 0);
        lv_obj_set_style_flex_main_place(app_lv_ui_res_local->obj_list, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_event_cb(app_lv_ui_res_local->obj_list, app_lv_ui_list_scroll_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_size(app_lv_ui_res_local->obj_list, LV_HOR_RES, LV_VER_RES);
        lv_obj_center(app_lv_ui_res_local->obj_list);
        /* 为列表批量追加按钮 */
        for (uint32_t idx = 0; idx < app_lv_ui_res_local->list_num; idx++) {
            /* 条目按钮 */
            lv_obj_t *btn = lv_obj_create(app_lv_ui_res_local->obj_list);
            app_lv_style_object(btn);
            lv_obj_center(btn);
            /* 条目按钮 */
            lv_obj_t *img = lv_img_create(btn);
            app_lv_style_object(img);
            lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(img, app_lv_ui_list_btn_cb, LV_EVENT_CLICKED, app_lv_ui_res_local->list[idx].scene);
            lv_img_set_src(img, app_lv_pic_str_find(app_lv_ui_res_local->list[idx].idx_pic + APP_LV_UI_WATERFALL_OFS_MAX));
            lv_obj_center(img);
        }
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 1, 1000);
    }
    app_lv_ui_res_local->refr_one = true;
    app_lv_ui_local_anim_handler(NULL, 0);
    app_lv_ui_local_anim_handler(NULL, 0);
}

/*@brief 界面隐藏
 *@param scene 场景
 */
void app_lv_ui_waterfall_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
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
