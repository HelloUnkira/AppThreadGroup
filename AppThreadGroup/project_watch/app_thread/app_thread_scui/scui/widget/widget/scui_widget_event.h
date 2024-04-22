#ifndef SCUI_WIDGET_EVENT_H
#define SCUI_WIDGET_EVENT_H

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param clip   剪切域
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, scui_area_t *clip, bool sync);

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync);

/*@brief 控件显示
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_show(scui_handle_t handle, bool delay);

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_hide(scui_handle_t handle, bool delay);

#define scui_widget_event_mask_quit(event)      ((event))
#define scui_widget_event_mask_keep(event)      ((event)->style.result |= 0x01)
#define scui_widget_event_mask_over(event)      ((event)->style.result |= 0x02)

#define scui_widget_event_check_quit(event)     ((event)->style.result == 0x00)
#define scui_widget_event_check_keep(event)     (((event)->style.result & 0x01) != 0)
#define scui_widget_event_check_over(event)     (((event)->style.result & 0x02) != 0)

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 */
void scui_widget_event_clear(scui_handle_t handle);

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_find(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_proc(scui_event_t *event);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_dispatch(scui_event_t *event);

/*@brief 控件事件冒泡
 *       亦可用于控件迭代等其他动作
 *@param event    事件(可以是假事件)
 *@param event_cb 事件回调(可以是假事件回调)
 *@param first    优先冒泡自己
 */
void scui_widget_event_bubble(scui_event_t *event, scui_event_cb_t event_cb, bool first);

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_widget_event_draw(scui_event_t *event);

#endif
