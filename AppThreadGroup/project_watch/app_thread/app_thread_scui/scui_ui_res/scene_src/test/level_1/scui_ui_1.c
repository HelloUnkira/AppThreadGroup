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
void scui_ui_scene_1_menial_btn_event_proc(scui_event_t *event)
{
    // 转移至控件调度
    if (!scui_event_type_widget(event->type)) {
         scui_widget_event_shift(event);
         return;
    }
    
    SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_page_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_menial_maker_t menial_maker = {0};
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        menial_maker.widget.type = scui_widget_type_menial;
        menial_maker.widget.parent = event->object;
        
        #if 1
        // menial_btn:
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        menial_maker.widget.event_cb = scui_ui_scene_1_menial_btn_event_proc;
        menial_maker.type = scui_menial_type_btn;
        menial_maker.data.btn.color[0].color_l.full = 0xFF00FF00;
        menial_maker.data.btn.color[0].color_d.full = 0xFF008000;
        menial_maker.data.btn.color[1].color_l.full = 0xFFFF0000;
        menial_maker.data.btn.color[1].color_d.full = 0xFF800000;
        scui_coord_t btn_w = menial_maker.widget.clip.w;
        
        menial_maker.data.btn.fixed  = 1;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x = (SCUI_HOR_RES - btn_w) / 2;
        menial_maker.widget.clip.y = 30;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 0;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 1 / 4 - btn_w / 2;
        menial_maker.widget.clip.y   = 30 + 60 + 10;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = 5;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 2 / 4 - btn_w / 2;
        menial_maker.widget.clip.y   = 30 + 60 + 10;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 2;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 3 / 4 - btn_w / 2;
        menial_maker.widget.clip.y   = 30 + 60 + 10;
        scui_widget_create(&menial_maker, &menial_handle);
        #endif
        
        
        
        
        
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_page_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_page_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_scroll_event(scui_event_t *event)
{
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        scui_widget_image_set(event->object, scui_image_prj_image_src_08_stopwatch_01_bgbmp);
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
