#ifndef SCUI_WIDGET_EVENT_H
#define SCUI_WIDGET_EVENT_H

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_find(scui_handle_t handle, scui_widget_event_t *event);

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_widget_event_t *event);

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_widget_event_t *event);

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 *@param event  自定义控件回调
 */
void scui_widget_event_clear(scui_handle_t handle);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_proc(scui_event_t *event);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_paint(scui_event_t *event);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_default(scui_event_t *event);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_dispatch(scui_event_t *event);

#endif
