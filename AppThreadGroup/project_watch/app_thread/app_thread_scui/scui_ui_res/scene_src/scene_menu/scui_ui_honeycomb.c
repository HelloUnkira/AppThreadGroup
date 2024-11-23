/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t scroll_width;      // scroll控件属性.widget.clip.w
    scui_coord_t scroll_height;     // scroll控件属性.widget.clip.h
    
    SCUI_UI_HONEYCOMB_T scale_ofs;
    
} * scui_ui_res_local = NULL;

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// 计算蜂窝布局的几个外部接口(布局使用图片原点坐标(左上角坐标))

// 图标间隔水平宽度
scui_coord_t scui_ui_honeycomb_dif_x(SCUI_UI_HONEYCOMB_T scale_ofs)
{
    // 图片宽度 + 一个间隙(控件:scroll.space属性, 同步修改)
    #if SCUI_UI_HONEYCOMB_SCALE_MODE
    return scale_ofs + SCUI_UI_HONEYCOMB_SPACE;
    #else
    scui_handle_t image = scui_ui_scene_list_image[0] + scale_ofs;
    return scui_image_w(image) + SCUI_UI_HONEYCOMB_SPACE;
    #endif
}

// 图标间隔:垂直线宽度
scui_coord_t scui_ui_honeycomb_dif_y(SCUI_UI_HONEYCOMB_T scale_ofs)
{
    // 在六边形布局中, 等边三角形的高度差是边长的 二分之根号三: 86 / 100
    return scui_ui_honeycomb_dif_x(scale_ofs) * 86 / 100;
}

// 中心图标水平偏移(原点(左上角))
scui_coord_t scui_ui_honeycomb_mid_x(SCUI_UI_HONEYCOMB_T scale_ofs)
{
    #if SCUI_UI_HONEYCOMB_SCALE_MODE
    return (scui_ui_res_local->scroll_width - scale_ofs) / 2;
    #else
    scui_handle_t image = scui_ui_scene_list_image[0] + scale_ofs;
    return (scui_ui_res_local->scroll_width - scui_image_w(image)) / 2;
    #endif
}

// 中心图标垂直偏移(原点(左上角))
scui_coord_t scui_ui_honeycomb_mid_y(SCUI_UI_HONEYCOMB_T scale_ofs)
{
    #if SCUI_UI_HONEYCOMB_SCALE_MODE
    return (scui_ui_res_local->scroll_height - scale_ofs) / 2;
    #else
    scui_handle_t image = scui_ui_scene_list_image[0] + scale_ofs;
    return (scui_ui_res_local->scroll_height - scui_image_h(image)) / 2;
    #endif
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_honeycomb_icon_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(parent, event->object);
        SCUI_LOG_INFO("list_idx:%d", index);
        
        scui_handle_t image   = scui_ui_scene_list_image[index];
        scui_handle_t scale_ofs = scui_ui_res_local->scale_ofs;
        
        scui_area_t edge_clip = scui_widget_clip(parent);
        scui_area_t icon_clip = scui_widget_clip(event->object);
        
        scui_area_t edge_center = {
            .x = edge_clip.x + edge_clip.w / 2,
            .y = edge_clip.y + edge_clip.h / 2,
        };
        scui_area_t icon_center = {
            .x = icon_clip.x + icon_clip.w / 2,
            .y = icon_clip.y + icon_clip.h / 2,
        };
        
        // 获得icon的四个中点以确定水平与垂直的缩放方向
        scui_opt_dir_t hor_dir = scui_opt_dir_none;
        scui_opt_dir_t ver_dir = scui_opt_dir_none;
        
        if (icon_center.x < edge_center.x)
            hor_dir = scui_opt_dir_to_r;
        if (icon_center.x > edge_center.x)
            hor_dir = scui_opt_dir_to_l;
        
        if (icon_center.y < edge_center.y)
            ver_dir = scui_opt_dir_to_d;
        if (icon_center.y > edge_center.y)
            ver_dir = scui_opt_dir_to_u;
        
        #if 0
        // 距离中心偏移做球面映射
        scui_multi_t edge_rw = (edge_clip.w - icon_clip.w) / 2;
        scui_multi_t edge_rh = (edge_clip.h - icon_clip.h) / 2;
        scui_multi_t dist_cx = scui_dist(icon_center.x, edge_center.x) + icon_clip.w / 2;
        scui_multi_t dist_cy = scui_dist(icon_center.y, edge_center.y) + icon_clip.h / 2;
        scui_multi_t dist_dm = edge_rw * edge_rw + edge_rh * edge_rh;
        scui_multi_t dist_cm = dist_cx * dist_cx + dist_cy * dist_cy;
        dist_cm = scui_min(dist_cm, dist_dm);
        
        #if 0
        scui_multi_t sin_a2 = (1024 * 1024) - (1024 * dist_cm / dist_dm) * (1024 * dist_cm / dist_dm);
        scui_multi_t sin_ia = 0;
        scui_multi_t sin_fa = 0;
        scui_sqrt(sin_a2, &sin_ia, &sin_fa, 0x8000);
        scui_multi_t dist_x = (1024 - sin_ia) * (dist_dm) / 1024;
        scale_ofs = scui_map(dist_x, 0, dist_dm, scale_ofs, SCUI_UI_HONEYCOMB_SCALE_MIN);
        #else
        scale_ofs = scui_map(dist_dm - dist_cm, 0, dist_dm, SCUI_UI_HONEYCOMB_SCALE_MIN, scale_ofs);
        #endif
        #endif
        
        #if SCUI_UI_HONEYCOMB_SCALE_MODE
        for (int64_t idx = scale_ofs; idx >= SCUI_UI_HONEYCOMB_SCALE_MIN; idx -= SCUI_UI_HONEYCOMB_SCALE_STEP) {
            scui_coord_t image_clip_w = idx;
            scui_coord_t image_clip_h = idx;
        #else
        for (int64_t idx = scale_ofs; idx >= SCUI_UI_HONEYCOMB_OFS_MIN; idx--) {
            scui_coord_t image_clip_w = scui_image_w(image + idx);
            scui_coord_t image_clip_h = scui_image_h(image + idx);
        #endif
            
            scui_area_t scale_clip = {
                .x = icon_clip.x,
                .y = icon_clip.y,
                .w = image_clip_w,
                .h = image_clip_h,
            };
            
            #if 0
            // 图标居中对齐
            scale_clip.x += (icon_clip.w - image_clip_w) / 2;
            scale_clip.y += (icon_clip.h - image_clip_h) / 2;
            #else
            // 水平缩放
            if (hor_dir == scui_opt_dir_none)
                scale_clip.x += (icon_clip.w - image_clip_w) / 2;
            if (hor_dir == scui_opt_dir_to_l)
                scale_clip.x += (icon_clip.w - image_clip_w) * 1 / 4;
            if (hor_dir == scui_opt_dir_to_r)
                scale_clip.x += (icon_clip.w - image_clip_w) * 3 / 4;
            
            // 垂直缩放
            if (ver_dir == scui_opt_dir_none)
                scale_clip.y += (icon_clip.h - image_clip_h) / 2;
            if (ver_dir == scui_opt_dir_to_u)
                scale_clip.y += (icon_clip.h - image_clip_h) * 1 / 4;
            if (ver_dir == scui_opt_dir_to_d)
                scale_clip.y += (icon_clip.h - image_clip_h) * 3 / 4;
            #endif
            
            
            
            #if 0
            #elif SCUI_UI_HONEYCOMB_EDGE_MODE == 0  // 圆屏
            scui_area_t scale_center = {
                .x = scale_clip.x + scale_clip.w / 2,
                .y = scale_clip.y + scale_clip.h / 2,
            };
            // 检查缩放的目标是否在圆屏的区域内
            scui_multi_t dist_cx = scui_dist(scale_center.x, edge_center.x) + image_clip_w / 2;
            scui_multi_t dist_cy = scui_dist(scale_center.y, edge_center.y) + image_clip_h / 2;
            dist_cx *= dist_cx;
            dist_cy *= dist_cy;
            
            if (dist_cx + dist_cy < edge_center.x * edge_center.y) {
                #if SCUI_UI_HONEYCOMB_SCALE_MODE
                scui_point_t scale = {
                    .x = scale_clip.w * SCUI_SCALE_COF / scui_image_w(image),
                    .y = scale_clip.h * SCUI_SCALE_COF / scui_image_h(image),
                };
                scui_widget_draw_image_scale(event->object, &scale_clip, image, NULL, scale, scui_opt_pos_c);
                #else
                scui_widget_draw_image(event->object, &scale_clip, image + idx, NULL, (scui_color_t){0});
                #endif
                break;
            }
            #elif SCUI_UI_HONEYCOMB_EDGE_MODE == 1  // 方屏
            if (scui_area_inside(&edge_clip, &scale_clip)) {
                #if SCUI_UI_HONEYCOMB_SCALE_MODE
                scui_point_t scale = {
                    .x = scale_clip.w * SCUI_SCALE_COF / scui_image_w(image),
                    .y = scale_clip.h * SCUI_SCALE_COF / scui_image_h(image),
                };
                scui_widget_draw_image_scale(event->object, &scale_clip, image, NULL, scale, scui_opt_pos_c);
                #else
                scui_widget_draw_image(event->object, &scale_clip, image + idx, NULL, (scui_color_t){0});
                #endif
                break;
            }
            #else
            #error "unknown edge mode"
            #endif
        }
        
        
        
        break;
    }
    case scui_event_ptr_click: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_widget_event_mask_over(event);
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(parent, event->object);
        SCUI_LOG_WARN("click idx:%d", index);
        
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
void scui_ui_scene_honeycomb_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        break;
    }
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
            
        }
        
        if (scui_widget_event_check_prepare(event)) {
            
            scui_ui_res_local->scroll_width  = scui_widget_clip(SCUI_UI_SCENE_HONEYCOMB_SCROLL).w;
            scui_ui_res_local->scroll_height = scui_widget_clip(SCUI_UI_SCENE_HONEYCOMB_SCROLL).h;
            
            #if SCUI_UI_HONEYCOMB_SCALE_MODE
            scui_handle_t icon = scui_ui_scene_list_image[0];
            SCUI_ASSERT(scui_image_w(icon) == scui_image_h(icon));
            scui_ui_res_local->scale_ofs = SCUI_UI_HONEYCOMB_SCALE_DEF;
            scui_coord_t icon_w = scui_ui_res_local->scale_ofs;
            scui_coord_t icon_h = scui_ui_res_local->scale_ofs;
            #else
            scui_ui_res_local->scale_ofs = SCUI_UI_HONEYCOMB_OFS_DEF;
            scui_handle_t icon = scui_ui_scene_list_image[0];
            icon += scui_ui_res_local->scale_ofs;
            scui_coord_t icon_w = scui_image_w(icon);
            scui_coord_t icon_h = scui_image_h(icon);
            #endif
            
            scui_point_t edge = {.x = icon_w, .y = icon_h,};
            scui_scroll_edge(SCUI_UI_SCENE_HONEYCOMB_SCROLL, &edge);
            
            uintptr_t layout_size = SCUI_UI_HONEYCOMB_LIST_NUM * sizeof(scui_point_t);
            scui_point_t *list_layout = SCUI_MEM_ALLOC(scui_mem_type_mix, layout_size);
            scui_ui_honeycomb_list_layout(list_layout, scui_ui_res_local->scale_ofs);
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle         = SCUI_HANDLE_INVALID;
            custom_maker.widget.type            = scui_widget_type_custom;
            custom_maker.widget.style.trans     = true;
            custom_maker.widget.style.indev_ptr = true;
            custom_maker.widget.clip.w          = icon_w;
            custom_maker.widget.clip.h          = icon_h;
            custom_maker.widget.parent          = SCUI_UI_SCENE_HONEYCOMB_SCROLL;
            custom_maker.widget.event_cb        = scui_ui_scene_honeycomb_icon_event_proc;
            
            for (uint8_t idx = 0; idx < SCUI_UI_HONEYCOMB_LIST_NUM; idx++) {
                custom_maker.widget.clip.x = list_layout[idx].x;
                custom_maker.widget.clip.y = list_layout[idx].y;
                scui_widget_create(&custom_maker, &custom_handle, false);
            }
            
            SCUI_MEM_FREE(list_layout);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        if (scui_widget_event_check_finish(event)) {
            
        }
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_enc_clockwise:
    case scui_event_enc_clockwise_anti: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        bool relayout = false;
        #if SCUI_UI_HONEYCOMB_SCALE_MODE
        if (event->type == scui_event_enc_clockwise &&
            scui_ui_res_local->scale_ofs <  SCUI_UI_HONEYCOMB_SCALE_MAX) {
            scui_ui_res_local->scale_ofs += SCUI_UI_HONEYCOMB_SCALE_SPAN;
            relayout = true;
        }
        if (event->type == scui_event_enc_clockwise_anti &&
            scui_ui_res_local->scale_ofs >  SCUI_UI_HONEYCOMB_SCALE_MIN) {
            scui_ui_res_local->scale_ofs -= SCUI_UI_HONEYCOMB_SCALE_SPAN;
            relayout = true;
        }
        #else
        if (event->type == scui_event_enc_clockwise &&
            scui_ui_res_local->scale_ofs < SCUI_UI_HONEYCOMB_OFS_MAX) {
            scui_ui_res_local->scale_ofs++;
            relayout = true;
        }
        if (event->type == scui_event_enc_clockwise_anti &&
            scui_ui_res_local->scale_ofs > SCUI_UI_HONEYCOMB_OFS_MIN) {
            scui_ui_res_local->scale_ofs--;
            relayout = true;
        }
        #endif
        
        if (relayout) {
            #if SCUI_UI_HONEYCOMB_SCALE_MODE
            scui_coord_t icon_w = scui_ui_res_local->scale_ofs;
            scui_coord_t icon_h = scui_ui_res_local->scale_ofs;
            #else
            scui_handle_t icon = scui_ui_scene_list_image[0];
            icon += scui_ui_res_local->scale_ofs;
            scui_coord_t icon_w = scui_image_w(icon);
            scui_coord_t icon_h = scui_image_h(icon);
            #endif
            
            scui_point_t edge = {.x = icon_w, .y = icon_h,};
            scui_scroll_edge(SCUI_UI_SCENE_HONEYCOMB_SCROLL, &edge);
            
            uintptr_t layout_size = SCUI_UI_HONEYCOMB_LIST_NUM * sizeof(scui_point_t);
            scui_point_t *list_layout = SCUI_MEM_ALLOC(scui_mem_type_mix, layout_size);
            scui_ui_honeycomb_list_layout(list_layout, scui_ui_res_local->scale_ofs);
            
            scui_handle_t child_num = scui_widget_child_num(SCUI_UI_SCENE_HONEYCOMB_SCROLL);
            for (scui_handle_t idx = 0; idx < child_num; idx++) {
                scui_handle_t child = scui_widget_child_by_index(SCUI_UI_SCENE_HONEYCOMB_SCROLL, idx);
                if (child != SCUI_HANDLE_INVALID) {
                    scui_widget_adjust_size(child, icon_w, icon_h);
                    scui_widget_move_pos(child, &list_layout[idx], true);
                }
            }
            scui_scroll_layout(SCUI_UI_SCENE_HONEYCOMB_SCROLL);
            scui_widget_draw(SCUI_UI_SCENE_HONEYCOMB, NULL, false);
            
            SCUI_MEM_FREE(list_layout);
        }
        break;
    }
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
