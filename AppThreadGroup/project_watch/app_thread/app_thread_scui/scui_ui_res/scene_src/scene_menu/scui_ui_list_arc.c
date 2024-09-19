/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_ui_bar_arc_t bar_arc;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_arc_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_arc_scroll_notify_event(scui_event_t *event)
{
    SCUI_LOG_INFO("widget %d, type %d", event->object, event->type);
    
    scui_handle_t scroll    = event->object;
    scui_area_t   scroll_c  = scui_widget_clip(scroll);
    scui_coord_t  scroll_cx = scroll_c.x + scroll_c.w / 2;
    scui_coord_t  scroll_cy = scroll_c.y + scroll_c.h / 2;
    
    for (uint8_t idx = 0; idx < scui_widget_child_num(scroll); idx++) {
        
        scui_handle_t child = scui_widget_child_by_index(scroll, idx);
        if (child == SCUI_HANDLE_INVALID ||
            scui_widget_child_num(child) == 0)
            continue;
        
        scui_handle_t group = scui_widget_child_by_index(child, 0);
        if (group == SCUI_HANDLE_INVALID)
            continue;
        
        scui_area_t   group_c  = scui_widget_clip(group);
        scui_coord_t  group_cx = group_c.x + group_c.w / 2;
        scui_coord_t  group_cy = group_c.y + group_c.h / 2;
        
        if ((group_cy < scroll_cy && scui_dist(group_c.y + group_c.h, scroll_cy) > scroll_cy) ||
            (group_cy > scroll_cy && scui_dist(group_c.y,             scroll_cy) > scroll_cy)) {
             scui_point_t point = {.x = scroll_cx,.y = group_c.y,};
             scui_widget_move_pos(group, &point);
             scui_widget_alpha_set(group, scui_alpha_trans, true);
             continue;
        }
        
        // 三角函数:
        // sin_a = dist_y / rad_rr;
        // cos_a = 1 - sin_a * sin_a
        // dis_x = rad_rr - rad_rr * cos_a
        scui_multi_t rad_rr = scroll_c.h / 2 - group_c.h / 2;
        scui_multi_t dist_y = scui_min(rad_rr, scui_dist(group_cy, scroll_cy));
        
        scui_multi_t cos_a2 = (1024 * 1024) - (1024 * dist_y / rad_rr) * (1024 * dist_y / rad_rr);
        scui_multi_t cos_ia = 0;
        scui_multi_t cos_fa = 0;
        scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
        scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
        SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
        
        scui_point_t point = {.x = dist_x,.y = group_c.y,};
        scui_alpha_t alpha = scui_map(dist_y, 0, rad_rr, scui_alpha_pct100, scui_alpha_pct0);
        
        scui_widget_move_pos(group, &point);
        scui_widget_alpha_set(group, alpha, true);
    }
    
    scui_widget_draw(event->object, NULL, false);
    
    
    
    scui_coord_t scroll_pct = 0;
    scui_scroll_auto_percent_get(event->object, &scroll_pct);
    SCUI_LOG_INFO("pct:%d", scroll_pct);
    scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
    scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_arc_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_click: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_alpha_t alpha = scui_widget_alpha_get(event->object);
        if (alpha <= scui_alpha_pct20)
            break;
        
        scui_widget_event_mask_over(event);
        scui_handle_t parent   = scui_widget_parent(event->object);
        scui_handle_t ancestor = scui_widget_parent(parent);
        scui_handle_t index    = scui_widget_child_to_index(ancestor, parent) - 1;
        scui_handle_t custom   = event->object;
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
void scui_ui_scene_list_arc_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        break;
    }
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        // 这里画个圈,校验测试使用
        scui_widget_image_set(SCUI_UI_SCENE_LIST_ARC_SCROLL,
        scui_image_prj_image_src_standby_watch_D10606001_bg_01_2bmp);
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        if (scui_widget_event_check_prepare(event)) {
            
            scui_custom_maker_t item_maker = {0};
            scui_handle_t item_handle     = SCUI_HANDLE_INVALID;
            item_maker.widget.type        = scui_widget_type_custom;
            item_maker.widget.style.trans = true;
            item_maker.widget.clip.w      = SCUI_DRV_HOR_RES;
            item_maker.widget.parent      = SCUI_UI_SCENE_LIST_ARC_SCROLL;
            item_maker.widget.child_num   = 1;
            
            item_maker.widget.clip.h = SCUI_DRV_VER_RES / 2 - 3 - 62 / 2;
            scui_custom_create(&item_maker, &item_handle, false);
            
            item_maker.widget.clip.h = 62;
            SCUI_ASSERT(scui_ui_scene_list_num != 0);
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                scui_custom_create(&item_maker, &item_handle, false);
                
                scui_custom_maker_t group_maker = {0};
                scui_handle_t group_handle           = SCUI_HANDLE_INVALID;
                group_maker.widget.type              = scui_widget_type_custom;
                group_maker.widget.style.trans       = true;
                group_maker.widget.style.indev_ptr   = true;
                group_maker.widget.clip.w            = item_maker.widget.clip.w;
                group_maker.widget.clip.h            = item_maker.widget.clip.h;
                group_maker.widget.parent            = item_handle;
                group_maker.widget.child_num         = 2;
                group_maker.widget.event_cb          = scui_ui_scene_item_arc_event_proc;
                scui_custom_create(&group_maker, &group_handle, false);
                
                scui_custom_maker_t icon_maker = {0};
                scui_handle_t icon_handle           = SCUI_HANDLE_INVALID;
                icon_maker.widget.type              = scui_widget_type_custom;
                icon_maker.widget.image             = scui_ui_scene_list_image[idx] + 4;
                icon_maker.widget.clip.w            = scui_image_w(icon_maker.widget.image);
                icon_maker.widget.clip.h            = scui_image_h(icon_maker.widget.image);
                icon_maker.widget.clip.y            = (group_maker.widget.clip.h - icon_maker.widget.clip.h) / 2;
                icon_maker.widget.parent            = (group_handle);
                scui_custom_create(&icon_maker, &icon_handle, false);
                
                scui_string_maker_t string_maker = {0};
                scui_handle_t string_handle             = SCUI_HANDLE_INVALID;
                string_maker.widget.type                = scui_widget_type_string;
                string_maker.widget.style.trans         = true;
                string_maker.widget.parent              = group_handle;
                string_maker.args.color.color_s.full    = 0xFFFFFFFF;
                string_maker.args.color.color_e.full    = 0xFFFFFFFF;
                string_maker.args.color.filter          = true;
                string_maker.widget.clip.x              = icon_maker.widget.clip.w + 8;
                string_maker.widget.clip.w              = group_maker.widget.clip.w - (icon_maker.widget.clip.w + 8 * 2);
                string_maker.widget.clip.h              = 58;
                string_maker.widget.clip.y              = (group_maker.widget.clip.h - string_maker.widget.clip.h) / 2;
                string_maker.font_idx                   = 1;
                string_maker.text                       = scui_ui_scene_list_text[idx];
                scui_string_create(&string_maker, &string_handle, false);
            }
            
            item_maker.widget.clip.h = SCUI_DRV_VER_RES / 2 - 3 - 62 / 2;
            scui_custom_create(&item_maker, &item_handle, false);
            
            scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_LIST_ARC_BAR_ARC;
            scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
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
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
