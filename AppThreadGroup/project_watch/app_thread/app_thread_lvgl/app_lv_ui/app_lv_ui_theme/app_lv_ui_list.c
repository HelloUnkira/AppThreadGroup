
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
void app_lv_ui_list_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        app_lv_ui_theme_list_alloc(&app_lv_ui_res_local->list, &app_lv_ui_res_local->list_num);
        /* 获取字体 */
        app_lv_ui_res_local->font36 = app_lv_multi_font_load(app_lv_multi_font_size_36);
        /* 初始化列表 */
        lv_obj_t *list = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_set_style_pad_row(list, app_lv_style_ver_pct(2), 0);
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
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
void app_lv_ui_list_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        app_lv_multi_font_free(app_lv_multi_font_size_36);
        app_lv_ui_theme_list_free(app_lv_ui_res_local->list);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}
