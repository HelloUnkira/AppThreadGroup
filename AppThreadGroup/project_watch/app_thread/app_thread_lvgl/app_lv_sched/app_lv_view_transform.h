#ifndef APP_LV_VIEW_TRANSFORM_H
#define APP_LV_VIEW_TRANSFORM_H

typedef enum {
    app_lv_view_transform_type_none = 0,        /* 无动画 */
    app_lv_view_transform_type_auto,            /* 自适应配置 */
    app_lv_view_transform_type_single_s,
    
    app_lv_view_transform_type_move,            /* 常规平铺(万金油) */
    app_lv_view_transform_type_cover_in,        /* 覆盖进入(推荐:浮动窗口) */
    app_lv_view_transform_type_cover_out,       /* 覆盖退出(推荐:浮动窗口) */
    app_lv_view_transform_type_zoom1,           /* 单边缩放(推荐:跟手界面切换) */
    app_lv_view_transform_type_zoom2,           /* 双边缩放(推荐:跟手界面切换) */
    app_lv_view_transform_type_center_in,       /* 透明缩小(推荐:界面跳转) */
    app_lv_view_transform_type_center_out,      /* 透明放大(推荐:界面跳转) */
    app_lv_view_transform_type_center_inout,    /* 透明放大(推荐:界面跳转) */
    app_lv_view_transform_type_rotate,          /* 待定开发 */
    
    app_lv_view_transform_type_single_e,
    app_lv_view_transform_type_num,
} app_lv_view_transform_type_t;

void app_lv_view_transform_reset(void);
void app_lv_view_transform_async_cb(void *user_data);

#endif
