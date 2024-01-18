#ifndef SCUI_EVENT_CUSTOM_H
#define SCUI_EVENT_CUSTOM_H

/* 事件表: */
typedef enum {
    scui_event_custom_s = scui_event_sys_num,
    
    /* 自定义事件<s> */
    
    /* 自定义事件<e> */
    
    scui_event_custom_e,
    
} scui_event_custom_t;

/*@brief 事件响应(before)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_before(scui_event_t *event);

/*@brief 事件响应(after)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_after(scui_event_t *event);

/*@brief 事件响应(custom)
*@param event 事件包
*@retval 事件响应回调返回值
*/
uint32_t scui_event_custom(scui_event_t *event);

#endif
