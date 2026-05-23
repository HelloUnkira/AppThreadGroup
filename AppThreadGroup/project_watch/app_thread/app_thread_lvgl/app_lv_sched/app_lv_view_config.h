#ifndef APP_LV_VIEW_CONFIG_H
#define APP_LV_VIEW_CONFIG_H

typedef enum {
    app_lv_view_id_unknown = 0,
    /* 在这个下面无需顺序的任意插入一个枚举量以标记你的唯一界面 */
    /* 为了保证追溯, 这里建议手动指定编号, 只要不保证空隙即可 */
    
    app_lv_view_id_standby,      // 息屏界面
    
    app_lv_view_id_home_watch,   // 主表盘
    app_lv_view_id_home_t,       // 下拉面板
    app_lv_view_id_home_b,       // 上拉面板
    app_lv_view_id_nega,         // 负一屏
    
    app_lv_view_id_num,
} app_lv_view_id_t;

#endif
