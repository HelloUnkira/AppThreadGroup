/*实现目标:
 *    测试(widget list)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 列表条目事件回调(设文本+点击跳转测试界面)
 *@param event 事件
 */
void scui_test_ui_list_item_event_proc(scui_event_t *event)
{
    static const char * const item_text[16] = {
        "Test Sim Ptr",  "Test Sim Enc", "Test Sim Bar", "Test Sim Key", "Test Multiply Scroll",
        "Test Multiply String", "Test Menial Object", "Test Symbol Char", "Test Automatic Icon", "Test Draw Graph",
        "Test Code", "Test Ring", "Test Chart", "Test Roller", "Test Button", "Test Misc",
    };
    static const scui_handle_t item_scene[16] = {
        SCUI_UI_SCENE_TEST_UI_INDEV_PTR,
        SCUI_UI_SCENE_TEST_UI_INDEV_ENC,
        SCUI_UI_SCENE_TEST_UI_INDEV_BAR,
        SCUI_UI_SCENE_TEST_UI_INDEV_KEY,
        SCUI_UI_SCENE_TEST_UI_SCROLL,
        SCUI_UI_SCENE_TEST_UI_STRING,
        SCUI_UI_SCENE_TEST_UI_OBJECT,
        SCUI_UI_SCENE_TEST_UI_SYMBOL,
        SCUI_UI_SCENE_TEST_UI_ICON,
        SCUI_UI_SCENE_TEST_UI_GRAPH,
        SCUI_UI_SCENE_TEST_UI_CODE,
        SCUI_UI_SCENE_TEST_UI_RING,
        SCUI_UI_SCENE_TEST_UI_CHART,
        SCUI_UI_SCENE_TEST_UI_ROLLER,
        SCUI_UI_SCENE_TEST_UI_BUTTON,
        SCUI_UI_SCENE_TEST_UI_MISC,
    };
    
    switch (event->type) {
    case scui_event_create: {
        /* 文本只能通过函数调用设置 */
        scui_handle_t index = scui_widget_child_to_index(event->object);
        scui_string_update_str(event->object, item_text[index]);
        break;
    }
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        scui_handle_t index = scui_widget_child_to_index(event->object);
        if (index >= scui_arr_len(item_scene))
            break;
        if (item_scene[index] == SCUI_HANDLE_INVALID)
            break;
        scui_window_stack_cover_by(item_scene[index],
            scui_window_switch_none, scui_opt_dir_none);
        break;
    }
    default:
        break;
    }
}
