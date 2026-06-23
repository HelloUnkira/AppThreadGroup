/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t item_list[50];
    scui_coord_t  list_num;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_quick_card_item_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw_graph: {
        
        scui_handle_t match_idx = -1;
        for (uint8_t idx = 0; idx < scui_ui_scene_mini_card_num; idx++)
            if (scui_ui_res_local->item_list[idx] == event->object) {
                match_idx = idx;
                continue;
            }
        
        /* 绘制背景 */
        scui_area_t   clip = scui_widget_area(event->object);
        scui_color_t  color = {.color.full = 0xFF282828,};
        scui_handle_t image[4] = {
            scui_image_prj_image_src_repeat_card_04_r36_1bmp,
            scui_image_prj_image_src_repeat_card_05_r36_2bmp,
            scui_image_prj_image_src_repeat_card_06_r36_3bmp,
            scui_image_prj_image_src_repeat_card_07_r36_4bmp,
        };
        scui_custom_data_t custom_data;
        scui_custom_data_config_image_crect4(&custom_data, image, color, -1);
        scui_custom_draw_image_crect4(event->object, &clip, &custom_data);
        
        
        
        break;
    }
    case scui_event_draw_buffer: {
        
        // 计算当前控件中心到父控件中心距离
        scui_area_t clip_p = scui_widget_clip(scui_widget_tree(event->object));
        scui_area_t clip_w = scui_widget_clip(event->object);
        
        #if 1
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        if ((clip_p.y + clip_p.h / 2 < clip_w.y + clip_w.h / 2)) {
            offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_w.x + clip_w.w / 2);
            offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_w.y + clip_w.h / 2);
            
            scui_multi_t rad_rr = clip_p.h / 2 - clip_w.h / 2;
            scui_multi_t dist_y = (1024 * scui_min(rad_rr, offset.y) / rad_rr);
            scui_multi_t cos_a2 = (1024 * 1024) - dist_y * dist_y;
            scui_multi_t cos_ia = 0, cos_fa = 0;
            scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
            scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
            SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
            
            dist_x  = scui_min(dist_x, clip_p.w / 2);
            percent = (clip_p.w / 2 - dist_x) * 100 / (clip_p.w / 2);
            percent = scui_min(scui_max(percent, 30), 100);
            SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        }
        
        scui_alpha_t alpha_raw = scui_alpha_cover;
        scui_widget_alpha_get(event->object, &alpha_raw);
        #if 0   // 更新alpha通道
        scui_alpha_t alpha = scui_alpha_pct(percent);
        scui_widget_alpha_set(event->object, scui_alpha_cover, true);
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR_UNUSED);
        scui_widget_alpha_set(event->object, alpha, true);
        #endif
        
        scui_point_t img_scale = {0};
        scui_opt_pos_t img_pos = scui_opt_pos_c;
        img_scale.x = 1024 * (scui_multi_t)percent / 100;
        img_scale.y = 1024 * (scui_multi_t)percent / 100;
        
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image_scale(event->object, NULL, surface_image, NULL, img_scale, img_pos);
        scui_widget_alpha_set(event->object, alpha_raw, true);
        #else
        scui_area_t target = {
            .x = (clip_p.w - src_clip.w) / 2,
            .y = 0,
            .w = clip_w.w,
            .h = clip_w.h,
        };
        scui_widget_draw_image(event->object, &target, surface_image, NULL, SCUI_COLOR_UNUSED);
        #endif
        
        break;
    }
    case scui_event_ptr_click: {
        
        scui_event_mask_over(event);
        scui_handle_t parent = scui_widget_parent(event->object);
        scui_handle_t scroll = scui_widget_parent(parent);
        scui_handle_t index  = scui_widget_child_to_index(parent) - 1;
        SCUI_LOG_WARN("click idx:%d", index);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_quick_card_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        {
            /* 上半部分留白 */
            scui_custom_maker_t spacer_maker = {0};
            scui_handle_t spacer_handle = SCUI_HANDLE_INVALID;
            scui_widget_maker_def_cfg(&spacer_maker, scui_widget_type_custom);
            spacer_maker.widget.style.indev_ptr = false;
            spacer_maker.widget.parent          = event->object;
            spacer_maker.widget.clip.w          = SCUI_HOR_RES;
            spacer_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 180 / 2;
            scui_widget_create(&spacer_maker, &spacer_handle);
        }
        
        // list的各个子控件树
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
            
            scui_custom_maker_t cont_maker = {0};
            scui_custom_maker_t item_maker = {0};
            scui_handle_t cont_handle = SCUI_HANDLE_INVALID;
            scui_handle_t item_handle = SCUI_HANDLE_INVALID;
            scui_widget_maker_def_cfg(&cont_maker, scui_widget_type_custom);
            scui_widget_maker_def_cfg(&item_maker, scui_widget_type_custom);
            
            cont_maker.widget.style.fully_bg  = true;
            cont_maker.widget.parent          = event->object;
            cont_maker.widget.clip.w          = SCUI_HOR_RES;
            cont_maker.widget.clip.h          = 180;
            cont_maker.widget.child_num       = 1;
            scui_widget_create(&cont_maker, &cont_handle);
            
            item_maker.widget.style.buffer    = true;
            item_maker.widget.style.buffer_d  = true;
            item_maker.widget.style.fully_bg  = true;
            item_maker.widget.style.indev_ptr = true;
            item_maker.widget.format          = SCUI_PIXEL_CF_DEF_A;
            item_maker.widget.parent          = cont_handle;
            item_maker.widget.clip.w          = 410;
            item_maker.widget.clip.h          = 180;
            item_maker.widget.clip.x          = (SCUI_HOR_RES - item_maker.widget.clip.w) / 2;
            item_maker.widget.event_cb        = scui_ui_scene_quick_card_item_event;
            item_maker.widget.child_num       = 50;
            
            scui_widget_create(&item_maker, &item_handle);
            scui_ui_res_local->item_list[idx] = item_handle;
        }
        
        /* 下半部分留白 */
        {
            scui_custom_maker_t spacer_maker = {0};
            scui_handle_t spacer_handle = SCUI_HANDLE_INVALID;
            scui_widget_maker_def_cfg(&spacer_maker, scui_widget_type_custom);
            spacer_maker.widget.style.indev_ptr = false;
            spacer_maker.widget.parent          = event->object;
            spacer_maker.widget.clip.w          = SCUI_HOR_RES;
            spacer_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 180 / 2;
            scui_widget_create(&spacer_maker, &spacer_handle);
        }
        
        break;
    }
    case scui_event_scroll_layout:
    case scui_event_scroll_start:
    case scui_event_scroll_keep:
    case scui_event_scroll_over: {
        
        scui_area_t   clip_p = scui_widget_clip(event->object);
        scui_handle_t list_m = scui_widget_child_now(event->object);
        scui_handle_t handle_t = SCUI_HANDLE_INVALID;
        
        #if 0
        // 寻找到的最后一个完全可显示控件
        for (scui_multi_t idx = 0; idx < list_m; idx++) {
            scui_handle_t handle_c = scui_widget_child_by_index(event->object, idx);
            if (handle_c == SCUI_HANDLE_INVALID)
                continue;
            
            scui_area_t clip_w = scui_widget_clip(handle_c);
            if (clip_w.y + clip_w.h > clip_p.y + clip_p.h)
                break;
            
            handle_t = handle_c;
        }
        #else
        // 寻找到中心可显示控件
        scui_point_t offset = {0};
        scui_widget_align_pos_calc(event->object, &handle_t, &offset, scui_opt_pos_c);
        #endif
        
        if (handle_t == SCUI_HANDLE_INVALID)
            return;
        
        scui_handle_t span  = 25;
        scui_handle_t num_c = 2;
        scui_handle_t idx_c = 0;
        scui_handle_t index = scui_widget_child_to_index(handle_t);
        for (scui_multi_t idx = index + 1; idx < list_m; idx++) {
            scui_handle_t handle_c = scui_widget_child_by_index(event->object, idx);
            if (handle_c == SCUI_HANDLE_INVALID)
                continue;
            
            if (idx_c > num_c)
                break;
            
            scui_area_t   clip_c = scui_widget_clip(handle_c);
            scui_point_t point_c = {.x = clip_c.x, };
            point_c.y = clip_p.y + clip_p.h - (num_c - idx_c) * span - clip_c.h;
            scui_widget_move_pos(handle_c, &point_c);
            
            idx_c++;
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
void scui_ui_scene_quick_card_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        scui_ui_res_local->list_num = 10;
        
        scui_scroll_maker_t scroll_maker = {0};
        scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&scroll_maker, scui_widget_type_scroll);
        scroll_maker.widget.style.fully_bg   = true;
        scroll_maker.widget.style.indev_enc  = true;
        scroll_maker.widget.style.indev_key  = true;
        scroll_maker.widget.style.order_draw = true;
        scroll_maker.widget.clip.w = SCUI_HOR_RES;
        scroll_maker.widget.clip.h = SCUI_VER_RES;
        scroll_maker.widget.parent = event->object;
        scroll_maker.widget.event_cb   = scui_ui_scene_quick_card_scroll_event;
        scroll_maker.widget.child_num  = scui_ui_res_local->list_num;
        scroll_maker.pos        = scui_opt_pos_c;
        scroll_maker.dir        = scui_opt_dir_ver;
        scroll_maker.space      = 3;
        scroll_maker.route_enc  = 117;
        scroll_maker.route_key  = 117;
        scroll_maker.keyid_fdir = SCUI_WIDGET_SCROLL_KEY_FDIR;
        scroll_maker.keyid_bdir = SCUI_WIDGET_SCROLL_KEY_BDIR;
        scroll_maker.springback = 70;
        scui_widget_create(&scroll_maker, &scroll_handle);
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw_graph: {
        
        // 这里画个圈,校验测试使用
        #if 0
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += 4; clip.y += 4;
        scui_widget_draw_image(event->object, &clip,
    scui_image_prj_image_src_03_activity_ring_big_max_03_bj_01bmp,
    NULL,
    SCUI_COLOR_MAKE32(true, 0xFF000000, 0xFF404040));
        #endif
        
        break;
    }
    default:
        break;
    }
}
