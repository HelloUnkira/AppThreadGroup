#ifndef LVGL_UI_H
#define LVGL_UI_H

void lvgl_ui_standby_create(lv_obj_t *page);
void lvgl_ui_standby_destroy(lv_obj_t *page);
void lvgl_ui_standby_focus_get(lv_obj_t *page);
void lvgl_ui_standby_focus_lost(lv_obj_t *page);

// home:
void lvgl_ui_home_watch_create(lv_obj_t *page);
void lvgl_ui_home_watch_destroy(lv_obj_t *page);
void lvgl_ui_home_watch_focus_get(lv_obj_t *page);
void lvgl_ui_home_watch_focus_lost(lv_obj_t *page);

void lvgl_ui_home_panel_t_create(lv_obj_t *page);
void lvgl_ui_home_panel_t_destroy(lv_obj_t *page);
void lvgl_ui_home_panel_t_focus_get(lv_obj_t *page);
void lvgl_ui_home_panel_t_focus_lost(lv_obj_t *page);

void lvgl_ui_home_panel_b_create(lv_obj_t *page);
void lvgl_ui_home_panel_b_destroy(lv_obj_t *page);
void lvgl_ui_home_panel_b_focus_get(lv_obj_t *page);
void lvgl_ui_home_panel_b_focus_lost(lv_obj_t *page);

void lvgl_ui_nega_create(lv_obj_t *page);
void lvgl_ui_nega_destroy(lv_obj_t *page);
void lvgl_ui_nega_focus_get(lv_obj_t *page);
void lvgl_ui_nega_focus_lost(lv_obj_t *page);

void lvgl_ui_card1_create(lv_obj_t *page);
void lvgl_ui_card1_destroy(lv_obj_t *page);
void lvgl_ui_card1_focus_get(lv_obj_t *page);
void lvgl_ui_card1_focus_lost(lv_obj_t *page);

void lvgl_ui_card2_create(lv_obj_t *page);
void lvgl_ui_card2_destroy(lv_obj_t *page);
void lvgl_ui_card2_focus_get(lv_obj_t *page);
void lvgl_ui_card2_focus_lost(lv_obj_t *page);

#endif
