#ifndef SCUI_BUTTON_H
#define SCUI_BUTTON_H

typedef enum {
    scui_button_mode_static = 0,    // 静态按钮
    scui_button_mode_scale,         // 缩放按钮
} scui_button_mode_t;

typedef struct {
    scui_widget_t       widget;
    scui_event_cb_t     notify_cb;  /* 专属事件通知 */
    scui_button_mode_t  mode;       /* 按钮模式 */
    scui_handle_t       image[4];   /* 图像句柄:四角(左上,右上,左下,右下) */
    scui_color_t        color;      /* 图像源色调 */
    scui_coord_t        delta;      /* 边界填充线(0:忽略;-1:所有;其他:边界) */
    /* 内部域(模式): */
    union {
        /* 缩放按钮 */
        struct {
            scui_coord_t btn1_lim;
            scui_coord_t btn1_way;
            scui_coord_t btn1_pct;
            uint8_t      btn1_hold;
        };
    };
    uint8_t btn_click:1;
} scui_button_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
    scui_event_cb_t     notify_cb;  /* 专属事件通知 */
    scui_button_mode_t  mode;       /* 按钮模式 */
    scui_handle_t       image[4];   /* 图像句柄:四角(左上,右上,左下,右下) */
    scui_color_t        color;      /* 图像源色调 */
    scui_coord_t        delta;      /* 边界填充线(0:忽略;-1:所有;其他:边界) */
} scui_button_maker_t;
#pragma pack(pop)

/*@brief 按钮控件创建
 *@param maker  按钮控件创建参数
 *@param handle 按钮控件句柄
 *@param layout 通过布局创建
 */
void scui_button_create(scui_button_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 按钮控件销毁
 *@param handle 按钮控件句柄
 */
void scui_button_destroy(scui_handle_t handle);

/*@brief 按钮控件事件处理回调
 *@param event 事件
 */
void scui_button_event(scui_event_t *event);

#endif
