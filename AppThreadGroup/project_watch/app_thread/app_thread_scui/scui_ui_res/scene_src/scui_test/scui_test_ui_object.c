/*实现目标:
 *    测试(widget object)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t  obj_arc_w;        /* 圆弧值方向 */
    scui_coord_t  obj_arc_v;        /* 圆弧值 */
    scui_handle_t obj_arc_anima;    /* 圆弧动画控件 */
    scui_coord_t  obj_bar_w1;       /* 条形值方向 */
    scui_coord_t  obj_bar_v1;       /* 条形值 */
    scui_handle_t obj_bar_anima;    /* 条形动画控件 */
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

/*@brief 页面标题事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_title_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_widget_t *widget = scui_handle_source_check(event->object);
        uint8_t *text = NULL;
        switch (widget->myself) {
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_TITLE:
            text = "Test Btn";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_TITLE:
            text = "Test Arc";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_TITLE:
            text = "Test Bar";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_4_TITLE:
            text = "Test Slider";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_5_TITLE:
            text = "Test Switch";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_6_TITLE:
            text = "Test Spinner";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_7_TITLE:
            text = "Test Chart";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_8_TITLE:
            text = "Test Empty";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_9_TITLE:
            text = "Test Empty";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_10_TITLE:
            text = "Test Empty";
            break;
        default:
            break;
        }
        scui_string_update_str(event->object, text);
        break;
    }
    default:
        break;
    }
}

/*@brief page_1 控件事件响应回调(Test Btn)
 *@param event 事件
 */
void scui_test_ui_object_page_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_btn_maker_define(obj_btn_maker);
        scui_handle_t obj_btn_handle = SCUI_HANDLE_INVALID;
        obj_btn_maker.widget.parent = event->object;
        obj_btn_maker.widget.event_cb = scui_test_ui_object_btn_event_proc;
        
        scui_obj_btn_res_t obj_btn_res = {0};
        scui_coord_t btn_w = 140;
        scui_coord_t btn_h = 100;
        
        /* 第1行: 绿/红/蓝 */
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFF00FF00;
        obj_btn_res.color[1].color_s.full = 0xFF008000;
        obj_btn_res.color[2].color_s.full = 0xFF00FF00;
        obj_btn_res.color[3].color_s.full = 0xFF008000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 13;
        obj_btn_maker.widget.clip.y = 48;
        obj_btn_maker.widget.clip.w = btn_w;
        obj_btn_maker.widget.clip.h = btn_h;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFFFF0000;
        obj_btn_res.color[1].color_s.full = 0xFF800000;
        obj_btn_res.color[2].color_s.full = 0xFFFF0000;
        obj_btn_res.color[3].color_s.full = 0xFF800000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = -1;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 48;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 0;
        obj_btn_res.color[0].color_s.full = 0xFF0000FF;
        obj_btn_res.color[1].color_s.full = 0xFF000080;
        obj_btn_res.color[2].color_s.full = 0xFF0000FF;
        obj_btn_res.color[3].color_s.full = 0xFF000080;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 48;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第2行: 四样式/渐变/圆头 */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res.color[1].color_s.full = 0xFF4682B4;
        obj_btn_res.color[2].color_s.full = 0xFF87CEFA;
        obj_btn_res.color[3].color_s.full = 0xFF4682B4;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 30;
        obj_btn_maker.widget.clip.x = 13;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        
        /* 四样式同显(bg/edge/box/sha) */
        scui_obj_btn_res_t obj_btn_bg_res = {0};
        obj_btn_bg_res.part = scui_object_part_rect_bg;
        obj_btn_bg_res.color[0].color_s.full = 0xFF87CEFA;
        obj_btn_bg_res.color[1].color_s.full = 0xFF4682B4;
        obj_btn_bg_res.color[2].color_s.full = 0xFF87CEFA;
        obj_btn_bg_res.color[3].color_s.full = 0xFF4682B4;
        obj_btn_bg_res.area.w = btn_w - (12 + 4 + 4) * 2;
        obj_btn_bg_res.area.h = btn_h - (12 + 4 + 4) * 2;
        obj_btn_bg_res.width  = 0;
        obj_btn_bg_res.radius = 30 - (12 + 4 + 4);
        
        scui_obj_btn_res_t obj_btn_edge_res = {0};
        obj_btn_edge_res.part = scui_object_part_rect_edge;
        obj_btn_edge_res.color[0].color_s.full = 0xFFFFFFFF;
        obj_btn_edge_res.color[1].color_s.full = 0xFFFFFFFF;
        obj_btn_edge_res.color[2].color_s.full = 0xFFFFFFFF;
        obj_btn_edge_res.color[3].color_s.full = 0xFFFFFFFF;
        obj_btn_edge_res.area.w = btn_w - (12 + 4) * 2;
        obj_btn_edge_res.area.h = btn_h - (12 + 4) * 2;
        obj_btn_edge_res.width  = 4;
        obj_btn_edge_res.radius = 30 - (12 + 4);
        
        scui_obj_btn_res_t obj_btn_box_res = {0};
        obj_btn_box_res.part  = scui_object_part_rect_box;
        obj_btn_box_res.color[0].color_s.full = 0xFFFF0000;
        obj_btn_box_res.color[1].color_s.full = 0xFFFF0000;
        obj_btn_box_res.color[2].color_s.full = 0xFFFF0000;
        obj_btn_box_res.color[3].color_s.full = 0xFFFF0000;
        obj_btn_box_res.area.w = btn_w - (12) * 2;
        obj_btn_box_res.area.h = btn_h - (12) * 2;
        obj_btn_box_res.width  = 4;
        obj_btn_box_res.radius = 30 - (12);
        
        scui_obj_btn_res_t obj_btn_sha_res = {0};
        obj_btn_sha_res.part  = scui_object_part_rect_sha;
        obj_btn_sha_res.color[0].color_s.full = 0xFF00FF00;
        obj_btn_sha_res.color[1].color_s.full = 0xFF00FF00;
        obj_btn_sha_res.color[2].color_s.full = 0xFF00FF00;
        obj_btn_sha_res.color[3].color_s.full = 0xFF00FF00;
        obj_btn_sha_res.area.w = btn_w - (0) * 2;
        obj_btn_sha_res.area.h = btn_h - (0) * 2;
        obj_btn_sha_res.width  = 12;
        obj_btn_sha_res.radius = 30 - (0);
        obj_btn_sha_res.shadow = 1;
        
        scui_obj_btn_style(obj_btn_handle, &obj_btn_bg_res);
        scui_obj_btn_style(obj_btn_handle, &obj_btn_edge_res);
        scui_obj_btn_style(obj_btn_handle, &obj_btn_box_res);
        scui_obj_btn_style(obj_btn_handle, &obj_btn_sha_res);
        
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 0;
        obj_btn_res.color[0].color_s.full = 0xFFFF8000;
        obj_btn_res.color[1].color_s.full = 0xFF804000;
        obj_btn_res.color[2].color_s.full = 0xFFFF8000;
        obj_btn_res.color[3].color_s.full = 0xFF804000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 25;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 0;
        obj_btn_res.color[0].color_s.full = 0xFF00FFFF;
        obj_btn_res.color[1].color_s.full = 0xFF008080;
        obj_btn_res.color[2].color_s.full = 0xFF00FFFF;
        obj_btn_res.color[3].color_s.full = 0xFF008080;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 50;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第3行: 黄/紫/青 */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 0;
        obj_btn_res.color[0].color_s.full = 0xFFFFFF00;
        obj_btn_res.color[1].color_s.full = 0xFF808000;
        obj_btn_res.color[2].color_s.full = 0xFFFFFF00;
        obj_btn_res.color[3].color_s.full = 0xFF808000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = -1;
        obj_btn_maker.widget.clip.x = 13;
        obj_btn_maker.widget.clip.y = 268;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFFFF00FF;
        obj_btn_res.color[1].color_s.full = 0xFF800080;
        obj_btn_res.color[2].color_s.full = 0xFFFF00FF;
        obj_btn_res.color[3].color_s.full = 0xFF800080;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 268;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFF00C8FF;
        obj_btn_res.color[1].color_s.full = 0xFF006080;
        obj_btn_res.color[2].color_s.full = 0xFF00C8FF;
        obj_btn_res.color[3].color_s.full = 0xFF006080;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 10;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 268;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_2 控件事件响应回调(Test Arc)
 *@param event 事件
 */
void scui_test_ui_object_page_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_arc_maker_define(obj_arc_maker);
        scui_handle_t obj_arc_handle = SCUI_HANDLE_INVALID;
        obj_arc_maker.widget.parent = event->object;
        obj_arc_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        scui_obj_arc_res_t obj_arc_res = {0};
        /* 统一渐变色调: bg 深蓝->深绿, fg 亮蓝->亮绿 */
        obj_arc_res.color[0].color_s.full = 0xFF000080;
        obj_arc_res.color[1].color_s.full = 0xFF0000FF;
        obj_arc_res.color[0].color_e.full = 0xFF008000;
        obj_arc_res.color[1].color_e.full = 0xFF00FF00;
        scui_coord_t arc_w = 100;
        scui_coord_t arc_h = 100;
        
        /* 第1行: 普通/整圆/反向 */
        obj_arc_res.center.x = arc_w / 2;
        obj_arc_res.center.y = arc_h / 2;
        obj_arc_res.radius   = arc_w / 2;
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_res.time     = 0;
        
        obj_arc_maker.anti = 0;
        obj_arc_maker.touch = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 28;
        obj_arc_maker.widget.clip.y = 45;
        obj_arc_maker.widget.clip.w = arc_w;
        obj_arc_maker.widget.clip.h = arc_h;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        obj_arc_res.width    = 8;
        obj_arc_res.round    = 0;
        obj_arc_res.gradw    = 1;
        obj_arc_res.grad     = 1;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_maker.anti = 1;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        /* 第2行: 跟手/细弧/粗弧 */
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_maker.anti = 0;
        obj_arc_maker.touch = 1;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 28;
        obj_arc_maker.widget.clip.y = 165;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_maker.touch = 0;
        obj_arc_res.width   = 4;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 300;
        obj_arc_res.width   = 16;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        /* 第3行: 动画/渐变/圆头粗弧 */
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 28;
        obj_arc_maker.widget.clip.y = 285;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_local->obj_arc_anima = obj_arc_handle;
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_res.width   = 12;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_local->obj_arc_v += scui_ui_res_local->obj_arc_w;
        
        if (scui_ui_res_local->obj_arc_v == 0)
            scui_ui_res_local->obj_arc_w = +1;
        if (scui_ui_res_local->obj_arc_v == 100)
            scui_ui_res_local->obj_arc_w = -1;
        
        scui_obj_arc_update_value(scui_ui_res_local->obj_arc_anima,
            scui_ui_res_local->obj_arc_v, false);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_3 控件事件响应回调(Test Bar)
 *@param event 事件
 */
void scui_test_ui_object_page_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_bar_maker_define(obj_bar_maker);
        scui_handle_t obj_bar_handle = SCUI_HANDLE_INVALID;
        obj_bar_maker.widget.parent = event->object;
        obj_bar_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        /* 统一渐变色调: bg 深蓝->深绿, fg 亮蓝->亮绿 */
        obj_bar_res.color[0].color_s.full = 0xFF000080;
        obj_bar_res.color[1].color_s.full = 0xFF0000FF;
        obj_bar_res.color[0].color_e.full = 0xFF008000;
        obj_bar_res.color[1].color_e.full = 0xFF00FF00;
        
        scui_coord_t cell_x[3] = {13, 163, 313};
        scui_coord_t cell_y[3] = {48, 158, 268};
        
        /* 第1行: 水平条(值30/70/100) */
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.way = 0;
        obj_bar_res.radius = 7;
        obj_bar_res.grad = 0;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_maker.widget.clip.x = cell_x[idx];
            obj_bar_maker.widget.clip.y = cell_y[0] + 38;
            obj_bar_maker.widget.clip.w = 140;
            obj_bar_maker.widget.clip.h = 24;
            scui_widget_create(&obj_bar_maker, &obj_bar_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_bar_handle, 30.0f + idx * 40, false);
        }
        
        /* 第2行: 圆角/渐变/分段 */
        obj_bar_res.radius = 15;
        obj_bar_maker.widget.clip.y = cell_y[1] + 35;
        obj_bar_maker.widget.clip.w = 140;
        obj_bar_maker.widget.clip.h = 30;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_maker.value_lim = 100;
            obj_bar_maker.value_int = 0;
            obj_bar_res.grad = (idx == 0) ? 0 : 1;
            if (idx == 2) {
                obj_bar_maker.value_lim = 5;
                obj_bar_maker.value_int = 1;
            }
            obj_bar_maker.widget.clip.x = cell_x[idx];
            scui_widget_create(&obj_bar_maker, &obj_bar_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_bar_handle, 60.0f - idx * 10, false);
        }
        
        /* 第3行: 垂直条(值40/80/分段) */
        obj_bar_maker.way = 1;
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.value_int = 0;
        obj_bar_res.radius = 7;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_res.grad = 0;
            if (idx == 2) {
                obj_bar_maker.value_lim = 5;
                obj_bar_maker.value_int = 1;
            }
            obj_bar_maker.widget.clip.x = cell_x[idx] + 58;
            obj_bar_maker.widget.clip.y = cell_y[2];
            obj_bar_maker.widget.clip.w = 24;
            obj_bar_maker.widget.clip.h = 100;
            scui_widget_create(&obj_bar_maker, &obj_bar_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_bar_handle, 40.0f + idx * 20, false);
        }
        
        /* 底部: 动画条 */
        obj_bar_maker.way = 0;
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.value_int = 0;
        obj_bar_res.radius = 15;
        obj_bar_res.grad = 1;
        obj_bar_maker.widget.clip.x = 83;
        obj_bar_maker.widget.clip.y = 375;
        obj_bar_maker.widget.clip.w = 300;
        obj_bar_maker.widget.clip.h = 30;
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        scui_ui_res_local->obj_bar_anima = obj_bar_handle;
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_local->obj_bar_v1 += scui_ui_res_local->obj_bar_w1;
        
        if (scui_ui_res_local->obj_bar_v1 == 0)
            scui_ui_res_local->obj_bar_w1 = +1;
        if (scui_ui_res_local->obj_bar_v1 == 100)
            scui_ui_res_local->obj_bar_w1 = -1;
        
        scui_obj_bar_update_value(scui_ui_res_local->obj_bar_anima,
            scui_ui_res_local->obj_bar_v1, false);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_4 控件事件响应回调(Test Slider)
 *@param event 事件
 */
void scui_test_ui_object_page_4_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_slider_maker_define(obj_slider_maker);
        scui_handle_t obj_slider_handle = SCUI_HANDLE_INVALID;
        obj_slider_maker.widget.parent = event->object;
        obj_slider_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        /* 统一渐变色调: bg 深蓝->深绿, fg 亮蓝->亮绿 */
        obj_bar_res.color[0].color_s.full = 0xFF000080;
        obj_bar_res.color[1].color_s.full = 0xFF0000FF;
        obj_bar_res.color[0].color_e.full = 0xFF008000;
        obj_bar_res.color[1].color_e.full = 0xFF00FF00;
        
        scui_coord_t cell_x[3] = {13, 163, 313};
        scui_coord_t cell_y[3] = {48, 158, 268};
        
        /* 第1行: 水平滑条(值30/70/100) */
        obj_slider_maker.obj_bar.value_lim = 100;
        obj_slider_maker.obj_bar.way = 0;
        obj_bar_res.radius = 7;
        obj_bar_res.grad = 0;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_slider_maker.widget.clip.x = cell_x[idx];
            obj_slider_maker.widget.clip.y = cell_y[0] + 38;
            obj_slider_maker.widget.clip.w = 140;
            obj_slider_maker.widget.clip.h = 24;
            scui_widget_create(&obj_slider_maker, &obj_slider_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_slider_handle, 30.0f + idx * 40, false);
        }
        
        /* 第2行: 圆角/渐变/分段 */
        obj_bar_res.radius = 15;
        obj_slider_maker.widget.clip.y = cell_y[1] + 35;
        obj_slider_maker.widget.clip.w = 140;
        obj_slider_maker.widget.clip.h = 30;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_slider_maker.obj_bar.value_lim = 100;
            obj_slider_maker.obj_bar.value_int = 0;
            obj_bar_res.grad = (idx == 0) ? 0 : 1;
            if (idx == 2) {
                obj_slider_maker.obj_bar.value_lim = 5;
                obj_slider_maker.obj_bar.value_int = 1;
            }
            obj_slider_maker.widget.clip.x = cell_x[idx];
            scui_widget_create(&obj_slider_maker, &obj_slider_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_slider_handle, 60.0f - idx * 10, false);
        }
        
        /* 第3行: 垂直滑条(值40/60/80) */
        obj_slider_maker.obj_bar.way = 1;
        obj_slider_maker.obj_bar.value_lim = 100;
        obj_slider_maker.obj_bar.value_int = 0;
        obj_bar_res.radius = 7;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_res.grad = (idx == 1) ? 1 : 0;
            obj_slider_maker.widget.clip.x = cell_x[idx] + 58;
            obj_slider_maker.widget.clip.y = cell_y[2];
            obj_slider_maker.widget.clip.w = 24;
            obj_slider_maker.widget.clip.h = 100;
            scui_widget_create(&obj_slider_maker, &obj_slider_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_slider_handle, 40.0f + idx * 20, false);
        }
        
        /* 底部: 渐变大滑条 */
        obj_slider_maker.obj_bar.way = 0;
        obj_slider_maker.obj_bar.value_lim = 100;
        obj_slider_maker.obj_bar.value_int = 0;
        obj_bar_res.radius = 15;
        obj_bar_res.grad = 1;
        obj_slider_maker.widget.clip.x = 83;
        obj_slider_maker.widget.clip.y = 375;
        obj_slider_maker.widget.clip.w = 300;
        obj_slider_maker.widget.clip.h = 30;
        scui_widget_create(&obj_slider_maker, &obj_slider_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
        scui_obj_bar_update_value(obj_slider_handle, 60.0f, false);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_5 控件事件响应回调(Test Switch)
 *@param event 事件
 */
void scui_test_ui_object_page_5_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_switch_maker_define(obj_switch_maker);
        scui_handle_t obj_switch_handle = SCUI_HANDLE_INVALID;
        obj_switch_maker.widget.parent = event->object;
        obj_switch_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        /* 统一渐变色调: bg 深蓝->深绿, fg 亮蓝->亮绿 */
        obj_bar_res.color[0].color_s.full = 0xFF000080;
        obj_bar_res.color[1].color_s.full = 0xFF0000FF;
        obj_bar_res.color[0].color_e.full = 0xFF008000;
        obj_bar_res.color[1].color_e.full = 0xFF00FF00;
        
        scui_coord_t cell_x[3] = {13, 163, 313};
        scui_coord_t cell_y[3] = {48, 158, 268};
        scui_coord_t cell_w[3] = {80, 100, 120};
        scui_coord_t cell_h[3] = {40, 50, 60};
        
        /* 第1行: 尺寸变化(小/中/大) */
        obj_switch_maker.obj_bar.value_lim = 100;
        obj_switch_maker.obj_bar.way = 0;
        obj_bar_res.grad = 0;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_res.radius = 10;
            obj_switch_maker.widget.clip.x = cell_x[idx] + (140 - cell_w[idx]) / 2;
            obj_switch_maker.widget.clip.y = cell_y[0] + (100 - cell_h[idx]) / 2;
            obj_switch_maker.widget.clip.w = cell_w[idx];
            obj_switch_maker.widget.clip.h = cell_h[idx];
            scui_widget_create(&obj_switch_maker, &obj_switch_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        }
        
        /* 第2行: 圆角/方角/渐变 */
        obj_bar_res.radius = 20;
        obj_switch_maker.widget.clip.x = cell_x[0] + 20;
        obj_switch_maker.widget.clip.y = cell_y[1] + 20;
        obj_switch_maker.widget.clip.w = 100;
        obj_switch_maker.widget.clip.h = 60;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.radius = -1;
        obj_switch_maker.widget.clip.x = cell_x[1] + 20;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.radius = 20;
        obj_bar_res.grad = 1;
        obj_switch_maker.widget.clip.x = cell_x[2] + 20;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        /* 第3行: 尺寸/渐变组合 */
        obj_bar_res.grad = 0;
        obj_bar_res.radius = 15;
        obj_switch_maker.widget.clip.x = cell_x[0] + 20;
        obj_switch_maker.widget.clip.y = cell_y[2] + 20;
        obj_switch_maker.widget.clip.w = 100;
        obj_switch_maker.widget.clip.h = 60;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.grad = 1;
        obj_switch_maker.widget.clip.x = cell_x[1] + 10;
        obj_switch_maker.widget.clip.y = cell_y[2] + 15;
        obj_switch_maker.widget.clip.w = 120;
        obj_switch_maker.widget.clip.h = 70;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.grad = 0;
        obj_bar_res.radius = 10;
        obj_switch_maker.widget.clip.x = cell_x[2] + 30;
        obj_switch_maker.widget.clip.y = cell_y[2] + 30;
        obj_switch_maker.widget.clip.w = 80;
        obj_switch_maker.widget.clip.h = 40;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_6 控件事件响应回调(Test Spinner)
 *@param event 事件
 */
void scui_test_ui_object_page_6_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_spinner_maker_define(obj_spinner_maker);
        scui_handle_t obj_spinner_handle = SCUI_HANDLE_INVALID;
        obj_spinner_maker.widget.parent = event->object;
        obj_spinner_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        scui_obj_arc_res_t obj_arc_res = {0};
        scui_coord_t arc_w = 90;
        scui_coord_t arc_h = 90;
        obj_arc_res.color[0].color_s.full = 0xFF000080;
        obj_arc_res.color[1].color_s.full = 0xFF0000FF;
        obj_arc_res.color[0].color_e.full = 0xFF008000;
        obj_arc_res.color[1].color_e.full = 0xFF00FF00;
        obj_arc_res.center.x = arc_w / 2;
        obj_arc_res.center.y = arc_h / 2;
        obj_arc_res.radius   = arc_w / 2;
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        
        /* 第1行: 慢速/快速/反向 */
        obj_spinner_maker.obj_arc.anti = 0;
        obj_spinner_maker.obj_arc.touch = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_res.time    = 3000;
        obj_spinner_maker.widget.clip.x = 33;
        obj_spinner_maker.widget.clip.y = 50;
        obj_spinner_maker.widget.clip.w = arc_w;
        obj_spinner_maker.widget.clip.h = arc_h;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.time    = 500;
        obj_spinner_maker.widget.clip.x = 173;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_spinner_maker.obj_arc.anti = 1;
        obj_arc_res.time    = 1500;
        obj_spinner_maker.widget.clip.x = 313;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        /* 第2行: 圆头/渐变/粗弧 */
        obj_spinner_maker.obj_arc.anti = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_res.time    = 1500;
        obj_spinner_maker.widget.clip.x = 33;
        obj_spinner_maker.widget.clip.y = 170;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_spinner_maker.widget.clip.x = 173;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.width   = 16;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_spinner_maker.widget.clip.x = 313;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        /* 第3行: 细弧/渐变圆头/默认 */
        obj_arc_res.width   = 4;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_spinner_maker.widget.clip.x = 33;
        obj_spinner_maker.widget.clip.y = 290;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.width   = 12;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_spinner_maker.widget.clip.x = 173;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_res.time    = 1500;
        obj_spinner_maker.widget.clip.x = 313;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_7 控件事件响应回调(Test Chart)
 *@param event 事件
 */
void scui_test_ui_object_page_7_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_chart_maker_define(obj_chart_maker);
        scui_handle_t obj_chart_handle = SCUI_HANDLE_INVALID;
        obj_chart_maker.widget.parent = event->object;
        obj_chart_maker.widget.event_cb = scui_test_ui_object_cht_event_proc;
        
        scui_coord_t vlist[100] = {0};
        scui_coord_t vlist_min[100] = {0};
        scui_coord_t vlist_max[100] = {0};
        for (uint32_t idx = 0; idx < 100; idx++) {
            vlist_min[idx] =  60 + (uint32_t)scui_rand(0xFF) % 40;
            vlist_max[idx] = 220 - (uint32_t)scui_rand(0xFF) % 40;
            vlist[idx] = 60 + (uint32_t)scui_rand(0xFF) % ((220 - 60));
        }
        
        scui_obj_chart_res_t obj_chart_res = {0};
        obj_chart_res.color.color.full = 0xFFFF0000;
        
        /* 第1行: 柱状/折线 */
        obj_chart_maker.type   = 0;
        obj_chart_maker.number = 19;
        obj_chart_maker.space  = 4;
        obj_chart_maker.value_min = 60;
        obj_chart_maker.value_max = 220;
        obj_chart_maker.area.x = 10;
        obj_chart_maker.area.y = 10;
        obj_chart_maker.widget.clip.x = 13;
        obj_chart_maker.widget.clip.y = 50;
        obj_chart_maker.widget.clip.w = 210;
        obj_chart_maker.widget.clip.h = 180;
        obj_chart_maker.area.w = obj_chart_maker.widget.clip.w - 10 * 2;
        obj_chart_maker.area.h = obj_chart_maker.widget.clip.h - 10 * 2;
        obj_chart_res.width = 6;
        obj_chart_res.round = true;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_rect_item;
        scui_obj_chart_style(obj_chart_handle, &obj_chart_res);
        scui_obj_chart_hist_data(obj_chart_handle, vlist_min, vlist_max);
        
        obj_chart_maker.type   = 1;
        obj_chart_maker.number = 30;
        obj_chart_maker.space  = 4;
        obj_chart_maker.widget.clip.x = 243;
        obj_chart_res.width = 2;
        obj_chart_res.grad = true;
        obj_chart_res.color.color.full = 0xFF2196F3;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_line_item;
        scui_obj_chart_style(obj_chart_handle, &obj_chart_res);
        scui_obj_chart_line_data(obj_chart_handle, vlist);
        
        /* 第2行: 大柱状/渐变折线 */
        obj_chart_maker.type   = 0;
        obj_chart_maker.number = 8;
        obj_chart_maker.space  = 8;
        obj_chart_maker.widget.clip.x = 13;
        obj_chart_maker.widget.clip.y = 250;
        obj_chart_maker.area.w = obj_chart_maker.widget.clip.w - 10 * 2;
        obj_chart_maker.area.h = obj_chart_maker.widget.clip.h - 10 * 2;
        obj_chart_res.width = 16;
        obj_chart_res.round = true;
        obj_chart_res.color.color.full = 0xFF00FF00;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_rect_item;
        scui_obj_chart_style(obj_chart_handle, &obj_chart_res);
        scui_obj_chart_hist_data(obj_chart_handle, vlist_min, vlist_max);
        
        obj_chart_maker.type   = 1;
        obj_chart_maker.number = 50;
        obj_chart_maker.space  = 2;
        obj_chart_maker.widget.clip.x = 243;
        obj_chart_res.width = 2;
        obj_chart_res.grad = true;
        obj_chart_res.color.color.full = 0xFFFF8000;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_line_item;
        scui_obj_chart_style(obj_chart_handle, &obj_chart_res);
        scui_obj_chart_line_data(obj_chart_handle, vlist);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_8 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_page_8_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_9 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_page_9_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_10 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_page_10_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
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

    default:
        break;
    }
}
