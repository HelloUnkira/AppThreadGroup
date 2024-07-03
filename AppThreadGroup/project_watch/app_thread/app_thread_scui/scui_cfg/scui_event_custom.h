#ifndef SCUI_EVENT_CUSTOM_H
#define SCUI_EVENT_CUSTOM_H

/* 事件表: */
typedef enum {
    scui_event_custom_s = scui_event_sys_num,
    scui_event_custom_all,
    
    /* 自定义事件<s> */
    
    /* 自定义事件<e> */
    
    scui_event_custom_e,
    
} scui_event_custom_t;

/*@brief 事件响应(custom)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
void scui_event_custom(scui_event_t *event);

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_prepare(scui_event_t *event);

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_finish(scui_event_t *event);

#endif
