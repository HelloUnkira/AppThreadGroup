/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_page_1_event_proc(scui_event_t *event)
{
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_page_2_event_proc(scui_event_t *event)
{
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_page_3_event_proc(scui_event_t *event)
{
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_event(scui_event_t *event)
{
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_widget_image_set(event->object, scui_image_prj_sw_01_bgjpg);
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}
