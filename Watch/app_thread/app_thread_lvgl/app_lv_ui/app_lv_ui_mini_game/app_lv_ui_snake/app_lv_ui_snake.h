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

#endif
