#ifndef SCUI_WIDGET_EVENT_H
#define SCUI_WIDGET_EVENT_H

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, bool sync);

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync);

/*@brief 控件显示
 *@param handle 控件句柄
 */
void scui_widget_show(scui_handle_t handle);

/*@brief 控件隐藏
 *@param handle 控件句柄
 */
void scui_widget_hide(scui_handle_t handle);

/*@brief 控件显示(时序异步)
 *@param handle 控件句柄
 */
void scui_widget_show_delay(scui_handle_t handle);

/*@brief 控件隐藏(时序异步)
 *@param handle 控件句柄
 */
void scui_widget_hide_delay(scui_handle_t handle);

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_hide_without(scui_handle_t handle, scui_handle_t child);

/*@brief 控件树镜像(相对父控件)
 *@param handle 控件句柄
 */
void scui_widget_mirror(scui_handle_t handle);

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
scui_event_retval_t scui_widget_event_dispatch(scui_event_t *event);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_widget_event_draw(scui_event_t *event);

#endif
