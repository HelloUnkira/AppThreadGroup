#ifndef APP_LV_UI_MAZE_H
#define APP_LV_UI_MAZE_H

typedef struct {
    uint8_t visit:1;
    uint8_t h_status:1;
    uint8_t v_status:1;
    uint8_t record:5;
} app_lv_ui_maze_block_t;

/* 矩阵宽高元素数量 */
#define APP_LV_UI_MAZE_LINE         15
#define APP_LV_UI_MAZE_ELEMENT      15

typedef struct {
    bool (*ready)(void);
    bool (*execute)(lv_dir_t direct);
    void (*get_matrix)(void **matrix);
    void (*clean)(void);
} app_lv_ui_maze_presenter_t;

extern app_lv_ui_maze_presenter_t app_lv_ui_maze_presenter;

#endif
