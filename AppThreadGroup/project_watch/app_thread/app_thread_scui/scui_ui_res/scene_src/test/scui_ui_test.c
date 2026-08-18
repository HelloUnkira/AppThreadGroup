/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_test_event_proc(scui_event_t *event)
{
    static bool ptr_long_jump = false;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        
        /* 设置背景 */
        scui_widget_image_set(SCUI_UI_SCENE_TEST, scui_image_prj_watchface_bg_01_bgjpg);
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    
    case scui_event_ptr_down:
        ptr_long_jump = false;
        break;
    case scui_event_ptr_hold:
        if (event->ptr_tick > 3000) {
            if (!ptr_long_jump) SCUI_LOG_WARN("ptr long hold");
            ptr_long_jump = true;
        }
        break;
    case scui_event_ptr_up:
        if (ptr_long_jump)
            scui_ui_scene_return();
        break;
    default:
        break;
    }
}
