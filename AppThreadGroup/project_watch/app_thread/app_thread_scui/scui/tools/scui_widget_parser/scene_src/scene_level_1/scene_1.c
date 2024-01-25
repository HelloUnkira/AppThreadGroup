/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 *@retval 事件响应状态
 */
scui_event_retval_t scui_ui_scene_1_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    default:
        return scui_event_retval_quit;
    }
}
