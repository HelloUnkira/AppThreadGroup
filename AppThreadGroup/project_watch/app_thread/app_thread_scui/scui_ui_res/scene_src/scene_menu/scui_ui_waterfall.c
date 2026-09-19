/*实现目标:
 *    主题:瀑布
 *    三列瀑布(左/中/右), 滚动时图标沿球面弧运动
 *    距中心垂直距离映射真实缩放(中心大, 上下小) + 弧面横向偏移 + 透明度渐变
 *    无换挡组图
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_ui_bar_arc_t bar_arc;
    scui_coord_t  line_space;    /* 图标垂直间隙 */
    scui_coord3_t scale_max;     /* 中心(选中行)缩放 */
    scui_coord3_t scale_min;     /* 边缘缩放 */
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_waterfall_icon_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_ptr_click:
    case scui_event_draw_graph: {
        
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(event->object) - 1;
        
        uint8_t type = 1;   //l:0;m:1;r:2;
        
        if (index % 3 == 0) type = 0;
        if (index % 3 == 1) type = 1;
        if (index % 3 == 2) type = 2;
        
        if (scui_ui_scene_list_num % 3 == 1 &&
            index == scui_ui_scene_list_num - 1) type = 1;
        if (scui_ui_scene_list_num % 3 == 2 &&
            index == scui_ui_scene_list_num - 2) type = 0;
        if (scui_ui_scene_list_num % 3 == 2 &&
            index == scui_ui_scene_list_num - 1) type = 2;
        
        scui_handle_t image = scui_ui_scene_list[index].image;
        scui_coord_t  image_w = scui_image_w(image);
        scui_coord_t  image_h = scui_image_h(image);
        
        scui_area_t  scroll_c  = scui_widget_area(SCUI_UI_SCENE_WATERFALL_SCROLL);
        scui_coord_t scroll_cx = scroll_c.x + scroll_c.w / 2;
        scui_coord_t scroll_cy = scroll_c.y + scroll_c.h / 2;
        
        scui_area_t  icon_c        = scui_widget_clip(event->object);
        scui_area_t  icon_c_origin = icon_c;
        scui_coord_t icon_cx = icon_c.x + icon_c.w / 2;
        scui_coord_t icon_cy = icon_c.y + icon_c.h / 2;
        scui_coord_t dist_cy = scui_dist(scroll_cy, icon_cy);
        scui_coord_t dist_ch = scroll_c.h / 2;
        
        /* 真实缩放: 距中心垂直距离映射(中心scale_max, 边缘scale_min) */
        scui_coord3_t scale_s = scui_ui_res_local->scale_max;
        if (dist_cy <= dist_ch)
            scale_s = scui_ui_res_local->scale_max +
                (scui_ui_res_local->scale_min - scui_ui_res_local->scale_max) * dist_cy / (scui_coord3_t)dist_ch;
        scui_coord_t icon_w = image_w * scale_s;
        scui_coord_t icon_h = image_h * scale_s;
        
        /* 球面弧: 横向偏移(滚动时图标沿弧面运动) */
        scui_multi_t rad_rr = scroll_c.w / 2 - image_h / 2;
        scui_multi_t dist_y = scui_min(rad_rr, scui_dist(icon_cy, scroll_cy));
        
        scui_multi_t cos_a2 = (1024 * 1024) - (1024 * dist_y / rad_rr) * (1024 * dist_y / rad_rr);
        scui_multi_t cos_ia = 0;
        scui_multi_t cos_fa = 0;
        scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
        scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
        dist_x = scui_min(dist_x, icon_c.w - icon_w);
        
        scui_alpha_t alpha = scui_map(dist_y, 0, rad_rr, scui_alpha_pct100, scui_alpha_pct0);
        scui_widget_alpha_set(event->object, alpha, true);
        
        if (type == 0)
            icon_c.x += dist_x;
        if (type == 1)
            icon_c.x += (icon_c.w - icon_w) / 2;
        if (type == 2)
            icon_c.x += (icon_c.w - icon_w) - dist_x;
        
        icon_c.y += (icon_c.h - icon_h) / 2;
        icon_c.h  = icon_h;
        icon_c.w  = icon_w;
        
        if (event->type == scui_event_draw_graph) {
            /* 绘制目标:从滚动空间坐标转换为控件局部坐标 */
            scui_area_t draw_clip = {
                .x = icon_c.x - icon_c_origin.x,
                .y = icon_c.y - icon_c_origin.y,
                .w = icon_c.w,
                .h = icon_c.h,
            };
            scui_point_t scale = {
                .x = icon_w * SCUI_SCALE_COF / image_w,
                .y = icon_h * SCUI_SCALE_COF / image_h,
            };
            scui_area_t  wf_img  = scui_image_area(image);
            scui_point_t anchor = scui_area_center(&draw_clip);
            scui_point_t center = scui_area_center(&wf_img);
            scui_widget_draw_image_scale(event->object, &draw_clip, image, NULL, SCUI_COLOR_UNUSED, anchor, center, scale);
        }
        
        if (event->type == scui_event_ptr_click) {
            
            if (alpha <= scui_alpha_pct20)
                break;
            
            if (scui_area_point(&icon_c, &event->ptr_c)) {
                SCUI_LOG_WARN("click idx:%d", index);
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_waterfall_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_waterfall_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_coord_t scroll_w = scui_widget_area(event->object).w;
        scui_coord_t scroll_h = scui_widget_area(event->object).w;
        
        // 取一张图(随便, 反正所有图都一样)
        scui_handle_t icon = scui_ui_scene_list[0].image;
        scui_coord_t icon_w = scui_image_w(icon);
        scui_coord_t icon_h = scui_image_h(icon);
        
        scui_area_t clip_l = {.y = icon_h / 2,.w = (scroll_w - icon_w) / 2,.h = icon_h,};
        scui_area_t clip_m = {.w = icon_w,.h = icon_h,};
        scui_area_t clip_r = {.y = icon_h / 2,.w = (scroll_w - icon_w) / 2,.h = icon_h,};
        
        clip_l.x = 0;
        clip_m.x = clip_l.w;
        clip_r.x = clip_m.x + clip_m.w;
        
        clip_l.y += (scroll_h - icon_h) / 2;
        clip_m.y += (scroll_h - icon_h) / 2;
        clip_r.y += (scroll_h - icon_h) / 2;
        
        scui_custom_maker_define(custom_maker);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        
        custom_maker.widget.parent      = event->object;
        
        /* 上半部分空白 */
        custom_maker.widget.style.indev_ptr = false;
        custom_maker.widget.event_cb        = NULL;
        custom_maker.widget.clip.x = 0;
        custom_maker.widget.clip.y = 0;
        custom_maker.widget.clip.w = scroll_w;
        custom_maker.widget.clip.h = (scroll_h - icon_h) / 2;
        scui_widget_create(&custom_maker, &custom_handle);
        
        custom_maker.widget.style.indev_ptr = true;
        custom_maker.widget.event_cb        = scui_ui_scene_waterfall_icon_event_proc;
        
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            // 余下一个,填中间
            if (scui_ui_scene_list_num % 3 == 1 &&
                idx == scui_ui_scene_list_num - 1) {
                custom_maker.widget.clip = clip_m;
                scui_widget_create(&custom_maker, &custom_handle);
                clip_m.y += clip_m.h + scui_ui_res_local->line_space;
                continue;
            }
            // 余下俩个,填俩边
            if (scui_ui_scene_list_num % 3 == 2 &&
                idx == scui_ui_scene_list_num - 2) {
                custom_maker.widget.clip = clip_l;
                scui_widget_create(&custom_maker, &custom_handle);
                clip_l.y += clip_l.h + scui_ui_res_local->line_space;
                continue;
            }
            if (scui_ui_scene_list_num % 3 == 2 &&
                idx == scui_ui_scene_list_num - 1) {
                custom_maker.widget.clip = clip_r;
                scui_widget_create(&custom_maker, &custom_handle);
                clip_r.y += clip_r.h + scui_ui_res_local->line_space;
                continue;
            }
            // 按顺序填充三列
            if (idx % 3 == 0) {
                custom_maker.widget.clip = clip_l;
                scui_widget_create(&custom_maker, &custom_handle);
                clip_l.y += clip_l.h + scui_ui_res_local->line_space;
            }
            if (idx % 3 == 1) {
                custom_maker.widget.clip = clip_m;
                scui_widget_create(&custom_maker, &custom_handle);
                clip_m.y += clip_m.h + scui_ui_res_local->line_space;
            }
            if (idx % 3 == 2) {
                custom_maker.widget.clip = clip_r;
                scui_widget_create(&custom_maker, &custom_handle);
                clip_r.y += clip_r.h + scui_ui_res_local->line_space;
            }
        }
        
        clip_l.y -= scui_ui_res_local->line_space;
        clip_m.y -= scui_ui_res_local->line_space;
        clip_r.y -= scui_ui_res_local->line_space;
        
        /* 下半部分空白 */
        custom_maker.widget.style.indev_ptr = false;
        custom_maker.widget.event_cb        = NULL;
        custom_maker.widget.clip.x = 0;
        custom_maker.widget.clip.y = scui_max(clip_m.y, scui_max(clip_l.y, clip_r.y));
        custom_maker.widget.clip.w = scroll_w;
        custom_maker.widget.clip.h = (scroll_h - icon_h) / 2;
        scui_widget_create(&custom_maker, &custom_handle);
        
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_WATERFALL_BAR_ARC;
        break;
    }
    case scui_event_scroll_layout:
    case scui_event_scroll_start:
    case scui_event_scroll_keep:
    case scui_event_scroll_over: {
        
        scui_coord_t scroll_pct = 0;
        scui_scroll_percent_get(event->object, &scroll_pct);
        scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
        scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_waterfall_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        scui_ui_res_local->line_space = 8;
        scui_ui_res_local->scale_max  = 1.0f;
        scui_ui_res_local->scale_min  = 0.4f;
        
        scui_ui_scene_list_cfg(scui_ui_scene_list_type_waterfall);
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
