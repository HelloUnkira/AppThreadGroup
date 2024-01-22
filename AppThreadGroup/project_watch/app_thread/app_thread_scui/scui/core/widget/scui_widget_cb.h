#ifndef SCUI_WIDGET_CB_H
#define SCUI_WIDGET_CB_H

/* 控件事件回调 */
typedef scui_event_cb_t scui_widget_cb_event_t;

/* 控件创建回调 */
typedef void (*scui_widget_cb_create_t)(void *maker, scui_handle_t *handle, bool layout);

/* 控件销毁回调 */
typedef void (*scui_widget_cb_destroy_t)(scui_handle_t handle);

/* 控件处理函数映射表 */
typedef struct {
    scui_widget_cb_event_t   event;
    scui_widget_cb_create_t  create;
    scui_widget_cb_destroy_t destroy;
} scui_widget_cb_t;

/*@brief 查找控件映射表
 *@param type 控件类型
 *@retval 控件映射表
 */
scui_widget_cb_t * scui_widget_cb_find(uint32_t type);

/*@brief 通过映射表调用创建一个控件
 *@param handle 控件句柄
 */
void scui_widget_cb_create(scui_handle_t handle);

/*@brief 通过映射表调用销毁一个控件
 *@param handle 控件句柄
 */
void scui_widget_cb_destroy(scui_handle_t handle);

/*@brief 通过映射表调用加载一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 根控件句柄
 */
void scui_widget_cb_load(scui_handle_t handle);

/*@brief 通过映射表调用卸载一个控件树
 *       从根控件开始到它的所有子控件
 *@param handle 根控件句柄
 */
void scui_widget_cb_unload(scui_handle_t handle);

#endif
