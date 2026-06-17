/*实现目标:
 *    窗口:xxx
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
    scui_coord_t  menial_bar_w1;
    scui_coord_t  menial_bar_v1;
    scui_handle_t menial_bar_1;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_menial_btn_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_button_click:
        SCUI_LOG_WARN("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_menial_arc_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_update_value: {
        scui_coord3_t angle = 0.0f;
        scui_menial_arc_current_angle(event->object, &angle);
        // SCUI_LOG_WARN("event %u widget %u angle:%.2f", event->type, event->object, angle);
        break;
    }
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_menial_bar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_update_value: {
        scui_coord3_t value = 0.0f;
        scui_menial_bar_current_value(event->object, &value);
        // SCUI_LOG_WARN("event %u widget %u value:%.2f", event->type, event->object, value);
        break;
    }
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_menial_cht_event_proc(scui_event_t *event)
{
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
        
        // scui_menial_arc_update_value(scui_ui_res_local->menial_arc_1,
        //     scui_ui_res_local->menial_arc_v, false);
        scui_menial_arc_update_value(scui_ui_res_local->menial_arc_2,
            scui_ui_res_local->menial_arc_v, false);
        scui_menial_arc_update_value(scui_ui_res_local->menial_arc_3,
            scui_ui_res_local->menial_arc_v, false);
        #endif
        
        #if 1
        // menial_bar:
        scui_ui_res_local->menial_bar_v1 += scui_ui_res_local->menial_bar_w1;
        
        if (scui_ui_res_local->menial_bar_v1 == 0)
            scui_ui_res_local->menial_bar_w1 = +1;
        if (scui_ui_res_local->menial_bar_v1 == 100)
            scui_ui_res_local->menial_bar_w1 = -1;
        
        scui_menial_bar_update_value(scui_ui_res_local->menial_bar_1,
            scui_ui_res_local->menial_bar_v1, false);
        #endif
        
        break;
    }
    case scui_event_create: {
        
        scui_menial_data_t menial_data_zero = {0};
        scui_menial_maker_t menial_maker = {0};
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&menial_maker, scui_widget_type_menial);
        menial_maker.widget.parent = event->object;
        
        #if 1
        // menial_btn:
        scui_coord_t btn_ofs_y = 30 + 60 + 10;
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
        menial_maker.widget.clip.y   = btn_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 0;
        menial_maker.data.btn.radius = 5;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = btn_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.data.btn.fixed  = 0;
        menial_maker.data.btn.check  = 1;
        menial_maker.data.btn.width  = 2;
        menial_maker.data.btn.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 3 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = btn_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        #endif
        
        #if 1
        // menial_arc:
        scui_coord_t arc_ofs_y = btn_ofs_y + 60 + 10;
        menial_maker.widget.clip.w = 100;
        menial_maker.widget.clip.h = 100;
        menial_maker.widget.event_cb = scui_ui_scene_1_menial_arc_event_proc;
        menial_maker.type = scui_menial_type_arc;
        menial_maker.data = menial_data_zero;
        menial_maker.data.arc.color[0].color_s.full = 0xFF000080;
        menial_maker.data.arc.color[1].color_s.full = 0xFF0000FF;
        menial_maker.data.arc.color[0].color_e.full = 0xFF008000;
        menial_maker.data.arc.color[1].color_e.full = 0xFF00FF00;
        menial_maker.data.arc.center.x = 100 / 2;
        menial_maker.data.arc.center.y = 100 / 2;
        menial_maker.data.arc.radius  = 100 / 2;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 100 / 2;
        menial_maker.widget.clip.y = arc_ofs_y;
        menial_maker.data.arc.angle_s = 270;
        menial_maker.data.arc.angle_e = 270 + 60;
        menial_maker.data.arc.time    = 1500;
        menial_maker.data.arc.width   = 8;
        menial_maker.data.arc.spinner = 1;
        menial_maker.data.arc.round   = 1;
        menial_maker.data.arc.gradw   = 0;
        menial_maker.data.arc.grad    = 1;
        menial_maker.data.arc.anti    = 1;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_arc_1 = menial_handle;
        
        menial_maker.data.arc.angle_s = 0;
        menial_maker.data.arc.angle_e = 0;
        menial_maker.data.arc.time    = 0;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 100 / 2;
        menial_maker.widget.clip.y = arc_ofs_y;
        menial_maker.data.arc.width   = 0;
        menial_maker.data.arc.spinner = 0;
        menial_maker.data.arc.round   = 0;
        menial_maker.data.arc.gradw   = 1;
        menial_maker.data.arc.grad    = 0;
        menial_maker.data.arc.anti    = 1;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_arc_2 = menial_handle;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 100 / 2;
        menial_maker.widget.clip.y = arc_ofs_y;
        menial_maker.data.arc.time    = 0;
        menial_maker.data.arc.width   = 8;
        menial_maker.data.arc.spinner = 0;
        menial_maker.data.arc.round   = 1;
        menial_maker.data.arc.gradw   = 1;
        menial_maker.data.arc.grad    = 1;
        menial_maker.data.arc.anti    = 0;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_arc_3 = menial_handle;
        #endif
        
        #if 1
        // menial_bar:
        scui_coord_t bar_ofs_y = arc_ofs_y + 100 + 10;
        menial_maker.widget.event_cb = scui_ui_scene_1_menial_bar_event_proc;
        menial_maker.type = scui_menial_type_bar;
        menial_maker.data = menial_data_zero;
        menial_maker.data.bar.color[0].color_s.full = 0xFF000080;
        menial_maker.data.bar.color[1].color_s.full = 0xFF0000FF;
        menial_maker.data.bar.color[0].color_e.full = 0xFF008000;
        menial_maker.data.bar.color[1].color_e.full = 0xFF00FF00;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 120 / 2 - 10;
        menial_maker.widget.clip.y = bar_ofs_y;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        menial_maker.data.bar.radius = 7;
        menial_maker.data.bar.value_lim = 0;
        menial_maker.data.bar.ext_slider = 0;
        menial_maker.data.bar.ext_switch = 0;
        menial_maker.data.bar.grad = 1;
        menial_maker.data.bar.way  = 0;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_ui_res_local->menial_bar_1 = menial_handle;
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y = bar_ofs_y;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        menial_maker.data.bar.radius = -1;
        menial_maker.data.bar.value_lim = 0;
        menial_maker.data.bar.value_int = 0;
        menial_maker.data.bar.ext_slider = 0;
        menial_maker.data.bar.ext_switch = 1;
        menial_maker.data.bar.grad = 0;
        menial_maker.data.bar.way  = 0;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 120 / 2 + 10;
        menial_maker.widget.clip.y = bar_ofs_y;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        menial_maker.data.bar.radius = -1;
        menial_maker.data.bar.value_lim = 0;
        menial_maker.data.bar.value_int = 0;
        menial_maker.data.bar.ext_slider = 1;
        menial_maker.data.bar.ext_switch = 0;
        menial_maker.data.bar.grad = 1;
        menial_maker.data.bar.way  = 0;
        scui_widget_create(&menial_maker, &menial_handle);
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y = bar_ofs_y + 60 + 10;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 90;
        menial_maker.data.bar.radius = 10;
        menial_maker.data.bar.value_lim = 7;
        menial_maker.data.bar.value_int = 1;
        menial_maker.data.bar.ext_slider = 1;
        menial_maker.data.bar.ext_switch = 0;
        menial_maker.data.bar.grad = 1;
        menial_maker.data.bar.way  = 1;
        scui_widget_create(&menial_maker, &menial_handle);
        #endif
        
        
        
        break;
    }
    case scui_event_draw_graph: {
        
        scui_handle_t font = scui_font_symbol_24bin;
        uint32_t symbol = scui_symbol_code(SCUI_SYMBOL_STR_WARNING);
        
        scui_area_t widget_clip = {.x = 0, .y = 0, .w = scui_widget_clip(event->object).w, .h = scui_widget_clip(event->object).h};
        scui_area_t symbol_area = scui_symbol_area(font, symbol);
        widget_clip.x += (widget_clip.w - symbol_area.h) / 2;
        
        scui_widget_draw_symbol(event->object, &widget_clip, NULL, SCUI_COLOR_WHITE, font, symbol);
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
        
        scui_menial_data_t menial_data_zero = {0};
        scui_menial_maker_t menial_maker = {0};
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&menial_maker, scui_widget_type_menial);
        menial_maker.widget.parent = event->object;
        
        #if 1
        // menial_cht:
        scui_coord_t vlist[100] = {0};
        scui_coord_t vlist_min[100] = {0};
        scui_coord_t vlist_max[100] = {0};
        for (uint32_t idx = 0; idx < 100; idx++) {
            vlist_min[idx] =  60 + (uint32_t)scui_rand(0xFF) % 40;   //统一底点就是柱状图,否则为股价图
            vlist_max[idx] = 220 - (uint32_t)scui_rand(0xFF) % 40;
            vlist[idx] = 60 + (uint32_t)scui_rand(0xFF) % ((220 - 60));
        }
        
        menial_maker.widget.style.fully_bg = true;
        menial_maker.widget.color.color.full = 0xFF4F4F4F;
        menial_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
        menial_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
        menial_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
        menial_maker.widget.event_cb = scui_ui_scene_1_menial_cht_event_proc;
        menial_maker.type = scui_menial_type_cht;
        menial_maker.data = menial_data_zero;
        menial_maker.data.cht.round = true;
        menial_maker.data.cht.area.x = 10;
        menial_maker.data.cht.area.y = 10;
        menial_maker.data.cht.area.w = menial_maker.widget.clip.w - 10 * 2;
        menial_maker.data.cht.area.h = menial_maker.widget.clip.h - 10 * 2;
        menial_maker.data.cht.color.color.full = 0xFFFF0000;
        menial_maker.data.cht.value_min = 60;
        menial_maker.data.cht.value_max = 220;
        
        
        
        menial_maker.widget.clip.x = SCUI_HOR_RES *  1 / 25;
        menial_maker.data.cht.type   = 0;
        menial_maker.data.cht.number = 15;
        menial_maker.data.cht.space  = 4;
        menial_maker.data.cht.width  = 6;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_menial_cht_hist_data(menial_handle, vlist_min, vlist_max);
        
        menial_maker.widget.clip.x = SCUI_HOR_RES * 13 / 25;
        menial_maker.data.cht.type   = 1;
        menial_maker.data.cht.number = 30;
        menial_maker.data.cht.space  = 4;
        menial_maker.data.cht.width  = 2;
        scui_widget_create(&menial_maker, &menial_handle);
        scui_menial_cht_line_data(menial_handle, vlist);
        
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
void scui_ui_scene_1_scroll_page_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        #if 0
        // test tvg cache block draw
        scui_menial_maker_t menial_maker = {0};
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&menial_maker, scui_widget_type_menial);
        menial_maker.widget.parent = event->object;
        
        menial_maker.type = scui_menial_type_bar;
        menial_maker.data.bar.color[0].color_s.full = 0xFF000080;
        menial_maker.data.bar.color[1].color_s.full = 0xFF0000FF;
        menial_maker.data.bar.color[0].color_e.full = 0xFF008000;
        menial_maker.data.bar.color[1].color_e.full = 0xFF00FF00;
        
        menial_maker.widget.clip.w = SCUI_HOR_RES * 3 / 4;
        menial_maker.widget.clip.h = SCUI_VER_RES * 1 / 2;
        menial_maker.widget.clip.x = (SCUI_HOR_RES - menial_maker.widget.clip.w) / 2;
        menial_maker.widget.clip.y = (SCUI_VER_RES - menial_maker.widget.clip.h) / 2;
        menial_maker.data.bar.radius = 23;
        menial_maker.data.bar.value_lim = 7;
        menial_maker.data.bar.value_int = 1;
        menial_maker.data.bar.ext_slider = 1;
        menial_maker.data.bar.ext_switch = 0;
        menial_maker.data.bar.grad = 1;
        menial_maker.data.bar.way  = 1;
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
void scui_ui_scene_1_scroll_event(scui_event_t *event)
{
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
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}
