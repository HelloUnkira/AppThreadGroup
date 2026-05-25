#include "lvgl_lib.h"

static void lvgl_theme_apply_cb(struct _lv_theme_t *t, lv_obj_t *obj)
{
    bool obj_is_base = true;
    obj_is_base = obj_is_base
        && obj->class_p != &lv_btn_class;    // 按钮不适用
        
    // 运动手表类控件默认以下效果
    // 透明黑色背景, 无边框, 无外部线, 无阴影
    if (obj_is_base)
    {
        // 这里释放可以直接移除这些属性?
        lv_obj_set_style_radius(obj, 0, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_style_outline_width(obj, 0, 0);
        lv_obj_set_style_shadow_width(obj, 0, 0);
    }
    
    // 手表所有控件都不需要自带滚动条
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    
    // flag在此处无法继承, 会被基础构造器复写
    // lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    // lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    // lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    // 基础对象默认黑色矩形背景
    if (obj->class_p == &lv_obj_class)
    {
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    }
    
    // 标签对象可以配置常用字库,常用字号,常用字间距,常用颜色,常用对齐
    if (obj->class_p == &lv_label_class)
    {
        lv_obj_set_style_text_font(obj, &lv_font_montserrat_26, 0);
        lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_color(obj, lv_color_white(), 0);
    }
}

void lvgl_theme_ready(void)
{
    static lv_theme_t theme_new = {0};
    lv_theme_set_apply_cb(&theme_new, lvgl_theme_apply_cb);
    
    lv_theme_t *theme = lv_disp_get_theme(NULL);
    lv_theme_set_parent(&theme_new, theme);
    
    lv_disp_set_theme(NULL, &theme_new);
    
    /* 设置四周边界(可选:调试用) */
    lv_obj_set_style_radius(lv_layer_top(), 45, 0);
    lv_obj_set_style_border_width(lv_layer_top(), 1, 0);
    lv_obj_set_style_border_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY),
        0);
}
