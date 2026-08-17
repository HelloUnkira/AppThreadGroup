/*实现目标:
 *    窗口:scui_test_ui_main
 *    测试入口主界面: 静态两个string条目, 水平居中垂直镜像对齐
 *    条目1: 进入主界面
 *    条目2: 进入测试列表
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调(条目1: 进入主界面)
 *@param event 事件
 */
void scui_test_ui_main_home_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_string_update_str(event->object, "Home");
        break;
    case scui_event_ptr_click:
        scui_event_mask_over(event);
        SCUI_LOG_WARN("test ui main click Home");
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_start, NULL);
        scui_event_notify(&event_ui);
        break;
    default:
        break;
    }
}

/*@brief 控件事件响应回调(条目2: 进入测试列表)
 *@param event 事件
 */
void scui_test_ui_main_test_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_string_update_str(event->object, "Test");
        break;
    case scui_event_ptr_click:
        scui_event_mask_over(event);
        SCUI_LOG_WARN("test ui main click Test List");
        scui_window_stack_cover_by(SCUI_UI_SCENE_TEST_UI_LIST,
            scui_window_switch_none, scui_opt_dir_none);
        break;
    default:
        break;
    }
}
