#ifndef APP_LV_UI_TETRIS_PRESENTER_H
#define APP_LV_UI_TETRIS_PRESENTER_H

/* 矩阵宽高元素数量 */
#define APP_LV_UI_TETRIS_LINE       13
#define APP_LV_UI_TETRIS_ELEMENT    13
#define APP_LV_UI_TETRIS_SCALE       4

typedef struct {
    bool (*ready)(void);
    bool (*execute)(void);
    void (*clean)(void);
    bool (*move_step)(void);
    bool (*move_left)(void);
    bool (*move_right)(void);
    bool (*rotate_left)(void);
    bool (*rotate_right)(void);
    void (*get_matrix)(void **matrix);
    void (*get_graph)(void **graph, uint32_t *scale, uint32_t *x, uint32_t *y);
    void (*get_graph_next)(void **graph, uint32_t *scale);
    void (*get_erase_line)(uint32_t *erase_line);
    void (*set_status)(bool *status);
    void (*get_status)(bool *status);
} app_lv_ui_tetris_presenter_t;

extern app_lv_ui_tetris_presenter_t app_lv_ui_tetris_presenter;

#endif
