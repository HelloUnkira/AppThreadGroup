
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_obj_t  *scene;
    lv_font_t *font36;
    uint32_t   list_num;
    app_lv_ui_theme_item_t *list;
} *app_lv_ui_res_local = NULL;

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_list_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_SCROLL: {
        lv_obj_t *list = lv_event_get_user_data(e);
        lv_area_t list_area;
        lv_obj_get_coords(list, &list_area);
        lv_coord_t list_w = lv_area_get_width(&list_area);
        lv_coord_t list_h = lv_area_get_height(&list_area);
        lv_coord_t list_x_c = list_area.x1 + list_w / 2;
        lv_coord_t list_y_c = list_area.y1 + list_h / 2;
        lv_coord_t list_r = LV_MIN(list_w, list_h); //  * 7 / 10;
        
        for (uint32_t idx = 0; idx < lv_obj_get_child_cnt(list); idx++) {
            lv_obj_t *item = lv_obj_get_child(list, idx);
            lv_area_t item_area;
            lv_obj_get_coords(item, &item_area);
            lv_coord_t item_w = lv_area_get_width(&item_area);
            lv_coord_t item_h = lv_area_get_height(&item_area);
            lv_coord_t item_x_c = item_area.x1 + item_w / 2;
            lv_coord_t item_y_c = item_area.y1 + item_h / 2;
            lv_coord_t diff_x = item_x_c - list_x_c;
            lv_coord_t diff_y = item_y_c - list_y_c;
            diff_x = LV_ABS(diff_x);
            diff_y = LV_ABS(diff_y);
            
            if(diff_y >= list_r) {
                item_x_c = list_r;
            } else {
                lv_sqrt_res_t res;
                lv_sqrt(list_r * list_r - diff_y * diff_y, &res, 0x8000);
                item_x_c = list_r - res.i;
            }
            
            lv_obj_set_style_translate_x(item, item_x_c, 0);
            lv_opa_t opa = lv_map(item_x_c, 0, list_r, LV_OPA_TRANSP, LV_OPA_COVER);
            // lv_obj_set_style_opa(item, LV_OPA_COVER - opa, 0);
            // app_lv_style_opa_update(item, LV_OPA_COVER - opa);   // 卡的要死
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

/*@brief     界面显示
 *@param[in] scene 场景
 */
void app_lv_ui_list_arc_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 获取字体 */
        app_lv_ui_res_local->font36 = app_lv_multi_font_load(app_lv_multi_font_size_36);
        app_lv_ui_theme_list_alloc(&app_lv_ui_res_local->list, &app_lv_ui_res_local->list_num);
        /* 初始化列表 */
        lv_obj_t *list = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_add_event_cb(list, app_lv_ui_list_cb, LV_EVENT_ALL, list);
        lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scroll_dir(list, LV_DIR_VER);
        lv_obj_set_scroll_snap_y(list, LV_SCROLL_SNAP_CENTER);
        lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_clip_corner(list, true, 0);
        lv_obj_set_style_pad_ver(list, app_lv_style_ver_pct(2), 0);
        lv_obj_set_style_pad_hor(list, app_lv_style_hor_pct(5), 0);
        lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
        lv_obj_center(list);
        /* 为列表批量追加按钮 */
        for (uint32_t idx = 0; idx < app_lv_ui_res_local->list_num + 1; idx++) {
            if (idx == 0) {
                /* 条目按钮 */
                lv_obj_t *btn = lv_btn_create(list);
                app_lv_style_object(btn);
                lv_obj_set_width(btn, app_lv_style_hor_pct(90));
                lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);
                /* 条目文本 */
                lv_obj_t *lab = lv_label_create(btn);
                app_lv_style_object(lab);
                lv_obj_set_style_text_font(lab, app_lv_ui_res_local->font36, 0);
                lv_obj_set_width(lab, app_lv_style_hor_pct(60));
                lv_label_set_long_mode(lab, LV_LABEL_LONG_SCROLL_CIRCULAR);
                lv_label_set_text(lab, app_lv_lang_str_find(APP_LV_LANG_0X01b1));
                lv_obj_center(lab);
            } else {
                /* 条目按钮 */
                lv_obj_t *btn = lv_btn_create(list);
                app_lv_style_object(btn);
                lv_obj_add_event_cb(btn, app_lv_ui_list_btn_cb, LV_EVENT_CLICKED, app_lv_ui_res_local->list[idx - 1].scene);
                /* 条目按钮添加图片 */
                lv_obj_t *img = lv_img_create(btn);
                app_lv_style_object(img);
                const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[idx - 1].idx_pic + 5);
                lv_img_set_src(img, img_str != NULL ? img_str : NULL);
                lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);
                /* 条目按钮添加文本 */
                lv_obj_t *lab = lv_label_create(btn);
                app_lv_style_object(lab);
                lv_obj_set_style_text_font(lab, app_lv_ui_res_local->font36, 0);
                lv_obj_set_style_text_align(lab, LV_TEXT_ALIGN_LEFT, 0);
                lv_obj_set_width(lab, app_lv_style_hor_pct(65));
                lv_label_set_long_mode(lab, LV_LABEL_LONG_SCROLL_CIRCULAR);
                const char *lab_str = app_lv_lang_str_find(app_lv_ui_res_local->list[idx - 1].idx_str);
                lv_label_set_text(lab, lab_str != NULL ? lab_str : "Internal Test");
                lv_obj_align_to(lab, img, LV_ALIGN_OUT_RIGHT_MID, app_lv_style_hor_pct(1), 0);
            }
        }
        /* 初次刷新,居中第一个按钮 */
        lv_event_send(list, LV_EVENT_SCROLL, NULL);
        lv_obj_scroll_to_view(lv_obj_get_child(list, 0), LV_ANIM_OFF);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
void app_lv_ui_list_arc_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        app_lv_ui_theme_list_free(app_lv_ui_res_local->list);
        app_lv_multi_font_free(app_lv_multi_font_size_36);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}
