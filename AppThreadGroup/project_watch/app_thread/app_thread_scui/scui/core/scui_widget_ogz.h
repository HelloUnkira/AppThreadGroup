#ifndef SCUI_WIDGET_OGZ_H
#define SCUI_WIDGET_OGZ_H

/* 控件处理函数映射表 */
typedef struct {
    uint32_t (*paint)(scui_widget_t *widget);
    uint32_t (*create)(scui_widget_t *widget, scui_handle_t handle);
    uint32_t (*destroy)(scui_widget_t *widget, bool from_parent);
} scui_widget_ogz_t;





#endif
