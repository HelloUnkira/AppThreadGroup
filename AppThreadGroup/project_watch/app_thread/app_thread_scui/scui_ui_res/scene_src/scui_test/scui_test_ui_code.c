/*实现目标:
 *    测试(widget code)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_code_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        scui_widget_draw(event->object, NULL, false, 0);
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

/*@brief code_1 控件事件响应回调(qrcode)
 *@param event 事件
 */
void scui_test_ui_code_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_ximage_maker_define(ximage_maker);
        scui_handle_t ximage_handle = SCUI_HANDLE_INVALID;
        
        ximage_maker.widget.parent = event->object;
        ximage_maker.widget.clip.x = 10 * 2;
        ximage_maker.widget.clip.y = 10 * 2;
        ximage_maker.widget.clip.w = widget_clip.w - 10 * 4;
        ximage_maker.widget.clip.h = widget_clip.h - 10 * 4;
        scui_widget_create(&ximage_maker, &ximage_handle);
        
        scui_ximage_qrcode(ximage_handle,
            (uint8_t *)"https://github.com/HelloUnkira/AppThreadGroup.git",
            strlen("https://github.com/HelloUnkira/AppThreadGroup.git"),
            SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF),
            SCUI_SCALE_COF);
        
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
    case scui_event_draw_graph: {
        
        scui_area_t clip = {
            .x = 10,
            .y = 10,
            .w = scui_widget_clip(event->object).w - 10 * 2,
            .h = scui_widget_clip(event->object).h - 10 * 2,
        };
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_BLACK);
        break;
    }
    default:
        break;
    }
}

/*@brief code_2 控件事件响应回调(barcode)
 *@param event 事件
 */
void scui_test_ui_code_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_ximage_maker_define(ximage_maker);
        scui_handle_t ximage_handle = SCUI_HANDLE_INVALID;
        
        ximage_maker.widget.parent = event->object;
        ximage_maker.widget.clip.x = 15;
        ximage_maker.widget.clip.y = 15;
        ximage_maker.widget.clip.w = widget_clip.w - 15 * 2;
        ximage_maker.widget.clip.h = widget_clip.h - 15 * 2;
        scui_widget_create(&ximage_maker, &ximage_handle);
        
        scui_ximage_barcode(ximage_handle,
            (uint8_t *)"https://github.com/HelloUnkira/AppThreadGroup.git",
            strlen("https://github.com/HelloUnkira/AppThreadGroup.git"),
            SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF),
            SCUI_SCALE_COF);
        
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
    case scui_event_draw_graph: {
        
        scui_area_t clip = {
            .x = 10,
            .y = 10,
            .w = scui_widget_clip(event->object).w - 10 * 2,
            .h = scui_widget_clip(event->object).h - 10 * 2,
        };
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_BLACK);
        break;
    }
    default:
        break;
    }
}
