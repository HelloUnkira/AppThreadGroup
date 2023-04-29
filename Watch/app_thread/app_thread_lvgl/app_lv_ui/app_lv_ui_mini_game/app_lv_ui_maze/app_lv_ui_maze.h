#ifndef APP_LV_UI_MAZE_H
#define APP_LV_UI_MAZE_H

typedef struct {
    uint8_t visit:1;
    uint8_t h_status:1;
    uint8_t v_status:1;
    uint8_t record:5;
} app_lv_ui_maze_block_t;

#endif
