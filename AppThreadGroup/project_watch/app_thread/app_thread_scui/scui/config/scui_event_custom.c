/*实现目标:
 *    自定义事件响应
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件响应(before)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_before(scui_event_t *event)
{
    switch (event->type) {
    default:
        return scui_event_retval_default;
    }
}

/*@brief 事件响应(after)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_after(scui_event_t *event)
{
    switch (event->type) {
    default:
        return scui_event_retval_default;
    }
}

/*@brief 事件响应(custom)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_custom(scui_event_t *event)
{
    switch (event->type) {
    default:
        return scui_event_retval_default;
    }
}
