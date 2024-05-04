#ifndef SCUI_EVENT_MGR_H
#define SCUI_EVENT_MGR_H

/* 检查事件调度耗时:以ms计 */
#define SCUI_EVENT_MGR_TICK_CHECK       0
#define SCUI_EVENT_MGR_TICK_FILTER      100

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

/*@brief 事件类型转为字符串
 *@param 事件
 *@retval 字符串
 */
const char * scui_event_to_str(scui_event_type_t type);

/*@brief 事件回调全局响应权限检查
 *       before和after的响应权限检查
 *@param event 事件包
 *@retval 允许该事件
 */
bool scui_event_cb_check(scui_event_t *event);

/*@brief 事件通报
 *@param event 事件包
 */
void scui_event_notify(scui_event_t *event);

/*@brief 事件派发
 *@param event 事件包
 */
void scui_event_dispatch(void);

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_respond(scui_event_t *event);

#endif
