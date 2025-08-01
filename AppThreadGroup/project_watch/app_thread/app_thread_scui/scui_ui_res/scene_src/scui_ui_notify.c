/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    void *occupy;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_notify_none_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        /* 单字符文本 */
        scui_string_args_t args = {0};
        args.color.color_s.full = 0xFFFFFFFF;
        args.color.color_e.full = 0xFFFFFFFF;
        args.color.filter = true;
        args.align_hor = 2;
        args.align_ver = 2;
        
        args.name = SCUI_FONT_TYPE_32_ASCII;
        args.utf8 = "Wait-Adaptor";
        
        scui_custom_draw_text(event->object, NULL, &args, SCUI_HANDLE_INVALID);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_notify_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle             = SCUI_HANDLE_INVALID;
        custom_maker.widget.type                = scui_widget_type_custom;
        custom_maker.widget.clip.w              = SCUI_HOR_RES;
        custom_maker.widget.parent              = event->object;
        custom_maker.widget.clip.h              = 80;
        custom_maker.widget.event_cb            = scui_ui_scene_notify_none_event;
        scui_widget_create(&custom_maker, &custom_handle);
        
        scui_widget_align_pos(custom_handle, event->object, scui_opt_align_icc, NULL);
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        // 这里画个圈,校验测试使用
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += 4; clip.y += 4;
        scui_widget_draw_image(event->object, &clip,
        scui_image_prj_image_src_03_activity_ring_big_max_03_bj_01bmp,
        NULL, SCUI_COLOR_MAKE32(true, 0xFF000000, 0xFF404040));
        
        break;
    }
    default:
        break;
    }
}
