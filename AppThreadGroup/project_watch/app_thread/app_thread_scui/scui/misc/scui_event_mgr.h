#ifndef SCUI_EVENT_MGR_H
#define SCUI_EVENT_MGR_H

/* 事件类型转标号字符串回调(可选支持,可能浪费空间占用) */
typedef const char * (*scui_event_cb_type_stringify_t)(scui_event_type_t type);

/*@brief 事件类型转标记字符串
 *@param type 事件类型
 *@retval 标记字符串
 */
const char * scui_event_type_stringify(scui_event_type_t type);

/*@brief 事件引擎注册转标号字符串回调
 *@param type_stringify 回调
 */
void scui_event_register_type_stringify(scui_event_cb_type_stringify_t type_stringify);

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_prepare(scui_event_cb_t event_cb);

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_finish(scui_event_cb_t event_cb);

/*@brief 事件响应注册(custom)
 *@param event_cb 事件回调
 */
void scui_event_register_custom(scui_event_cb_t event_cb);

/*@brief 事件通报
 *@param event 事件包
 */
void scui_event_notify(scui_event_t *event);

/*@brief 事件派发
 *@param event 事件包
 */
void scui_event_dispatch(void);

#endif
