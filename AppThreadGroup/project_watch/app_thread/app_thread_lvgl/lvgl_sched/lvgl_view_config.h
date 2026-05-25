#ifndef LVGL_VIEW_CONFIG_H
#define LVGL_VIEW_CONFIG_H

typedef enum
{
    lvgl_view_id_unknown = 0,
    /* 在这个下面无需顺序的任意插入一个枚举量以标记你的唯一界面 */
    /* 为了保证追溯, 这里建议手动指定编号, 只要不保证空隙即可 */
    
    lvgl_view_id_standby,       // 息屏
    lvgl_view_id_home_watch,    // 主表盘
    lvgl_view_id_home_panel_t,  // 下拉面板
    lvgl_view_id_home_panel_b,  // 上拉面板
    lvgl_view_id_nega,          // 负一屏
    lvgl_view_id_card1,         // 卡片1
    lvgl_view_id_card2,         // 卡片2
    
    lvgl_view_id_num,
} lvgl_view_id_t;

#endif
