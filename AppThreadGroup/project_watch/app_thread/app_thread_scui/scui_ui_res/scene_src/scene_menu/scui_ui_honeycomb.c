/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t scroll_width;      // scroll控件属性.widget.clip.w
    scui_coord_t scroll_height;     // scroll控件属性.widget.clip.h
    
    scui_handle_t ofs_cur;
    
} * scui_ui_res_local = NULL;

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// 结算蜂窝布局的几个外部接口(布局使用图片原点坐标(左上角坐标))

// 图标间隔水平宽度
scui_coord_t scui_ui_honeycomb_dif_x(scui_handle_t ofs_cur)
{
    scui_handle_t image = scui_ui_honeycomb_list_image[0] + ofs_cur;
    // 图片宽度 + 左右俩个间隙(控件:scroll.space属性, 同步修改)
    return scui_image_w(image) + SCUI_UI_HONEYCOMB_SPACE;
}

// 图标间隔:垂直线宽度
scui_coord_t scui_ui_honeycomb_dif_y(scui_handle_t ofs_cur)
{
    // 在六边形布局中, 等边三角形的高度差是边长的 二分之根号三: 86 / 100
    return scui_ui_honeycomb_dif_x(ofs_cur) * 86 / 100;
}

// 中心图标水平偏移(原点(左上角))
scui_coord_t scui_ui_honeycomb_mid_x(scui_handle_t ofs_cur)
{
    scui_handle_t image = scui_ui_honeycomb_list_image[0] + ofs_cur;
    return (scui_ui_res_local->scroll_width - scui_image_w(image)) / 2;
}

// 中心图标垂直偏移(原点(左上角))
scui_coord_t scui_ui_honeycomb_mid_y(scui_handle_t ofs_cur)
{
    scui_handle_t image = scui_ui_honeycomb_list_image[0] + ofs_cur;
    return (scui_ui_res_local->scroll_height - scui_image_h(image)) / 2;
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
        
        scui_handle_t image   = scui_ui_honeycomb_list_image[index];
        scui_handle_t ofs_cur = scui_ui_res_local->ofs_cur;
        
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
        scui_event_dir_t hor_dir = scui_event_dir_none;
        scui_event_dir_t ver_dir = scui_event_dir_none;
        
        if (icon_center.x < edge_center.x)
            hor_dir = scui_event_dir_to_r;
        if (icon_center.x > edge_center.x)
            hor_dir = scui_event_dir_to_l;
        
        if (icon_center.y < edge_center.y)
            ver_dir = scui_event_dir_to_d;
        if (icon_center.y > edge_center.y)
            ver_dir = scui_event_dir_to_u;
        
        for (int64_t idx = ofs_cur; idx >= SCUI_UI_HONEYCOMB_OFS_MIN; idx--) {
            scui_coord_t image_clip_w = scui_image_w(image + idx);
            scui_coord_t image_clip_h = scui_image_h(image + idx);
            
            scui_area_t scale_clip = icon_clip;
            
            #if 1
            // 水平缩放
            if (hor_dir == scui_event_dir_none)
                scale_clip.x += (icon_clip.w - image_clip_w) / 2;
            if (hor_dir == scui_event_dir_to_l);
            if (hor_dir == scui_event_dir_to_r)
                scale_clip.x += (icon_clip.w - image_clip_w);
            
            // 垂直缩放
            if (ver_dir == scui_event_dir_none)
                scale_clip.y += (icon_clip.h - image_clip_h) / 2;
            if (ver_dir == scui_event_dir_to_u);
            if (ver_dir == scui_event_dir_to_d)
                scale_clip.y += (icon_clip.h - image_clip_h);
            #else
            // 图标居中对齐
            scale_clip.x += (icon_clip.w - image_clip_w) / 2;
            scale_clip.y += (icon_clip.h - image_clip_h) / 2;
            #endif
            
            scale_clip.w = image_clip_w;
            scale_clip.h = image_clip_h;
            
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
                scui_widget_draw_image(event->object, &scale_clip, image + idx, NULL, (scui_color_t){0});
                break;
            }
            #elif SCUI_UI_HONEYCOMB_EDGE_MODE == 1  // 方屏
            if (scui_area_inside(&edge_clip, &scale_clip)) {
                scui_widget_draw_image(event->object, &scale_clip, image + idx, NULL, (scui_color_t){0});
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
            scui_ui_res_local->ofs_cur       = SCUI_UI_HONEYCOMB_OFS_DEF;
            
            // 取一张图(随便, 反正所有图都一样)
            scui_handle_t icon = scui_ui_honeycomb_list_image[0];
            icon += scui_ui_res_local->ofs_cur;
            scui_coord_t icon_w = scui_image_w(icon);
            scui_coord_t icon_h = scui_image_h(icon);
            
            uintptr_t layout_size = SCUI_UI_HONEYCOMB_LIST_NUM * sizeof(scui_point_t);
            scui_point_t *list_layout = SCUI_MEM_ALLOC(scui_mem_type_mix, layout_size);
            scui_ui_honeycomb_list_layout(list_layout, SCUI_UI_HONEYCOMB_OFS_DEF);
            
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
                scui_custom_create(&custom_maker, &custom_handle, false);
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
        if (event->type == scui_event_enc_clockwise &&
            scui_ui_res_local->ofs_cur < SCUI_UI_HONEYCOMB_OFS_MAX) {
            scui_ui_res_local->ofs_cur++;
            relayout = true;
        }
        if (event->type == scui_event_enc_clockwise_anti &&
            scui_ui_res_local->ofs_cur > SCUI_UI_HONEYCOMB_OFS_MIN) {
            scui_ui_res_local->ofs_cur--;
            relayout = true;
        }
        
        if (relayout) {
            // 取一张图(随便, 反正所有图都一样)
            scui_handle_t icon = scui_ui_honeycomb_list_image[0];
            icon += scui_ui_res_local->ofs_cur;
            scui_coord_t icon_w = scui_image_w(icon);
            scui_coord_t icon_h = scui_image_h(icon);
            
            uintptr_t layout_size = SCUI_UI_HONEYCOMB_LIST_NUM * sizeof(scui_point_t);
            scui_point_t *list_layout = SCUI_MEM_ALLOC(scui_mem_type_mix, layout_size);
            scui_ui_honeycomb_list_layout(list_layout, scui_ui_res_local->ofs_cur);
            
            scui_handle_t child_num = scui_widget_child_num(SCUI_UI_SCENE_HONEYCOMB_SCROLL);
            for (scui_handle_t idx = 0; idx < child_num; idx++) {
                scui_handle_t child = scui_widget_child_by_index(SCUI_UI_SCENE_HONEYCOMB_SCROLL, idx);
                if (child != SCUI_HANDLE_INVALID) {
                    scui_widget_adjust_size(child, icon_w, icon_h);
                    scui_widget_move_pos(child, &list_layout[idx]);
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
