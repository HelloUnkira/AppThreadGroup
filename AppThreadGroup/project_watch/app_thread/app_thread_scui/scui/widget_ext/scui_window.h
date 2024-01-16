#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    scui_widget_t widget;
    scui_handle_t sibling[4];   // 0:上;1:下;2:左;3:右;
    uint8_t level:6;            // 窗口所在层级(数字越大越高)
    uint8_t surface:1;          // 窗口是否有独立画布
    uint8_t resident:1;         // 窗口是否常驻
} scui_window_t;

typedef struct {
    scui_widget_maker_t widget;
    scui_event_cb_t     event_cb;   /* 通常响应show,hide,focus_get,focus_lost,indev */
    uint8_t level:7;
    uint8_t buffer:1;
} scui_window_maker_t;

#endif
