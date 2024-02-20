/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_window_mgr_t scui_window_mgr;

/*@brief 窗口切换事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_switch_event(scui_event_t *event)
{
    scui_event_retval_t ret = scui_event_retval_quit;
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    switch (event->type) {
    case scui_event_ptr_down:
        SCUI_LOG_INFO("event %u", event->type);
        if (scui_window_mgr.switch_args.lock)
            break;
    case scui_event_ptr_fling:
        SCUI_LOG_INFO("event %u", event->type);
        break;
    case scui_event_ptr_move:
        SCUI_LOG_INFO("event %u", event->type);
        break;
    case scui_event_ptr_up:
        SCUI_LOG_INFO("event %u", event->type);
        if (scui_window_mgr.switch_args.lock)
            break;
        break;
    default:
        break;
    }
    
    return ret;
}
