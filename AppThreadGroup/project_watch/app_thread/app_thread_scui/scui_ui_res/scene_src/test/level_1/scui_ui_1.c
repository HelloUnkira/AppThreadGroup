/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
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
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        SCUI_LOG_WARN("scui_event_key_click");
        static scui_window_switch_type_t switch_type = SCUI_WINDOW_SWITCH_TYPE;
        switch_type++;
        
        if (switch_type < scui_window_switch_single_s + 1)
            switch_type = scui_window_switch_single_e - 1;
        if (switch_type > scui_window_switch_single_e - 1)
            switch_type = scui_window_switch_single_s + 1;
        
        SCUI_ASSERT(switch_type > scui_window_switch_single_s);
        SCUI_ASSERT(switch_type < scui_window_switch_single_e);
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        *cfg_type = switch_type;
        
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_vedio_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {
        break;
    }
    case scui_event_create: {
        
        scui_xvedio_maker_t xvedio_maker = {0};
        scui_handle_t xvedio_handle = SCUI_HANDLE_INVALID;
        xvedio_maker.widget.type    = scui_widget_type_xvedio;
        xvedio_maker.widget.parent  = event->object;
        
        xvedio_maker.widget.clip.x   = SCUI_VER_RES / 6;
        xvedio_maker.widget.clip.y   = SCUI_VER_RES / 5 - 80 / 2;
        xvedio_maker.widget.clip.w   = 80;
        xvedio_maker.widget.clip.h   = 80;
        xvedio_maker.iframe.type     = scui_image_type_gif;
        xvedio_maker.iframe.handle   = scui_image_prj_image_src_vedio_bulbgif;
        xvedio_maker.iframe.gif.loop = 100;
        scui_widget_create(&xvedio_maker, &xvedio_handle);
        
        xvedio_maker.widget.clip.x   = SCUI_VER_RES / 6 + 120;
        xvedio_maker.widget.clip.y   = SCUI_VER_RES / 5 - 100 / 2;
        xvedio_maker.widget.clip.w   = 100;
        xvedio_maker.widget.clip.h   = 100;
        xvedio_maker.iframe.type     = scui_image_type_lottie;
        xvedio_maker.iframe.handle   = scui_image_prj_image_src_vedio_comfirmlottiejson;
        scui_widget_create(&xvedio_maker, &xvedio_handle);
        
        xvedio_maker.widget.clip.x   = SCUI_VER_RES / 6 + 120 + 120;
        xvedio_maker.widget.clip.y   = SCUI_VER_RES / 5 - 100 / 2;
        xvedio_maker.widget.clip.w   = 100;
        xvedio_maker.widget.clip.h   = 100;
        xvedio_maker.iframe.type     = scui_image_type_lottie;
        xvedio_maker.iframe.handle   = scui_image_prj_image_src_vedio_musiclottiejson;
        scui_widget_create(&xvedio_maker, &xvedio_handle);
        
        break;
    }
    case scui_event_destroy: {
        break;
    }
    case scui_event_draw: {
        
        break;
    }
    default:
        break;
    }
}
