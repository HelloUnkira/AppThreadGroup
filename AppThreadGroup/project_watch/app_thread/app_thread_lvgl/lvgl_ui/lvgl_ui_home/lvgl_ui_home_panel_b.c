#include "lvgl_lib.h"

void lvgl_ui_home_panel_b_create(lv_obj_t *page)
{
    lv_color_t color = lv_color_make(lv_rand(0, 0xFF), lv_rand(0, 0xFF), lv_rand(0,
                0xFF));
    lv_obj_set_style_bg_color(page, color, 0);
    
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text_static(label, "lvgl_ui_home_panel_b");
    lv_obj_center(label);
}

void lvgl_ui_home_panel_b_destroy(lv_obj_t *page)
{
}

void lvgl_ui_home_panel_b_focus_get(lv_obj_t *page)
{
}

void lvgl_ui_home_panel_b_focus_lost(lv_obj_t *page)
{
}
