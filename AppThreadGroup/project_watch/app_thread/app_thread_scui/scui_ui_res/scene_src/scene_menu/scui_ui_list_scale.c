/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t     linear;
    scui_ui_bar_arc_t bar_arc;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_s_event_proc(scui_event_t *event)
{
    // 特殊的固定调用
    scui_linear_s_event(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_color_t  btn_color_full = {.color.full = 0x00282828,};
        scui_widget_draw_color(event->object, NULL, btn_color_full);
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_area_t image_clip = scui_widget_clip(event->object);
        scui_handle_t image_icon = scui_ui_scene_list_image[linear_item.draw_idx] + 3;
        image_clip.y += (image_clip.h - scui_image_h(image_icon)) / 2;
        image_clip.h -= (image_clip.h - scui_image_h(image_icon));
        scui_widget_draw_image(event->object, &image_clip, image_icon, NULL, SCUI_COLOR_UNUSED);
        
        image_clip = scui_widget_clip(event->object);
        image_icon = scui_image_prj_image_src_repeat_arrow_06_backbmp;
        image_clip.x = image_clip.w - scui_image_w(image_icon);
        image_clip.w = scui_image_w(image_icon);
        image_clip.y += (image_clip.h - scui_image_h(image_icon)) / 2;
        image_clip.h -= (image_clip.h - scui_image_h(image_icon));
        scui_color_t color_white = {.filter = true,.color.full = 0xFFFFFFFF,};
        scui_widget_draw_image(event->object, &image_clip, image_icon, NULL, color_white);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_m_event_proc(scui_event_t *event)
{
    // 特殊的固定调用
    scui_linear_m_event(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_handle_t  custom = linear_item.handle_s;
        scui_area_t  src_clip = scui_widget_clip(custom);
        scui_area_t  dst_clip = scui_widget_clip(event->object);
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        
        
        
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
        
        
        
        scui_area_t   btn_clip = dst_clip;
        scui_color_t  btn_color_full = {.color.full = 0xFF282828,};
        scui_handle_t btn_image_full[4] = {
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
        scui_custom_draw_image_crect4(event, &btn_clip, btn_image_full, btn_color_full, -1);
        scui_widget_draw_image_scale(event->object, NULL, image, NULL, img_scale, img_pos);
        
        
        
        scui_widget_alpha_set(event->object, alpha_raw, true);
        scui_handle_clear(image);
        break;
    }
    case scui_event_ptr_click: {
        
        scui_alpha_t alpha = scui_alpha_trans;
        scui_widget_alpha_get(event->object, &alpha);
        if (alpha <= scui_alpha_pct20)
            break;
        
        scui_event_mask_over(event);
        scui_handle_t parent = scui_widget_parent(event->object);
        scui_handle_t index  = scui_widget_child_to_index(parent, event->object) - 1;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_handle_t custom = linear_item.handle_m;
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
void scui_ui_scene_list_scale_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_scroll_event(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_custom_maker_t custom_maker = {0};
        scui_handle_t custom_handle             = SCUI_HANDLE_INVALID;
        custom_maker.widget.type                = scui_widget_type_custom;
        custom_maker.widget.style.sched_anima   = true;
        custom_maker.widget.clip.w              = SCUI_HOR_RES;
        custom_maker.widget.parent              = event->object;
        
        // 上半部分留白占用
        custom_maker.widget.style.indev_ptr     = false;
        custom_maker.widget.clip.h              = SCUI_VER_RES / 2 - 10 - 72 / 2;
        custom_maker.widget.event_cb            = NULL;
        scui_widget_create(&custom_maker, &custom_handle);
        
        // list的各个子控件
        custom_maker.widget.style.indev_ptr     = true;
        custom_maker.widget.clip.h              = 72;
        custom_maker.widget.event_cb            = scui_ui_scene_item_m_event_proc;
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            scui_widget_create(&custom_maker, &custom_handle);
            scui_linear_item_t linear_item = {.draw_idx = idx,};
            scui_linear_item_gets(event->object, &linear_item);
            linear_item.handle_m = custom_handle;
            scui_linear_item_sets(event->object, &linear_item);
        }
        
        // 下半部分留白占用
        custom_maker.widget.style.indev_ptr     = false;
        custom_maker.widget.clip.h              = SCUI_VER_RES / 2 - 10 - 72 / 2;
        custom_maker.widget.event_cb            = NULL;
        scui_widget_create(&custom_maker, &custom_handle);
        
        // list的各个子控件树
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            
            scui_coord_t scale_w = scui_image_w(scui_ui_scene_list_image[0] + 3);
            scui_coord_t scale_h = scui_image_h(scui_ui_scene_list_image[0] + 3);
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
            custom_maker.widget.type        = scui_widget_type_custom;
            custom_maker.widget.clip.w      = SCUI_HOR_RES - 20 - 10;
            custom_maker.widget.clip.h      = scale_h;
            custom_maker.widget.child_num   = 1;
            custom_maker.widget.event_cb    = scui_ui_scene_item_s_event_proc;
            scui_widget_create(&custom_maker, &custom_handle);
            scui_linear_item_t linear_item = {.draw_idx = idx,};
            scui_linear_item_gets(event->object, &linear_item);
            linear_item.handle_s = custom_handle;
            scui_linear_item_sets(event->object, &linear_item);
            scui_handle_t *handle_m = NULL;
            scui_custom_handle_m(custom_handle, &handle_m);
            *handle_m = linear_item.handle_m;
            
            scui_string_maker_t string_maker = {0};
            scui_handle_t string_handle             = SCUI_HANDLE_INVALID;
            string_maker.widget.type                = scui_widget_type_string;
            string_maker.widget.parent              = custom_handle;
            string_maker.widget.clip.x              = 52 + 8;
            string_maker.widget.clip.w              = custom_maker.widget.clip.w - (52 + 16 + 8 * 2);
            string_maker.widget.clip.h              = custom_maker.widget.clip.h;
            string_maker.args.align_hor             = 0;
            string_maker.args.align_ver             = 2;
            string_maker.args.mode_scroll           = 1;
            string_maker.args.color.color_s.full    = 0xFFFFFFFF;
            string_maker.args.color.color_e.full    = 0xFFFFFFFF;
            string_maker.args.color.filter          = true;
            // string_maker.draw_cache                 = true;
            string_maker.text                       = scui_ui_scene_list_text[idx];
            string_maker.font_idx                   = SCUI_FONT_IDX_36;
            scui_widget_create(&string_maker, &string_handle);
        }
        
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_LIST_SCALE_BAR_ARC;
        break;
    }
    case scui_event_widget_scroll_layout:
    case scui_event_widget_scroll_start:
    case scui_event_widget_scroll_keep:
    case scui_event_widget_scroll_over: {
        
        scui_coord_t scroll_pct = 0;
        scui_scroll_percent_get(event->object, &scroll_pct);
        scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
        scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        break;
    }
    default:
        break;
    }
    
    
    
    // 转移至控件调度
    scui_widget_event_shift(event);
}

/*@brief 控件事件响应回调
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
        
        // 清空图像资源缓存
        scui_image_cache_clear();
        scui_ui_scene_list_cfg(scui_ui_scene_list_type_list_scale);
        
        scui_linear_maker_t linear_maker = {0};
        scui_handle_t linear_handle = SCUI_HANDLE_INVALID;
        linear_maker.widget.type = scui_widget_type_linear;
        linear_maker.widget.style.cover_bg  = true;
        linear_maker.widget.style.indev_enc = true;
        linear_maker.widget.style.indev_key = true;
        linear_maker.widget.clip.w = SCUI_HOR_RES;
        linear_maker.widget.clip.h = SCUI_VER_RES;
        linear_maker.widget.parent = event->object;
        linear_maker.widget.event_cb   = scui_ui_scene_list_scale_scroll_event;
        linear_maker.widget.child_num  = 50;
        linear_maker.scroll.pos        = scui_opt_pos_c;
        linear_maker.scroll.dir        = scui_opt_dir_ver;
        linear_maker.scroll.space      = 10;
        linear_maker.scroll.route_enc  = 117;
        linear_maker.scroll.route_key  = 117;
        linear_maker.scroll.keyid_fdir = SCUI_WIDGET_SCROLL_KEY_FDIR;
        linear_maker.scroll.keyid_bdir = SCUI_WIDGET_SCROLL_KEY_BDIR;
        linear_maker.scroll.springback = 70;
        linear_maker.list_num = scui_ui_scene_list_num;
        scui_widget_create(&linear_maker, &linear_handle);
        scui_ui_res_local->linear = linear_handle;
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_key_click:
        
        #if 0
        scui_plug_coupler_recycle(scui_ui_res_local->coupler, true);
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_del();
        scui_window_switch_type_cfg_set(type);
        scui_event_mask_over(event);
        #endif
        break;
    default:
        break;
    }
}
