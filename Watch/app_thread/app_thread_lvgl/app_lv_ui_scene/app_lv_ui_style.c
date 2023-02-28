/*实现目标:
 *    lvgl控件默认风格或者常规风格
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
#include "app_lv_ui_event.h"

/*@brief    默认控件风格
 *          黑色背景,无边框
 *param[in] obj 控件
 */
void app_lv_ui_style_object(lv_obj_t *obj)
{
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_border_side(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_border_color(obj, lv_color_black(), 0);
}

/*@brief 默认界面风格
 *       无主题风格,全屏幕,黑色背景全覆盖,无边界,无内部间隙
 *retval 返回场景(窗口,界面)
 */
lv_obj_t * app_lv_ui_style_scene(void)
{
    lv_obj_t *scene = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(scene);
    app_lv_ui_style_object(scene);
    lv_obj_set_size(scene, LV_HOR_RES, LV_VER_RES);
    return scene;
}

/*@brief     顶部居中标题标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_title_label(lv_obj_t *parent)
{
    #define app_lv_ui_style_title_label_font  &lv_font_montserrat_22
    
    lv_obj_t *label = lv_label_create(parent);
    app_lv_ui_style_object(label);
    lv_obj_set_size(label, LV_HOR_RES - 20, 30);
    lv_obj_set_style_pad_ver(label, 3, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, app_lv_ui_style_title_label_font, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(label, "Null");
    return label;
}

/*@brief     默认按钮
 *           蓝色背景,无边框
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_btn(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    app_lv_ui_style_object(btn);
    lv_obj_set_style_radius(btn, 45, 0);
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    return btn;
}

/*@brief     顶部风格
 *           左部按钮,右部时间,按钮返回上一层
 *param[in]  parent 父控件
 *param[out] btn   左上部按钮实例
 *param[out] time  右上部时钟实例
 *param[out] title 左上部文本实例
 *retval     顶部风格控件
 */
lv_obj_t * app_lv_ui_style_title(lv_obj_t *parent, lv_obj_t **btn, lv_obj_t **time, lv_obj_t **title)
{
    APP_MODULE_ASSERT(btn   != NULL);
    APP_MODULE_ASSERT(time  != NULL);
    APP_MODULE_ASSERT(title != NULL);
    /* 组合控件 */
    lv_obj_t *obj_box = lv_obj_create(parent);
    app_lv_ui_style_object(obj_box);
    lv_obj_set_size(obj_box, LV_HOR_RES, 40);
    lv_obj_set_style_pad_all(obj_box, 10, 0);
    lv_obj_align(obj_box, LV_ALIGN_TOP_MID, 0, 0);
    /* 左上角标题 */
    *btn = lv_btn_create(obj_box);
    app_lv_ui_style_object(*btn);
    lv_obj_set_size(*btn, LV_HOR_RES / 2 - 20, 20);
    lv_obj_set_style_shadow_width(*btn, 0, 0);
    lv_obj_align(*btn, LV_ALIGN_LEFT_MID, 10, 0);
    *title = lv_label_create(*btn);
    lv_obj_set_style_text_color(*title, lv_color_white(), 0);
    lv_obj_set_style_text_align(*title, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(*title, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(*title, "< Null");
    /* 右上角时间 */
    *time = lv_label_create(obj_box);
    lv_obj_set_size(*time, LV_HOR_RES / 2 - 20, 20);
    lv_obj_set_style_text_color(*time, lv_color_white(), 0);
    lv_obj_set_style_text_align(*time, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(*time, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_label_set_text(*time, "00:00");
    /* 组合控件 */
    return obj_box;
}

/*@brief     底部双按钮组
 *           左部按钮,右部按钮,无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 */
lv_obj_t * app_lv_ui_style_two_btns(lv_obj_t *parent, lv_obj_t **btn_l, lv_obj_t **lbl_l, lv_obj_t **btn_r, lv_obj_t **lbl_r)
{
    #define app_lv_ui_style_two_btns_btn_text_font  &lv_font_montserrat_22

    APP_MODULE_ASSERT(btn_l != NULL);
    APP_MODULE_ASSERT(lbl_l != NULL);
    APP_MODULE_ASSERT(btn_r != NULL);
    APP_MODULE_ASSERT(lbl_r != NULL);
    /* 下部按钮 */
    lv_obj_t *btn = lv_obj_create(parent);
    app_lv_ui_style_object(btn);
    lv_obj_set_size(btn, LV_HOR_RES, 60);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    /* 左下角按钮 */
    *btn_l = lv_btn_create(btn);
    app_lv_ui_style_object(*btn_l);
    lv_obj_set_size(*btn_l, LV_HOR_RES / 2 - 20, 40);
    lv_obj_set_style_radius(*btn_l, 45, 0);
    lv_obj_set_style_bg_color(*btn_l, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_shadow_width(*btn_l, 0, 0);
    lv_obj_align(*btn_l, LV_ALIGN_LEFT_MID, 5, 0);
    *lbl_l = lv_label_create(*btn_l);
    lv_obj_set_style_text_color(*lbl_l, lv_color_white(), 0);
    lv_obj_set_style_text_font(*lbl_l, app_lv_ui_style_two_btns_btn_text_font, 0);
    lv_obj_align(*lbl_l, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(*lbl_l, "L_Btn");
    /* 右下角按钮 */
    *btn_r = lv_btn_create(btn);
    app_lv_ui_style_object(*btn_r);
    lv_obj_set_size(*btn_r, LV_HOR_RES / 2 - 20, 40);
    lv_obj_set_style_radius(*btn_r, 45, 0);
    lv_obj_set_style_shadow_width(*btn_r, 0, 0);
    lv_obj_align(*btn_r, LV_ALIGN_RIGHT_MID, -5, 0);
    *lbl_r = lv_label_create(*btn_r);
    lv_obj_set_style_text_color(*lbl_r, lv_color_white(), 0);
    lv_obj_set_style_text_font(*lbl_r, app_lv_ui_style_two_btns_btn_text_font, 0);
    lv_obj_align(*lbl_r, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(*lbl_r, "R_Btn");
    /* 下部按钮 */
    return btn;
}

/*@brief     特效滚轮
 *           默认遮罩,被选值(白色,内部字体28),非选值(灰色,内部字体22),轮滚
 *param[in]  parent  父控件
 *param[in]  width   滚轮宽度
 *param[in]  options 滚轮显示值
 *param[in]  vis_row 滚轮默认显示个数
 */
lv_obj_t * app_lv_ui_style_roller(lv_obj_t *parent, lv_coord_t width, const char *options, uint8_t vis_row)
{
    #define app_lv_ui_style_roller_rolcus_font      &lv_font_montserrat_22
    #define app_lv_ui_style_roller_rolsel_font      &lv_font_montserrat_28
    
    lv_obj_t *roller = lv_roller_create(parent);
    app_lv_ui_style_object(roller);
    lv_obj_add_event_cb(roller, app_lv_ui_roller_mask_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_width(roller, width);
    lv_obj_set_style_bg_opa(roller, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(roller, LV_OPA_0, LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller, app_lv_ui_style_roller_rolcus_font, 0);
    lv_obj_set_style_text_font(roller, app_lv_ui_style_roller_rolsel_font, LV_PART_SELECTED);
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, 0);
    lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL);///* 滚动模式显示异常 */LV_ROLLER_MODE_INFINITE);//
    lv_roller_set_visible_row_count(roller, vis_row);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);
    return roller;
}
