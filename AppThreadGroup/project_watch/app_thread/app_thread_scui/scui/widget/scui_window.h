#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    scui_widget_t widget;
    scui_handle_t sibling[4];   /* 0:上;1:下;2:左;3:右; */
    uint8_t level:6;            /* 窗口所在层级(数字越大越高) */
    uint8_t resident:1;         /* 窗口是否常驻 */
} scui_window_t;

typedef struct {
    scui_widget_maker_t widget;
    scui_event_cb_t     event_cb;   /* 事件响应回调: */
    uint8_t level:6;                /* 窗口所在层级(数字越大越高) */
    uint8_t resident:1;             /* 窗口是否常驻 */
    uint8_t buffer:1;               /* 窗口是否使用独立画布 */
} scui_window_maker_t;

/*@brief 窗口控件创建
 *@param maker  窗口控件创建参数
 *@param handle 窗口控件句柄
 *@param layout 通过布局创建
 */
void scui_window_create(scui_window_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 窗口控件销毁
 *@param handle 窗口控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_window_destroy(scui_handle_t handle, bool from_parent);

#endif