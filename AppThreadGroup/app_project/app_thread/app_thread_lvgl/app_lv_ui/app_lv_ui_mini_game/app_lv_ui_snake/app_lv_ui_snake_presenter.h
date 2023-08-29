#ifndef APP_LV_UI_SNAKE_PRESENTER_H
#define APP_LV_UI_SNAKE_PRESENTER_H

/* 矩阵宽高元素数量 */
#define APP_LV_UI_SNAKE_LINE        15
#define APP_LV_UI_SNAKE_ELEMENT     15

typedef struct {
    bool (*ready)(void);
    bool (*execute)(lv_dir_t direct);
    void (*clean)(void);
    void (*get_matrix)(void **matrix);
    bool (*get_node)(uint32_t count, app_lv_ui_snake_node_t *node);
    void (*get_food)(app_lv_ui_snake_node_t *node);
} app_lv_ui_snake_presenter_t;

extern app_lv_ui_snake_presenter_t app_lv_ui_snake_presenter;

#endif
