#ifndef SCUI_WIDGET_OGZ_H
#define SCUI_WIDGET_OGZ_H

/* 控件处理函数映射表 */
typedef struct {
    uint32_t (*event)(scui_widget_t *widget, scui_event_t *event);
    uint32_t (*create)(scui_widget_t *widget, scui_handle_t handle);
    uint32_t (*destroy)(scui_widget_t *widget, bool from_parent);
} scui_widget_ogz_t;

/*@brief 控件函数映射表
 *@param type 控件类型
 *@retval 控件函数映射表
 */
scui_widget_ogz_t * scui_widget_ogz_func(uint32_t type);




#endif
