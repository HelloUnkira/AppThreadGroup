#ifndef APP_LV_UI_2048_PRESENTER_H
#define APP_LV_UI_2048_PRESENTER_H

/* 矩阵宽高元素数量 */
#define APP_LV_UI_2048_NUM      4

typedef struct {
    void (*ready)(uint8_t difficult);
    void (*execute)(lv_dir_t direct);
    void (*get_matrix)(void **matrix);
} app_lv_ui_2048_presenter_t;

extern app_lv_ui_2048_presenter_t app_lv_ui_2048_presenter;

#endif