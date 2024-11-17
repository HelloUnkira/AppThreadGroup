/*实现目标:
 *    自定义事件响应
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件响应(custom)
 *@param event 事件包
 */
void scui_event_custom(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_prepare(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_widget_scroll_s:
    case scui_event_widget_scroll_e:
    case scui_event_widget_scroll_c:
        scui_ui_presenter.vibrate_shot();
        break;
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_finish(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    case scui_event_key_click: {
        
        scui_handle_t stack_top = {0};
        scui_window_stack_nest(&stack_top);
        if (stack_top > 1) {
            // 返回上一层
            scui_window_stack_del();
        } else {
            // 回到主界面
            scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
        }
        
        scui_widget_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
