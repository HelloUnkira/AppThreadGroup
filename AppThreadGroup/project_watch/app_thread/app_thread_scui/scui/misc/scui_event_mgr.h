#ifndef SCUI_EVENT_MGR_H
#define SCUI_EVENT_MGR_H

/*@brief 事件引擎注册先响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_before(scui_event_cb_t event_cb);

/*@brief 事件引擎注册后响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_after(scui_event_cb_t event_cb);

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

/*@brief 事件响应
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond(scui_event_t *event);

/*@brief 事件响应(sched)
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_sched(scui_event_t *event);

/*@brief 事件响应(ptr)
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_ptr(scui_event_t *event);

/*@brief 事件响应(enc)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_enc(scui_event_t *event);

/*@brief 事件响应(key)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_key(scui_event_t *event);

/*@brief 事件响应(custom)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_respond_custom(scui_event_t *event);

#endif
