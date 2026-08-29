/*实现目标:
 *    测试(widget main)
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

/*@brief 控件事件响应回调(条目2: Monkey Test)
 *@param event 事件
 */
void scui_test_ui_main_monkey_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_string_update_str(event->object, "Monkey Test");
        break;
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        static bool work = false; work = !work;
        scui_monkey_test(work);
        
        scui_widget_color_set(event->object, SCUI_COLOR_MAKE32(false, 0x0,
            work ? 0xFFFF0000 : 0xFF404040));
        scui_widget_draw(event->object, NULL, false, 0);
        
        SCUI_LOG_WARN("test ui main Monkey Test click To Home");
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_start, NULL);
        scui_event_notify(&event_ui);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调(条目3: Monitor Use)
 *@param event 事件
 */
void scui_test_ui_main_monitor_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_string_update_str(event->object, "Monitor Use");
        break;
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        static bool use = false; use = !use;
        if (use) scui_monitor_show(false);
        else scui_monitor_hide();
        
        scui_widget_color_set(event->object, SCUI_COLOR_MAKE32(false, 0x0,
            use ? 0xFFFF0000 : 0xFF404040));
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调(条目4: 进入测试列表)
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
