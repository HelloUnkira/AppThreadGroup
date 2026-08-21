/*实现目标:
 *    窗口:scui_test_ui_object
 *    用途: 测试 menial 菜单对象控件(从 scui_ui_1 挖取)
 *    构件: scroll + 3 page
 *    page_1: menial_btn×4/menial_arc×3/menial_bar×4
 *    page_2: menial_cht×2
 *    page_3: 空
 *    交互: 动画持续更新 arc/bar 数值
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t  menial_arc_w;    /* 圆弧值方向 */
    scui_coord_t  menial_arc_v;    /* 圆弧值 */
    scui_handle_t menial_arc_1;    /* 圆弧控件 */
    scui_handle_t menial_arc_2;    /* 圆弧控件 */
    scui_handle_t menial_arc_3;    /* 圆弧控件 */
    scui_coord_t  menial_bar_w1;   /* 条形值方向 */
    scui_coord_t  menial_bar_v1;   /* 条形值 */
    scui_handle_t menial_bar_1;    /* 条形控件 */
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
        scui_menial_arc_current_angle(event->object, &angle);
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
        scui_menial_bar_current_value(event->object, &value);
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
        
        scui_menial_maker_define(menial_maker);
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        menial_maker.widget.parent = event->object;
        
        #if 1
        // menial_btn:
        scui_coord_t btn_ofs_y = 30 + 60 + 10;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        menial_maker.widget.event_cb = scui_test_ui_object_btn_event_proc;
        menial_maker.type = scui_menial_type_btn;
        
        scui_menial_btn_res_t btn_res = {0};
        btn_res.color[0].color_s.full = 0xFF00FF00;  /* def */
        btn_res.color[1].color_s.full = 0xFF008000;  /* pre */
        btn_res.color[2].color_s.full = 0xFFFF0000;  /* chk */
        btn_res.color[3].color_s.full = 0xFF800000;  /* chk pre */
        
        menial_maker.data.btn.fixed = 1;
        menial_maker.data.btn.check = 1;
        btn_res.width  = 0;
        btn_res.radius = -1;
        menial_maker.widget.clip.x = (SCUI_HOR_RES - 120) / 2;
        menial_maker.widget.clip.y = 30;
        scui_widget_create(&menial_maker, &menial_handle);
        btn_res.part = scui_object_part_rect_bg;
        scui_menial_btn_style(menial_handle, &btn_res);
        
        menial_maker.data.btn.fixed = 0;
        menial_maker.data.btn.check = 0;
        btn_res.width  = 0;
        btn_res.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 1 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = btn_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        btn_res.part = scui_object_part_rect_bg;
        scui_menial_btn_style(menial_handle, &btn_res);
        
        menial_maker.data.btn.fixed = 0;
        menial_maker.data.btn.check = 1;
        btn_res.width  = 0;
        btn_res.radius = 5;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = btn_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        btn_res.part = scui_object_part_rect_bg;
        scui_menial_btn_style(menial_handle, &btn_res);
        
        menial_maker.data.btn.fixed = 0;
        menial_maker.data.btn.check = 1;
        btn_res.width  = 2;
        btn_res.radius = -1;
        menial_maker.widget.clip.x   = SCUI_HOR_RES * 3 / 4 - 120 / 2;
        menial_maker.widget.clip.y   = btn_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        btn_res.part = scui_object_part_rect_bg;
        scui_menial_btn_style(menial_handle, &btn_res);
        #endif
        
        #if 1
        // menial_arc:
        scui_coord_t arc_ofs_y = btn_ofs_y + 60 + 10;
        menial_maker.widget.clip.w = 100;
        menial_maker.widget.clip.h = 100;
        menial_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        menial_maker.type = scui_menial_type_arc;
        
        scui_menial_arc_res_t arc_res = {0};
        arc_res.color[0].color_s.full = 0xFF000080;
        arc_res.color[1].color_s.full = 0xFF0000FF;
        arc_res.color[0].color_e.full = 0xFF008000;
        arc_res.color[1].color_e.full = 0xFF00FF00;
        arc_res.center.x = 100 / 2;
        arc_res.center.y = 100 / 2;
        arc_res.radius   = 100 / 2;
        arc_res.time     = 1500;
        
        menial_maker.data.arc.spinner = 1;
        menial_maker.data.arc.anti    = 1;
        arc_res.width   = 8;
        arc_res.round   = 1;
        arc_res.gradw   = 0;
        arc_res.grad    = 1;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 100 / 2;
        menial_maker.widget.clip.y = arc_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        
        arc_res.part = scui_object_part_arc_bg;
        scui_menial_arc_style(menial_handle, &arc_res);
        
        arc_res.angle_s = 270;
        arc_res.angle_e = 270 + 60;
        arc_res.part = scui_object_part_arc_fg;
        scui_menial_arc_style(menial_handle, &arc_res);
        scui_ui_res_local->menial_arc_1 = menial_handle;
        
        arc_res.angle_s = 0;
        arc_res.angle_e = 0;
        arc_res.time    = 0;
        
        menial_maker.data.arc.spinner = 0;
        menial_maker.data.arc.anti    = 1;
        arc_res.width   = 0;
        arc_res.round   = 0;
        arc_res.gradw   = 1;
        arc_res.grad    = 0;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 100 / 2;
        menial_maker.widget.clip.y = arc_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        arc_res.part = scui_object_part_arc_bg;
        scui_menial_arc_style(menial_handle, &arc_res);
        arc_res.part = scui_object_part_arc_fg;
        scui_menial_arc_style(menial_handle, &arc_res);
        scui_ui_res_local->menial_arc_2 = menial_handle;
        
        menial_maker.data.arc.spinner = 0;
        menial_maker.data.arc.anti    = 0;
        arc_res.width   = 8;
        arc_res.round   = 1;
        arc_res.gradw   = 1;
        arc_res.grad    = 1;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 100 / 2;
        menial_maker.widget.clip.y = arc_ofs_y;
        scui_widget_create(&menial_maker, &menial_handle);
        arc_res.part = scui_object_part_arc_bg;
        scui_menial_arc_style(menial_handle, &arc_res);
        arc_res.part = scui_object_part_arc_fg;
        scui_menial_arc_style(menial_handle, &arc_res);
        scui_ui_res_local->menial_arc_3 = menial_handle;
        #endif
        
        #if 1
        // menial_bar:
        scui_coord_t bar_ofs_y = arc_ofs_y + 100 + 10;
        menial_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        menial_maker.type = scui_menial_type_bar;
        
        scui_menial_bar_res_t bar_res = {0};
        bar_res.color[0].color_s.full = 0xFF000080;
        bar_res.color[1].color_s.full = 0xFF0000FF;
        bar_res.color[0].color_e.full = 0xFF008000;
        bar_res.color[1].color_e.full = 0xFF00FF00;
        
        menial_maker.data.bar.value_lim = 100;
        bar_res.radius = 7;
        bar_res.grad = 1;
        menial_maker.data.bar.way  = 0;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 1 / 4 - 120 / 2 - 10;
        menial_maker.widget.clip.y = bar_ofs_y;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        scui_widget_create(&menial_maker, &menial_handle);
        bar_res.part = scui_object_part_rect_bg;
        scui_menial_bar_style(menial_handle, &bar_res);
        bar_res.part = scui_object_part_rect_fg;
        scui_menial_bar_style(menial_handle, &bar_res);
        scui_ui_res_local->menial_bar_1 = menial_handle;
        
        menial_maker.data.bar.value_lim = 100;
        menial_maker.data.bar.ext_switch = 1;
        menial_maker.data.bar.ext_slider = 0;
        bar_res.radius = -1;
        bar_res.grad = 0;
        menial_maker.data.bar.way  = 0;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y = bar_ofs_y;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        scui_widget_create(&menial_maker, &menial_handle);
        bar_res.part = scui_object_part_rect_bg;
        scui_menial_bar_style(menial_handle, &bar_res);
        bar_res.part = scui_object_part_rect_fg;
        scui_menial_bar_style(menial_handle, &bar_res);
        
        menial_maker.data.bar.value_lim = 100;
        menial_maker.data.bar.ext_slider = 1;
        menial_maker.data.bar.ext_switch = 0;
        bar_res.radius = -1;
        bar_res.grad = 1;
        menial_maker.data.bar.way  = 0;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 3 / 4 - 120 / 2 + 10;
        menial_maker.widget.clip.y = bar_ofs_y;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 60;
        scui_widget_create(&menial_maker, &menial_handle);
        bar_res.part = scui_object_part_rect_bg;
        scui_menial_bar_style(menial_handle, &bar_res);
        bar_res.part = scui_object_part_rect_fg;
        scui_menial_bar_style(menial_handle, &bar_res);
        
        menial_maker.data.bar.value_lim = 7;
        menial_maker.data.bar.value_int = 1;
        menial_maker.data.bar.ext_slider = 1;
        menial_maker.data.bar.ext_switch = 0;
        menial_maker.data.bar.way = 1;
        bar_res.radius = 10;
        bar_res.grad = 1;
        menial_maker.data.bar.way  = 1;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 2 / 4 - 120 / 2;
        menial_maker.widget.clip.y = bar_ofs_y + 60 + 10;
        menial_maker.widget.clip.w = 120;
        menial_maker.widget.clip.h = 90;
        scui_widget_create(&menial_maker, &menial_handle);
        bar_res.part = scui_object_part_rect_bg;
        scui_menial_bar_style(menial_handle, &bar_res);
        bar_res.part = scui_object_part_rect_fg;
        scui_menial_bar_style(menial_handle, &bar_res);
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
        
        scui_menial_maker_define(menial_maker);
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        
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
        menial_maker.widget.event_cb = scui_test_ui_object_cht_event_proc;
        menial_maker.type = scui_menial_type_cht;
        
        scui_menial_cht_res_t cht_res = {0};
        cht_res.round = true;
        cht_res.color.color.full = 0xFFFF0000;
        
        menial_maker.data.cht.value_min = 60;
        menial_maker.data.cht.value_max = 220;
        menial_maker.data.cht.area.x = 10;
        menial_maker.data.cht.area.y = 10;
        menial_maker.data.cht.area.w = menial_maker.widget.clip.w - 10 * 2;
        menial_maker.data.cht.area.h = menial_maker.widget.clip.h - 10 * 2;
        
        menial_maker.data.cht.type   = 0;
        menial_maker.data.cht.number = 15;
        menial_maker.data.cht.space  = 4;
        cht_res.width = 6;
        menial_maker.widget.clip.x = SCUI_HOR_RES *  1 / 25;
        scui_widget_create(&menial_maker, &menial_handle);
        cht_res.part = scui_object_part_rect_item;
        scui_menial_cht_style(menial_handle, &cht_res);
        scui_menial_cht_hist_data(menial_handle, vlist_min, vlist_max);
        
        menial_maker.data.cht.type   = 1;
        menial_maker.data.cht.number = 30;
        menial_maker.data.cht.space  = 4;
        cht_res.width = 2;
        menial_maker.widget.clip.x = SCUI_HOR_RES * 13 / 25;
        scui_widget_create(&menial_maker, &menial_handle);
        cht_res.part = scui_object_part_line_item;
        scui_menial_cht_style(menial_handle, &cht_res);
        scui_menial_cht_line_data(menial_handle, vlist);
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
        scui_menial_maker_define(menial_maker);
        scui_handle_t menial_handle = SCUI_HANDLE_INVALID;
        
        menial_maker.widget.parent = event->object;
        
        menial_maker.type = scui_menial_type_bar;
        
        menial_maker.widget.clip.w = SCUI_HOR_RES * 3 / 4;
        menial_maker.widget.clip.h = SCUI_VER_RES * 1 / 2;
        menial_maker.widget.clip.x = (SCUI_HOR_RES - menial_maker.widget.clip.w) / 2;
        menial_maker.widget.clip.y = (SCUI_VER_RES - menial_maker.widget.clip.h) / 2;
        menial_maker.data.bar.value_lim = 7;
        menial_maker.data.bar.value_int = 1;
        menial_maker.data.bar.ext_slider = 1;
        menial_maker.data.bar.ext_switch = 0;
        menial_maker.data.bar.way  = 1;
        scui_widget_create(&menial_maker, &menial_handle);
        
        scui_menial_bar_res_t bar_res = {0};
        bar_res.color[0].color_s.full = 0xFF000080;
        bar_res.color[1].color_s.full = 0xFF0000FF;
        bar_res.color[0].color_e.full = 0xFF008000;
        bar_res.color[1].color_e.full = 0xFF00FF00;
        bar_res.radius = 23;
        bar_res.grad = 1;
        menial_maker.data.bar.way  = 1;
        bar_res.part = scui_object_part_rect_bg;
        scui_menial_bar_style(menial_handle, &bar_res);
        bar_res.part = scui_object_part_rect_fg;
        scui_menial_bar_style(menial_handle, &bar_res);
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
    default:
        break;
    }
}
