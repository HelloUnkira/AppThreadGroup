#ifndef APP_LV_UI_SNAKE_H
#define APP_LV_UI_SNAKE_H

typedef struct {
    uint8_t status:1;   /* 地图该点是否为障碍物 */
    uint8_t record:7;   /* 外部信息,可以是颜色索引 */
} app_lv_ui_snake_block_t;

typedef struct {
    uint32_t pos_x;     /* 蛇身节点坐标 */
    uint32_t pos_y;     /* 蛇身节点坐标 */
    uint8_t  record;    /* 可以是颜色索引 */
} app_lv_ui_snake_node_t;

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
