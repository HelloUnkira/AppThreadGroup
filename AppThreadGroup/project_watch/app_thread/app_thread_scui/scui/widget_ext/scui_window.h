#ifndef SCUI_WINDOW_H
#define SCUI_WINDOW_H

typedef struct {
    scui_widget_t widget;
    uint8_t level:7;
    uint8_t buffer:1;
} scui_window_t;

typedef struct {
    scui_widget_layout_t widget;
    uint8_t level:7;
    uint8_t buffer:1;
} scui_window_layout_t;

#endif
