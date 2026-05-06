/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
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
void scui_ui_scene_float_1_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.type = scui_widget_type_custom;
        custom_maker.widget.parent = event->object;
        custom_maker.widget.clip.x = 10 * 2;
        custom_maker.widget.clip.y = 10 * 2;
        custom_maker.widget.clip.w = widget_clip.w - 10 * 4;
        custom_maker.widget.clip.h = widget_clip.h - 10 * 4;
        custom_maker.type = scui_custom_type_qrcode;
        custom_maker.data.qrcode.color = SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF);
        custom_maker.data.qrcode.data  = "https://github.com/HelloUnkira/AppThreadGroup.git";
        custom_maker.data.qrcode.size  = strlen(custom_maker.data.qrcode.data);
        scui_widget_create(&custom_maker, &custom_handle);
        scui_custom_update_qrcode(custom_handle);
        
        #if 0
        /* 覆盖测试用的 */
        custom_maker.widget.clip.x = widget_clip.w / 4;
        custom_maker.widget.clip.y = widget_clip.h / 4;
        custom_maker.widget.clip.w = widget_clip.w / 2;
        custom_maker.widget.clip.h = widget_clip.h / 2;
        custom_maker.widget.style.fully_bg = true;
        custom_maker.type = scui_custom_type_none;
        scui_widget_create(&custom_maker, &custom_handle);
        #endif
        
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += 10; clip.w -= 10 * 2;
        clip.y += 10; clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_BLACK);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_1_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.type = scui_widget_type_custom;
        custom_maker.widget.parent = event->object;
        custom_maker.widget.clip.x = 15;
        custom_maker.widget.clip.y = 15;
        custom_maker.widget.clip.w = widget_clip.w - 15 * 2;
        custom_maker.widget.clip.h = widget_clip.h - 15 * 2;
        custom_maker.type = scui_custom_type_barcode;
        custom_maker.data.barcode.color = SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF);
        custom_maker.data.barcode.data  = "https://github.com/HelloUnkira/AppThreadGroup.git";
        custom_maker.data.barcode.size  = strlen(custom_maker.data.barcode.data);
        scui_widget_create(&custom_maker, &custom_handle);
        scui_custom_update_barcode(custom_handle);
        
        #if 0
        /* 覆盖测试用的 */
        custom_maker.widget.clip.x = widget_clip.w / 4;
        custom_maker.widget.clip.y = widget_clip.h / 4;
        custom_maker.widget.clip.w = widget_clip.w / 2;
        custom_maker.widget.clip.h = widget_clip.h / 2;
        custom_maker.widget.style.fully_bg = true;
        custom_maker.type = scui_custom_type_none;
        scui_widget_create(&custom_maker, &custom_handle);
        #endif
        
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += 10; clip.w -= 10 * 2;
        clip.y += 10; clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_BLACK);
        break;
    }
    default:
        break;
    }
}
