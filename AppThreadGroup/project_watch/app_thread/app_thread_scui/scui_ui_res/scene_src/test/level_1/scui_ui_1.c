/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t  menial_arc_w;
    scui_coord_t  menial_arc_v;
    scui_handle_t menial_arc_1;
    scui_handle_t menial_arc_2;
    scui_handle_t menial_arc_3;
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
    static scui_coord_t arc_val = 0;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        #if 1
        // menial_arc:
        scui_ui_res_local->menial_arc_v += scui_ui_res_local->menial_arc_w;
        
        if (scui_ui_res_local->menial_arc_v == 0)
            scui_ui_res_local->menial_arc_w = +1;
        if (scui_ui_res_local->menial_arc_v == 100)
            scui_ui_res_local->menial_arc_w = -1;
        
        scui_menial_arc_update_value(scui_ui_res_local->menial_arc_1,
            scui_ui_res_local->menial_arc_v, true);
        scui_menial_arc_update_value(scui_ui_res_local->menial_arc_2,
            scui_ui_res_local->menial_arc_v, true);
        scui_menial_arc_update_value(scui_ui_res_local->menial_arc_3,
            scui_ui_res_local->menial_arc_v, true);
        #endif
        
        
        
        break;
    }
    case scui_event_create: {
        
        scui_menial_data_t menial_data_zero = {0};
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
        menial_maker.data = menial_data_zero;
        menial_maker.data.btn.color[0].color_l.full = 0xFF00FF00;
        menial_maker.data.btn.color[0].color_d.full = 0xFF008000;
        menial_maker.data.btn.color[1].color_l.full = 0xFFFF0000;
        menial_maker.data.btn.color[1].color_d.full = 0xFF800000;
        
        menial_maker.data.btn.fixed  = 1;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x = (SCUI_HOR_RES - 120) / 2;
        menial_maker.widget.clip.y = 30;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 0;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 1 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = 30 + 60 + 10;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = 5;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = 30 + 60 + 10;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 2;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 3 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = 30 + 60 + 10;
        scui_widget_create(&menial_maker, &menial_handle);
        #endif
        
        #if 1
        scui_ui_res_local->menial_arc_w = +1;
        // menial_arc:
        menial_maker.widget.clip.w = 100;
        menial_maker.widget.clip.h = 100;
        menial_maker.widget.event_cb = NULL;
        menial_maker.type = scui_menial_type_arc;
        menial_maker.data = menial_data_zero;
        menial_maker.data.arc.color[0].color_s.full = 0xFF000080;
        menial_maker.data.arc.color[1].color_s.full = 0xFF0000FF;
        menial_maker.data.arc.color[0].color_e.full = 0xFF008000;
        menial_maker.data.arc.color[1].color_e.full = 0xFF00FF00;
        menial_maker.data.arc.center.x = 100 / 2;
        menial_maker.data.arc.center.y = 100 / 2;
        menial_maker.data.arc.radius  = 100 / 2;
        menial_maker.data.arc.angle_s = 0;
        menial_maker.data.arc.angle_e = 0;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 100 / 2;
        menial_maker.widget.clip.y = 30 + 60 + 10 + 60 + 10;
        menial_maker.data.arc.width = 8;
        menial_maker.data.arc.round = 1;
        menial_maker.data.arc.grad  = 0;
        menial_maker.data.arc.anti  = 0;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_arc_1 = menial_handle;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 100 / 2;
        menial_maker.widget.clip.y = 30 + 60 + 10 + 60 + 10;
        menial_maker.data.arc.width = 0;
        menial_maker.data.arc.round = 0;
        menial_maker.data.arc.grad  = 0;
        menial_maker.data.arc.anti  = 1;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_arc_2 = menial_handle;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 100 / 2;
        menial_maker.widget.clip.y = 30 + 60 + 10 + 60 + 10;
        menial_maker.data.arc.width = 8;
        menial_maker.data.arc.round = 1;
        menial_maker.data.arc.gradw = 1;
        menial_maker.data.arc.grad  = 1;
        menial_maker.data.arc.anti  = 0;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_arc_3 = menial_handle;
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
