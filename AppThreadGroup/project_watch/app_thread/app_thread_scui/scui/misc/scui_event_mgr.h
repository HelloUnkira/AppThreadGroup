#ifndef SCUI_EVENT_MGR_H
#define SCUI_EVENT_MGR_H

/* 常用事件配置组合 */
#define scui_event_define(event_v, object_v, sync_v, type_v, absorb_v)  \
scui_event_t event_v = {.object = object_v, .style.sync = sync_v,.type = type_v, .absorb = absorb_v,};

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_access(scui_event_cb_t event_cb);

/*@brief 事件响应注册(custom)
 *@param event_cb 事件回调
 */
void scui_event_register_custom(scui_event_cb_t event_cb);

/*@brief 事件引擎注册响应回调
 *@param event_cb 事件回调
 */
void scui_event_register_finish(scui_event_cb_t event_cb);

/*@brief 事件通报
 *@param event 事件包
 */
void scui_event_notify(scui_event_t *event);

/*@brief 事件派发
 *@param event 事件包
 */
void scui_event_dispatch(void);

#endif
