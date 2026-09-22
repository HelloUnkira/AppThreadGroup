/*实现目标:
 *    测试(widget object)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t  obj_arc_w;    /* 圆弧值方向 */
    scui_coord_t  obj_arc_v;    /* 圆弧值 */
    scui_handle_t obj_arc_1;    /* 圆弧控件 */
    scui_handle_t obj_arc_2;    /* 圆弧控件 */
    scui_handle_t obj_arc_3;    /* 圆弧控件 */
    scui_coord_t  obj_bar_w1;   /* 条形值方向 */
    scui_coord_t  obj_bar_v1;   /* 条形值 */
    scui_handle_t obj_bar_1;    /* 条形控件 */
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_btn_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_button_click:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_arc_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_update_value: {
        scui_coord3_t angle = 0.0f;
        scui_obj_arc_current_angle(event->object, &angle);
        SCUI_LOG_INFO("arc angle:%.2f", angle);
        break;
    }
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_bar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_update_value: {
        scui_coord3_t value = 0.0f;
        scui_obj_bar_current_value(event->object, &value);
        SCUI_LOG_INFO("bar value:%.2f", value);
        break;
    }
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_cht_event_proc(scui_event_t *event)
{
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_scroll_event_proc(scui_event_t *event)
{
}

/*@brief page_1 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_page_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        #if 1
        // obj_btn:
        scui_obj_btn_maker_define(obj_btn_maker);
        scui_handle_t obj_btn_handle = SCUI_HANDLE_INVALID;
        obj_btn_maker.widget.parent = event->object;
        
        scui_coord_t obj_btn_ofs_y = 30 + 60 + 10;
        obj_btn_maker.widget.clip.w = 120;
        obj_btn_maker.widget.clip.h = 60;
        obj_btn_maker.widget.event_cb = scui_test_ui_object_btn_event_proc;
        
        scui_obj_btn_res_t obj_btn_res = {0};
        obj_btn_res.color[0].color_s.full = 0xFF00FF00;  /* def */
        obj_btn_res.color[1].color_s.full = 0xFF008000;  /* pre */
        obj_btn_res.color[2].color_s.full = 0xFFFF0000;  /* chk */
        obj_btn_res.color[3].color_s.full = 0xFF800000;  /* chk pre */
        
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 1;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = -1;
        obj_btn_maker.widget.clip.x = (SCUI_HOR_RES - 120) / 2;
        obj_btn_maker.widget.clip.y = 30;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 0;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = -1;
        obj_btn_maker.widget.clip.x   = SCUI_HOR_RES * 1 / 4 - 120 / 2;
        obj_btn_maker.widget.clip.y   = obj_btn_ofs_y;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 5;
        obj_btn_maker.widget.clip.x   = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        obj_btn_maker.widget.clip.y   = obj_btn_ofs_y;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_maker.widget.clip.x   = SCUI_HOR_RES * 3 / 4 - 120 / 2;
        obj_btn_maker.widget.clip.y   = obj_btn_ofs_y;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        
        /* 四样式同显(bg/edge/box/sha), 覆盖自test_ui_button倒数1 */
        scui_obj_btn_res_t obj_btn_bg_res = {0};
        obj_btn_bg_res.part = scui_object_part_rect_bg;
        obj_btn_bg_res.color[0].color_s.full = 0xFF87CEFA;  /* def */
        obj_btn_bg_res.color[1].color_s.full = 0xFF4682B4;  /* pre */
        obj_btn_bg_res.color[2].color_s.full = 0xFF87CEFA;  /* chk */
        obj_btn_bg_res.color[3].color_s.full = 0xFF4682B4;  /* chk pre */
        obj_btn_bg_res.area.w = 120 - (12 + 4 + 4) * 2;
        obj_btn_bg_res.area.h = 60 - (12 + 4 + 4) * 2;
        obj_btn_bg_res.width  = 0;
        obj_btn_bg_res.radius = 30 - (12 + 4 + 4);
        
        scui_obj_btn_res_t obj_btn_edge_res = {0};
        obj_btn_edge_res.part = scui_object_part_rect_edge;
        obj_btn_edge_res.color[0].color_s.full = 0xFFFFFFFF;  /* def */
        obj_btn_edge_res.color[1].color_s.full = 0xFFFFFFFF;  /* pre */
        obj_btn_edge_res.color[2].color_s.full = 0xFFFFFFFF;  /* chk */
        obj_btn_edge_res.color[3].color_s.full = 0xFFFFFFFF;  /* chk pre */
        obj_btn_edge_res.area.w = 120 - (12 + 4) * 2;
        obj_btn_edge_res.area.h = 60 - (12 + 4) * 2;
        obj_btn_edge_res.width  = 4;
        obj_btn_edge_res.radius = 30 - (12 + 4);
        
        scui_obj_btn_res_t obj_btn_box_res = {0};
        obj_btn_box_res.part  = scui_object_part_rect_box;
        obj_btn_box_res.color[0].color_s.full = 0xFFFF0000;  /* def */
        obj_btn_box_res.color[1].color_s.full = 0xFFFF0000;  /* pre */
        obj_btn_box_res.color[2].color_s.full = 0xFFFF0000;  /* chk */
        obj_btn_box_res.color[3].color_s.full = 0xFFFF0000;  /* chk pre */
        obj_btn_box_res.area.w = 120 - (12) * 2;
        obj_btn_box_res.area.h = 60 - (12) * 2;
        obj_btn_box_res.width  = 4;
        obj_btn_box_res.radius = 30 - (12);
        
        scui_obj_btn_res_t obj_btn_sha_res = {0};
        obj_btn_sha_res.part  = scui_object_part_rect_sha;
        obj_btn_sha_res.color[0].color_s.full = 0xFF00FF00;  /* def */
        obj_btn_sha_res.color[1].color_s.full = 0xFF00FF00;  /* pre */
        obj_btn_sha_res.color[2].color_s.full = 0xFF00FF00;  /* chk */
        obj_btn_sha_res.color[3].color_s.full = 0xFF00FF00;  /* chk pre */
        obj_btn_sha_res.area.w = 120 - (0) * 2;
        obj_btn_sha_res.area.h = 60 - (0) * 2;
        obj_btn_sha_res.width  = 12;
        obj_btn_sha_res.radius = 30 - (0);
        obj_btn_sha_res.shadow = 1;
        
        scui_obj_btn_style(obj_btn_handle, &obj_btn_bg_res);
        scui_obj_btn_style(obj_btn_handle, &obj_btn_edge_res);
        scui_obj_btn_style(obj_btn_handle, &obj_btn_box_res);
        scui_obj_btn_style(obj_btn_handle, &obj_btn_sha_res);
        #endif
        
        #if 1
        // obj_arc:
        scui_obj_arc_maker_define(obj_arc_maker);
        scui_handle_t obj_arc_handle = SCUI_HANDLE_INVALID;
        obj_arc_maker.widget.parent = event->object;
        
        scui_coord_t obj_arc_ofs_y = obj_btn_ofs_y + 60 + 10;
        obj_arc_maker.widget.clip.w = 100;
        obj_arc_maker.widget.clip.h = 100;
        obj_arc_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        scui_obj_arc_res_t obj_arc_res = {0};
        obj_arc_res.color[0].color_s.full = 0xFF000080;
        obj_arc_res.color[1].color_s.full = 0xFF0000FF;
        obj_arc_res.color[0].color_e.full = 0xFF008000;
        obj_arc_res.color[1].color_e.full = 0xFF00FF00;
        obj_arc_res.center.x = 100 / 2;
        obj_arc_res.center.y = 100 / 2;
        obj_arc_res.radius   = 100 / 2;
        obj_arc_res.time     = 1500;
        
        obj_arc_maker.anti = 1;
        obj_arc_maker.ext_touch = 0;
        obj_arc_maker.ext_spinner = 1;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 100 / 2;
        obj_arc_maker.widget.clip.y = obj_arc_ofs_y;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_local->obj_arc_1 = obj_arc_handle;
        
        obj_arc_res.angle_s = 0;
        obj_arc_res.angle_e = 0;
        obj_arc_res.time    = 0;
        
        obj_arc_maker.anti = 1;
        obj_arc_maker.ext_touch = 0;
        obj_arc_maker.ext_spinner = 0;
        obj_arc_res.width   = 0;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 0;
        obj_arc_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 100 / 2;
        obj_arc_maker.widget.clip.y = obj_arc_ofs_y;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_local->obj_arc_2 = obj_arc_handle;
        
        obj_arc_maker.anti = 0;
        obj_arc_maker.ext_touch = 1;
        obj_arc_maker.ext_spinner = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 100 / 2;
        obj_arc_maker.widget.clip.y = obj_arc_ofs_y;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_local->obj_arc_3 = obj_arc_handle;
        #endif
        
        #if 1
        // obj_bar:
        scui_obj_bar_maker_define(obj_bar_maker);
        scui_handle_t obj_bar_handle = SCUI_HANDLE_INVALID;
        obj_bar_maker.widget.parent = event->object;
        
        scui_coord_t obj_bar_ofs_y = obj_arc_ofs_y + 100 + 10;
        obj_bar_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        obj_bar_res.color[0].color_s.full = 0xFF000080;
        obj_bar_res.color[1].color_s.full = 0xFF0000FF;
        obj_bar_res.color[0].color_e.full = 0xFF008000;
        obj_bar_res.color[1].color_e.full = 0xFF00FF00;
        
        obj_bar_maker.value_lim = 100;
        obj_bar_res.radius = 7;
        obj_bar_res.grad = 1;
        obj_bar_maker.way  = 0;
        obj_bar_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 120 / 2 - 10;
        obj_bar_maker.widget.clip.y = obj_bar_ofs_y;
        obj_bar_maker.widget.clip.w = 120;
        obj_bar_maker.widget.clip.h = 60;
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        scui_ui_res_local->obj_bar_1 = obj_bar_handle;
        
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.ext_switch = 1;
        obj_bar_maker.ext_slider = 0;
        obj_bar_res.radius = -1;
        obj_bar_res.grad = 0;
        obj_bar_maker.way  = 0;
        obj_bar_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        obj_bar_maker.widget.clip.y = obj_bar_ofs_y;
        obj_bar_maker.widget.clip.w = 120;
        obj_bar_maker.widget.clip.h = 60;
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.ext_slider = 1;
        obj_bar_maker.ext_switch = 0;
        obj_bar_res.radius = -1;
        obj_bar_res.grad = 1;
        obj_bar_maker.way  = 0;
        obj_bar_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 120 / 2 + 10;
        obj_bar_maker.widget.clip.y = obj_bar_ofs_y;
        obj_bar_maker.widget.clip.w = 120;
        obj_bar_maker.widget.clip.h = 60;
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        
        obj_bar_maker.value_lim = 7;
        obj_bar_maker.value_int = 1;
        obj_bar_maker.ext_slider = 1;
        obj_bar_maker.ext_switch = 0;
        obj_bar_maker.way = 1;
        obj_bar_res.radius = 10;
        obj_bar_res.grad = 1;
        obj_bar_maker.way  = 1;
        obj_bar_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        obj_bar_maker.widget.clip.y = obj_bar_ofs_y + 60 + 10;
        obj_bar_maker.widget.clip.w = 120;
        obj_bar_maker.widget.clip.h = 90;
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        #endif
        break;
    }
    default:
        break;
    }
}

/*@brief page_2 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_page_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        #if 1
        // obj_chart:
        scui_obj_chart_maker_define(obj_chart_maker);
        scui_handle_t obj_chart_handle = SCUI_HANDLE_INVALID;
        
        obj_chart_maker.widget.parent = event->object;
        
        scui_coord_t vlist[100] = {0};
        scui_coord_t vlist_min[100] = {0};
        scui_coord_t vlist_max[100] = {0};
        for (uint32_t idx = 0; idx < 100; idx++) {
            vlist_min[idx] =  60 + (uint32_t)scui_rand(0xFF) % 40;   //统一底点就是柱状图,否则为股价图
            vlist_max[idx] = 220 - (uint32_t)scui_rand(0xFF) % 40;
            vlist[idx] = 60 + (uint32_t)scui_rand(0xFF) % ((220 - 60));
        }
        
        obj_chart_maker.widget.style.fully_bg = true;
        obj_chart_maker.widget.color.color.full = 0xFF4F4F4F;
        obj_chart_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
        obj_chart_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
        obj_chart_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
        obj_chart_maker.widget.event_cb = scui_test_ui_object_cht_event_proc;
        
        scui_obj_chart_res_t obj_chart_res = {0};
        obj_chart_res.round = true;
        obj_chart_res.color.color.full = 0xFFFF0000;
        
        obj_chart_maker.value_min = 60;
        obj_chart_maker.value_max = 220;
        obj_chart_maker.area.x = 10;
        obj_chart_maker.area.y = 10;
        obj_chart_maker.area.w = obj_chart_maker.widget.clip.w - 10 * 2;
        obj_chart_maker.area.h = obj_chart_maker.widget.clip.h - 10 * 2;
        
        obj_chart_maker.type   = 0;
        obj_chart_maker.number = 19;
        obj_chart_maker.space  = 4;
        obj_chart_res.width = 6;
        obj_chart_maker.widget.clip.x = SCUI_HOR_RES *  1 / 25;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_rect_item;
        scui_obj_chart_style(obj_chart_handle, &obj_chart_res);
        scui_obj_chart_hist_data(obj_chart_handle, vlist_min, vlist_max);
        
        obj_chart_maker.type   = 1;
        obj_chart_maker.number = 30;
        obj_chart_maker.space  = 4;
        obj_chart_res.width = 2;
        obj_chart_res.grad = true;
        obj_chart_maker.widget.clip.x = SCUI_HOR_RES * 13 / 25;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_line_item;
        scui_obj_chart_style(obj_chart_handle, &obj_chart_res);
        scui_obj_chart_line_data(obj_chart_handle, vlist);
        #endif
        break;
    }
    default:
        break;
    }
}

/*@brief page_3 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_page_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        #if 0
        // test tvg cache block draw
        scui_obj_bar_maker_define(obj_bar_maker);
        scui_handle_t obj_bar_handle = SCUI_HANDLE_INVALID;
        
        obj_bar_maker.widget.parent = event->object;
        
        obj_bar_maker.widget.clip.w = SCUI_HOR_RES * 3 / 4;
        obj_bar_maker.widget.clip.h = SCUI_VER_RES * 1 / 2;
        obj_bar_maker.widget.clip.x = (SCUI_HOR_RES - obj_bar_maker.widget.clip.w) / 2;
        obj_bar_maker.widget.clip.y = (SCUI_VER_RES - obj_bar_maker.widget.clip.h) / 2;
        obj_bar_maker.value_lim = 7;
        obj_bar_maker.value_int = 1;
        obj_bar_maker.ext_slider = 1;
        obj_bar_maker.ext_switch = 0;
        obj_bar_maker.way  = 1;
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        
        scui_obj_bar_res_t obj_bar_res = {0};
        obj_bar_res.color[0].color_s.full = 0xFF000080;
        obj_bar_res.color[1].color_s.full = 0xFF0000FF;
        obj_bar_res.color[0].color_e.full = 0xFF008000;
        obj_bar_res.color[1].color_e.full = 0xFF00FF00;
        obj_bar_res.radius = 23;
        obj_bar_res.grad = 1;
        obj_bar_maker.way  = 1;
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        #endif
        
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        break;
    case scui_event_destroy:
        break;
    case scui_event_anima_elapse: {
        
        #if 1
        // obj_arc:
        scui_ui_res_local->obj_arc_v += scui_ui_res_local->obj_arc_w;
        
        if (scui_ui_res_local->obj_arc_v == 0)
            scui_ui_res_local->obj_arc_w = +1;
        if (scui_ui_res_local->obj_arc_v == 100)
            scui_ui_res_local->obj_arc_w = -1;
        
        // scui_obj_arc_update_value(scui_ui_res_local->obj_arc_1,
        //     scui_ui_res_local->obj_arc_v, false);
        scui_obj_arc_update_value(scui_ui_res_local->obj_arc_2,
            scui_ui_res_local->obj_arc_v, false);
        // scui_obj_arc_update_value(scui_ui_res_local->obj_arc_3,
        //     scui_ui_res_local->obj_arc_v, false);
        #endif
        
        #if 1
        // obj_bar:
        scui_ui_res_local->obj_bar_v1 += scui_ui_res_local->obj_bar_w1;
        
        if (scui_ui_res_local->obj_bar_v1 == 0)
            scui_ui_res_local->obj_bar_w1 = +1;
        if (scui_ui_res_local->obj_bar_v1 == 100)
            scui_ui_res_local->obj_bar_w1 = -1;
        
        scui_obj_bar_update_value(scui_ui_res_local->obj_bar_1,
            scui_ui_res_local->obj_bar_v1, false);
        #endif
        break;
    }
    default:
        break;
    }
}