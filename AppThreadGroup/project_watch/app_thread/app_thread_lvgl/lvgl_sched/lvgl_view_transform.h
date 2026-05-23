#ifndef LVGL_VIEW_TRANSFORM_H
#define LVGL_VIEW_TRANSFORM_H

typedef enum {
    lvgl_view_tr_none = 0,        /* 无动画 */
    lvgl_view_tr_auto,            /* 自适应配置 */
    lvgl_view_tr_single_s,
    
    lvgl_view_tr_move,            /* 常规平铺(万金油) */
    lvgl_view_tr_cover_in,        /* 覆盖进入(推荐:浮动窗口) */
    lvgl_view_tr_cover_out,       /* 覆盖退出(推荐:浮动窗口) */
    lvgl_view_tr_zoom1,           /* 单边缩放(推荐:跟手界面切换) */
    lvgl_view_tr_zoom2,           /* 双边缩放(推荐:跟手界面切换) */
    lvgl_view_tr_center_in,       /* 透明缩小(推荐:界面跳转) */
    lvgl_view_tr_center_out,      /* 透明放大(推荐:界面跳转) */
    lvgl_view_tr_center_inout,    /* 透明放大(推荐:界面跳转) */
    lvgl_view_tr_rotate,          /* 待定开发 */
    
    lvgl_view_tr_single_e,
    lvgl_view_tr_num,
} lvgl_view_tr_t;

void lvgl_view_transform_reset(void);
void lvgl_view_transform_async_cb(void *user_data);

#endif
