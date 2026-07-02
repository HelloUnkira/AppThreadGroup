/*实现目标:
 *    可缩放列表界面:
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t     item_list[50];
    scui_ui_bar_arc_t bar_arc;
} * scui_ui_res_local = NULL;

/*@brief 弧形进度条事件回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 列表项控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_list_scale_item_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw_graph: {
        SCUI_LOG_INFO("draw widget:%u", event->object);
        
        scui_handle_t match_idx = -1;
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++)
            if (scui_ui_res_local->item_list[idx] == event->object) {
                match_idx = idx;
                continue;
            }
        
        scui_area_t image_clip = scui_widget_area(event->object);
        scui_handle_t image_icon = scui_ui_scene_list_image[match_idx] + 3;
        image_clip.x += (20);
        image_clip.y += (image_clip.h - scui_image_h(image_icon)) / 2;
        image_clip.h -= (image_clip.h - scui_image_h(image_icon));
        scui_widget_draw_image(event->object, &image_clip, image_icon, NULL, SCUI_COLOR_UNUSED);
        
        image_clip = scui_widget_area(event->object);
        image_icon = scui_image_prj_image_src_repeat_arrow_06_backbmp;
        image_clip.x = image_clip.w - scui_image_w(image_icon) - 10;
        image_clip.w = scui_image_w(image_icon);
        image_clip.y += (image_clip.h - scui_image_h(image_icon)) / 2;
        image_clip.h -= (image_clip.h - scui_image_h(image_icon));
        scui_color_t color_white = {.filter = true,.color.full = 0xFFFFFFFF,};
        scui_widget_draw_image(event->object, &image_clip, image_icon, NULL, color_white);
        
        break;
    }
    case scui_event_draw_buffer: {
        SCUI_LOG_INFO("draw buffer widget:%u", event->object);
        
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        // 计算当前控件中心到父控件中心距离
        scui_area_t clip_p = scui_widget_clip(scui_widget_parent(event->object));
        scui_area_t clip_w = scui_widget_clip(event->object);
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
        
        scui_alpha_t alpha_raw = scui_alpha_cover;
        scui_widget_alpha_get(event->object, &alpha_raw);
        // 更新alpha通道
        scui_alpha_t alpha = scui_alpha_pct(percent);
        scui_widget_alpha_set(event->object, scui_alpha_cover, true);
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR_ZEROED);
        scui_widget_alpha_set(event->object, alpha, true);
        
        scui_area_t   btn_clip = scui_widget_area(event->object);
        scui_color_t  btn_color = {.color.full = 0xFF282828,};
        scui_handle_t btn_image[4] = {
            scui_image_prj_image_src_repeat_card_04_r36_1bmp,
            scui_image_prj_image_src_repeat_card_05_r36_2bmp,
            scui_image_prj_image_src_repeat_card_06_r36_3bmp,
            scui_image_prj_image_src_repeat_card_07_r36_4bmp,
        };
        
        scui_point_t img_scale = {0};
        scui_opt_pos_t img_pos = scui_opt_pos_c;
        img_scale.x = 1024 * (scui_multi_t)percent / 100;
        img_scale.y = 1024 * (scui_multi_t)percent / 100;
        scui_coord_t  btn_scale_x = (scui_multi_t)btn_clip.w * (1024 - img_scale.x) / 1024;
        btn_clip.x += btn_scale_x / 2;
        btn_clip.w -= btn_scale_x;
        scui_custom_data_t custom_data;
        scui_custom_data_config_image_crect4(&custom_data, btn_image, btn_color, -1);
        scui_custom_draw_image_crect4(event->object, &btn_clip, &custom_data);
        
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image_scale(event->object, NULL, surface_image, NULL, img_scale, img_pos);
        
        /* 恢复原始 alpha */
        scui_widget_alpha_set(event->object, alpha_raw, true);
        break;
    }
    case scui_event_ptr_click: {

        /* 半透明项忽略点击 */
        scui_alpha_t alpha = scui_alpha_trans;
        scui_widget_alpha_get(event->object, &alpha);
        if (alpha <= scui_alpha_pct20)
            break;

        scui_event_mask_over(event);
        scui_handle_t index = scui_widget_child_to_index(event->object) - 1;
        SCUI_LOG_WARN("click idx:%d", index);
        break;
    }
    default:
        break;
    }
}

/*@brief 滚动控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        {
            /* 上半部分留白 */
            scui_custom_maker_t spacer_maker = {0};
            scui_handle_t spacer_handle = SCUI_HANDLE_INVALID;
            scui_widget_maker_def_cfg(&spacer_maker, scui_widget_type_custom);
            spacer_maker.widget.style.fully_bg  = true;
            spacer_maker.widget.style.cover_fg  = true;
            spacer_maker.widget.style.indev_ptr = false;
            spacer_maker.widget.parent          = event->object;
            spacer_maker.widget.clip.w          = SCUI_HOR_RES;
            spacer_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 72 / 2;
            scui_widget_create(&spacer_maker, &spacer_handle);
        }
        
        /* 列表项(核心区域) */
        {
            scui_custom_maker_t item_maker = {0};
            scui_handle_t item_handle = SCUI_HANDLE_INVALID;
            scui_widget_maker_def_cfg(&item_maker, scui_widget_type_custom);
            
            item_maker.widget.style.buffer    = true;
            item_maker.widget.style.buffer_d  = true;
            item_maker.widget.style.fully_bg  = true;
            item_maker.widget.style.cover_buf = true;
            item_maker.widget.style.indev_ptr = true;
            item_maker.widget.format          = SCUI_PIXEL_CF_DEF_A;
            item_maker.widget.parent          = event->object;
            item_maker.widget.clip.w          = SCUI_HOR_RES;
            item_maker.widget.clip.h          = 72;
            item_maker.widget.event_cb        = scui_ui_scene_list_scale_item_event;
            item_maker.widget.child_num       = 5;
            
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                scui_widget_create(&item_maker, &item_handle);
                scui_ui_res_local->item_list[idx] = item_handle;
                
                scui_string_maker_t string_maker = {0};
                scui_handle_t string_handle = SCUI_HANDLE_INVALID;
                string_maker.widget.type             = scui_widget_type_string;
                string_maker.widget.parent           = item_handle;
                string_maker.widget.clip.x           = 52 + 8 + 20;
                string_maker.widget.clip.w           = item_maker.widget.clip.w - (52 + 16 + 8 * 2);
                string_maker.widget.clip.h           = item_maker.widget.clip.h;
                string_maker.args.align_hor          = 0;
                string_maker.args.align_ver          = 2;
                string_maker.args.mode_scroll        = 1;
                string_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_maker.args.color.color_e.full = 0xFFFFFFFF;
                string_maker.args.color.filter       = true;
                // string_maker.draw_cache              = true;
                string_maker.text                    = scui_ui_scene_list_text[idx];
                string_maker.font_idx                = SCUI_FONT_IDX_36;
                scui_widget_create(&string_maker, &string_handle);
            }
        }
        
        /* 下半部分留白 */
        {
            scui_custom_maker_t spacer_maker = {0};
            scui_handle_t spacer_handle = SCUI_HANDLE_INVALID;
            scui_widget_maker_def_cfg(&spacer_maker, scui_widget_type_custom);
            spacer_maker.widget.style.fully_bg  = true;
            spacer_maker.widget.style.cover_fg  = true;
            spacer_maker.widget.style.indev_ptr = false;
            spacer_maker.widget.parent          = event->object;
            spacer_maker.widget.clip.w          = SCUI_HOR_RES;
            spacer_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 72 / 2;
            scui_widget_create(&spacer_maker, &spacer_handle);
        }
        
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_LIST_SCALE_BAR_ARC;
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

/*@brief 窗口控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        /* 清空图像资源缓存并加载该场景资源 */
        scui_ui_scene_list_cfg(scui_ui_scene_list_type_list_scale);
        
        scui_scroll_maker_t scroll_maker = {0};
        scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
        scui_widget_maker_def_cfg(&scroll_maker, scui_widget_type_scroll);
        scroll_maker.widget.style.fully_bg  = true;
        scroll_maker.widget.style.indev_enc = true;
        scroll_maker.widget.style.indev_key = true;
        scroll_maker.widget.clip.w          = SCUI_HOR_RES;
        scroll_maker.widget.clip.h          = SCUI_VER_RES;
        scroll_maker.widget.parent          = event->object;
        scroll_maker.widget.event_cb   = scui_ui_scene_list_scale_scroll_event;
        scroll_maker.widget.child_num  = 50;
        scroll_maker.pos               = scui_opt_pos_c;
        scroll_maker.dir               = scui_opt_dir_ver;
        scroll_maker.space             = 10;
        scroll_maker.route_enc         = 117;
        scroll_maker.route_key         = 117;
        scroll_maker.keyid_fdir        = SCUI_WIDGET_SCROLL_KEY_FDIR;
        scroll_maker.keyid_bdir        = SCUI_WIDGET_SCROLL_KEY_BDIR;
        scroll_maker.springback        = 70;
        scui_widget_create(&scroll_maker, &scroll_handle);
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_key_click:
        
        #if 0
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_del(SCUI_UI_SCENE_LIST);
        scui_window_switch_type_cfg_set(type);
        scui_event_mask_over(event);
        #endif
        break;
    default:
        break;
    }
}
