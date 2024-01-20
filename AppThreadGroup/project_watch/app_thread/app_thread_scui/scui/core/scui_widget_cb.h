#ifndef SCUI_WIDGET_CB_H
#define SCUI_WIDGET_CB_H

/* 控件事件回调 */
typedef scui_event_cb_t scui_widget_cb_event_t;

/* 控件创建回调 */
typedef void (*scui_widget_cb_create_t)(void *maker, scui_handle_t *handle, bool layout);

/* 控件销毁回调 */
typedef void (*scui_widget_cb_destroy_t)(scui_handle_t handle, bool parent_way);

/* 控件处理函数映射表 */
typedef struct {
    scui_widget_cb_event_t   event;
    scui_widget_cb_create_t  create;
    scui_widget_cb_destroy_t destroy;
} scui_widget_cb_t;

/*@brief 控件函数映射表
 *@param type 控件类型
 *@retval 控件函数映射表
 */
scui_widget_cb_t * scui_widget_cb_link(uint32_t type);

#endif
