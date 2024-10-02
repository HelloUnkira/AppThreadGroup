/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_key_click:
        scui_widget_draw(event->object, NULL, false);
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_coord_t del_x = clip.w - 10;
        scui_coord_t del_y = clip.h - 10;
        scui_point_t pos_s = {0};
        scui_point_t pos_e = {0};
        scui_area_m_to_s(&clip);
        
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        scui_draw_graph_dsc_t draw_graph = {
            .type           = scui_draw_graph_type_line,
            .src_color      = color_mix,
            .line.src_width = 2,
        };
        
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = clip.y2 - 10;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y2 - 10;
        pos_e.x = clip.x2 - 10;
        pos_e.y = clip.y1 + 5;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = pos_s.x;
        pos_e.y = clip.y2 - 10;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = pos_s.y;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = pos_s.x + del_x / 2;
        pos_e.y = clip.y2 - 10;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        pos_s.x = clip.x2 - 10;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x1 + 5 + del_x / 2;
        pos_e.y = clip.y2 - 10;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y2 - 10;
        pos_e.x = clip.x2 - 10;
        pos_e.y = clip.y1 + 5 + del_y / 2;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        pos_s.x = clip.x1 + 5;
        pos_s.y = clip.y1 + 5;
        pos_e.x = clip.x2 - 10;
        pos_e.y = clip.y1 + 5 + del_y / 2;
        draw_graph.line.src_pos_1 = pos_s;
        draw_graph.line.src_pos_2 = pos_e;
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        scui_draw_graph_dsc_t draw_graph = {
            .type                = scui_draw_graph_type_circle,
            .src_color           = color_mix,
            .circle.src_width    = 40,
            .circle.src_radius   = 40,
            .circle.src_center.x = clip.x + clip.w / 2,
            .circle.src_center.y = clip.y + clip.h / 2,
        };
        
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        scui_draw_graph_dsc_t draw_graph = {
            .type                = scui_draw_graph_type_circle,
            .src_color           = color_mix,
            .circle.src_width    = 2,
            .circle.src_radius   = 40,
            .circle.src_center.x = clip.x + clip.w / 2,
            .circle.src_center.y = clip.y + clip.h / 2,
        };
        
        scui_widget_draw_graph(event->object, NULL, &draw_graph);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_4_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_color_t color_mix = {
            .color.full = 0xFF00FF00,
        };
        scui_draw_graph_dsc_t draw_graph = {
            .type                = scui_draw_graph_type_circle,
            .src_color           = color_mix,
            .circle.src_width    = 2,
            .circle.src_radius   = 20,
            .circle.src_center.x = clip.x + clip.w / 2,
            .circle.src_center.y = clip.y + clip.h / 2,
        };
        scui_area_t widget_clip = clip;
        
        widget_clip = clip;
        widget_clip.w /= 4;
        widget_clip.h /= 4;
        draw_graph.circle.src_center.x = clip.x + clip.w / 4;
        draw_graph.circle.src_center.y = clip.y + clip.h / 4;
        scui_widget_draw_graph(event->object, &widget_clip, &draw_graph);
        
        widget_clip = clip;
        widget_clip.w /= 4;
        widget_clip.h /= 4;
        widget_clip.x += clip.w * 3 / 4;
        draw_graph.circle.src_center.x = clip.x + clip.w * 3 / 4;
        draw_graph.circle.src_center.y = clip.y + clip.h / 4;
        scui_widget_draw_graph(event->object, &widget_clip, &draw_graph);
        
        widget_clip = clip;
        widget_clip.w /= 4;
        widget_clip.h /= 4;
        widget_clip.y += clip.h * 3 / 4;
        draw_graph.circle.src_center.x = clip.x + clip.w / 4;
        draw_graph.circle.src_center.y = clip.y + clip.h * 3 / 4;
        scui_widget_draw_graph(event->object, &widget_clip, &draw_graph);
        
        widget_clip = clip;
        widget_clip.w /= 4;
        widget_clip.h /= 4;
        widget_clip.x += clip.w * 3 / 4;
        widget_clip.y += clip.h * 3 / 4;
        draw_graph.circle.src_center.x = clip.x + clip.w * 3 / 4;
        draw_graph.circle.src_center.y = clip.y + clip.h * 3 / 4;
        scui_widget_draw_graph(event->object, &widget_clip, &draw_graph);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_5_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_6_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_7_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_8_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_6_9_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t  clip = {0};
        scui_color_t color_black = {0};
        clip = scui_widget_clip(event->object);
        clip.x += 10;
        clip.y += 10;
        clip.w -= 10 * 2;
        clip.h -= 10 * 2;
        scui_widget_draw_color(event->object, &clip, color_black);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
