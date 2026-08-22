/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_sleep_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
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

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_sleep_body_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        break;
    }
    case scui_event_draw_graph: {
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x = clip.y = 0;
        
        /* 睡眠图标 */
        scui_handle_t image_sleep = scui_image_prj_sleep_recordpng;
        scui_area_t clip_sleep = {
            .x = (SCUI_HOR_RES - scui_image_w(image_sleep)) / 2,
            .y = 90,
            .w = scui_image_w(image_sleep),
            .h = scui_image_h(image_sleep),
        };
        scui_widget_draw_image(event->object, &clip_sleep, image_sleep, NULL, SCUI_COLOR_UNUSED);
        
        /* 标题(静态) */
        scui_string_update_text(SCUI_UI_SCENE_SLEEP_TITLE, SCUI_LANG_0X004f);
        
        if (scui_presenter.sleep_none()) {
            /* 无睡眠记录 */
            scui_string_update_str(SCUI_UI_SCENE_SLEEP_HOUR, "--");
            scui_string_update_text(SCUI_UI_SCENE_SLEEP_UNIT, SCUI_LANG_0X0050);
        } else {
            /* 睡眠时长 */
            char digit_str[10] = {0};
            snprintf(digit_str, sizeof(digit_str), "%d", scui_presenter.sleep_hour());
            scui_string_update_str(SCUI_UI_SCENE_SLEEP_HOUR, digit_str);
            scui_string_update_text(SCUI_UI_SCENE_SLEEP_UNIT, SCUI_LANG_0X0126);
        }
        
        break;
    }
    default:
        break;
    }
}
