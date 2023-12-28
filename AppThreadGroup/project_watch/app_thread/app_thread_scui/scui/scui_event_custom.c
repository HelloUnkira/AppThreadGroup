/*实现目标:
 *    自定义事件响应
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 事件响应(before)
 *@param event 事件包
 *@retval 事件响应回调返回值
 */
uint32_t scui_event_before(scui_event_t *event)
{
    switch (event->type) {
    default:
        return SCUI_EVENT_DEFAULT;
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
        return SCUI_EVENT_DEFAULT;
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
        return SCUI_EVENT_DEFAULT;
    }
}
