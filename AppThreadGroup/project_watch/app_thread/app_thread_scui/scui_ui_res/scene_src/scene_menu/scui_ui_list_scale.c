/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t    list_idx;
    scui_handle_t   *list_widget;
    scui_surface_t **list_surface;
    bool            *list_refr;
    bool            *list_draw;
    
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_scroll_draw_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw: {
        
        if (scui_widget_event_check_finish(event)) {
            
            uintptr_t surface_nums = 0;
            uintptr_t surface_bytes = 0;
            
            // scui_widget_clip_check(scui_widget_root(event->object), true);
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                if (scui_ui_res_local->list_draw[idx]) {
                    scui_ui_res_local->list_draw[idx] = false;
                    
                    // 统计画布的内存占用
                    scui_surface_t *surface = scui_ui_res_local->list_surface[idx];
                    if (surface != NULL) {
                        SCUI_LOG_DEBUG("surface index:%x", idx);
                        uintptr_t cf_byte = scui_pixel_bits(surface->format) / 8;
                        surface_bytes += surface->hor_res * surface->ver_res * cf_byte;
                        surface_nums++;
                    }
                    continue;
                }
                
                scui_widget_t widget_t = {.surface = scui_ui_res_local->list_surface[idx],};
                scui_handle_t handle_t = scui_handle_find();
                scui_handle_set(handle_t, &widget_t);
                scui_widget_surface_destroy(handle_t);
                scui_handle_set(handle_t, NULL);
                scui_ui_res_local->list_surface[idx] = NULL;
            }
            
            SCUI_LOG_INFO("surface num:%u, total bytes:%x", surface_nums, surface_bytes);
        }
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
    
    scui_scroll_event(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_string_draw_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw:
        
        if (scui_widget_surface(event->object) == NULL) {
            scui_handle_t handle = scui_widget_root(event->object);
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                if (scui_ui_res_local->list_widget[idx] != handle)
                    continue;
                handle = scui_widget_child_by_index(SCUI_UI_SCENE_LIST_SCALE_SCROLL, idx + 1);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                scui_widget_draw(handle, NULL, false);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                scui_ui_res_local->list_refr[idx] = true;
                break;
            }
            
            return;
        }
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
    
    scui_string_event(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_event_proc(scui_event_t *event)
{
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
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_color_t  btn_color_full = {.color.full = 0x00282828,};
        scui_widget_draw_color(event->object, NULL, btn_color_full);
        
        scui_area_t   image_clip = scui_widget_clip(event->object);
        scui_handle_t image_icon = scui_ui_scene_list_image[scui_ui_res_local->list_idx] + 3;
        image_clip.y += (image_clip.h - scui_image_h(image_icon)) / 2;
        image_clip.h -= (image_clip.h - scui_image_h(image_icon));
        scui_widget_draw_image(event->object, &image_clip, image_icon, NULL, (scui_color_t){0});
        
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
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_scale_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(parent, event->object) - 1;
        scui_handle_t  custom = scui_ui_res_local->list_widget[index];
        SCUI_LOG_INFO("list_idx:%d", index);
        
        scui_ui_res_local->list_idx = index;
        /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
        if (scui_widget_draw_empty(event->object)) {
            scui_ui_res_local->list_draw[index] = false;
            
            if (scui_ui_res_local->list_surface[index] != NULL) {
                scui_widget_t widget_t = {.surface = scui_ui_res_local->list_surface[index],};
                scui_handle_t handle_t = scui_handle_find();
                scui_handle_set(handle_t, &widget_t);
                scui_widget_surface_destroy(handle_t);
                scui_handle_set(handle_t, NULL);
                scui_ui_res_local->list_surface[index] = NULL;
            }
            break;
        }
        
        // 创建一个独立临时的子画布,将目标绘制到一个独立子画布中
        if (scui_ui_res_local->list_surface[index] == NULL) {
            scui_area_t clip = scui_widget_clip(custom);
            scui_widget_surface_create(custom, SCUI_PIXEL_CF_DEF_A, scui_pixel_pb_shared, clip.w, clip.h);
            scui_ui_res_local->list_surface[index] = scui_widget_surface(custom);
            scui_ui_res_local->list_refr[index] = true;
        }
        scui_widget_surface_remap(custom, scui_ui_res_local->list_surface[index]);
        
        if (scui_ui_res_local->list_refr[index]) {
            scui_ui_res_local->list_refr[index] = false;
            scui_widget_show(custom, false);
            scui_widget_draw(custom, NULL, true);
        }
        scui_ui_res_local->list_draw[index] = true;
        
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        scui_event_pos_t pos = scui_event_pos_c;
        {   // 计算当前控件中心到父控件中心距离
            scui_area_t clip_p = scui_widget_clip(SCUI_UI_SCENE_LIST_SCALE);
            scui_area_t clip_w = scui_widget_clip(event->object);
            offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_w.x + clip_w.w / 2);
            offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_w.y + clip_w.h / 2);
            
            scui_multi_t rad_rr = clip_p.h / 2 - clip_w.h / 2;
            scui_multi_t dist_y = scui_min(rad_rr, offset.y);
            
            scui_multi_t cos_a2 = (1024 * 1024) - (1024 * dist_y / rad_rr) * (1024 * dist_y / rad_rr);
            scui_multi_t cos_ia = 0;
            scui_multi_t cos_fa = 0;
            scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
            scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
            SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
            
            dist_x  = scui_min(dist_x, clip_p.w / 2);
            percent = (clip_p.w / 2 - dist_x) * 100 / (clip_p.w / 2);
            percent = scui_min(percent, 100);
            percent = scui_max(percent, 30);
            SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        }
        
        #if 1   // 更新alpha通道
        scui_alpha_t alpha = scui_alpha_pct(percent);
        scui_widget_alpha_set(event->object, scui_alpha_cover, true);
        scui_widget_draw_color(event->object, NULL, (scui_color_t){0});
        scui_widget_alpha_set(event->object, alpha, true);
        #endif
        
        scui_area_t  src_clip  = scui_widget_clip(custom);
        scui_area_t  dst_clip  = scui_widget_clip(event->object);
        scui_point_t img_scale = {
            .x = 1024 * (scui_multi_t)percent / 100,
            .y = 1024 * (scui_multi_t)percent / 100,
        };
        
        scui_area_t   btn_clip = dst_clip;
        scui_color_t  btn_color_full = {.color.full = 0xFF282828,};
        scui_handle_t btn_image_full[4] = {
            scui_image_prj_image_src_repeat_card_04_r36_1bmp,
            scui_image_prj_image_src_repeat_card_05_r36_2bmp,
            scui_image_prj_image_src_repeat_card_06_r36_3bmp,
            scui_image_prj_image_src_repeat_card_07_r36_4bmp,
        };
        
        scui_coord_t  btn_scale_x = (scui_multi_t)btn_clip.w * (1024 - img_scale.x) / 1024;
        btn_clip.x += btn_scale_x / 2;
        btn_clip.w -= btn_scale_x;
        scui_button_draw_rect4(event, &btn_clip, btn_image_full, btn_color_full, -1);
        
        scui_image_t img_inst  = {
            .status         = scui_image_status_mem,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_mem = scui_widget_surface(custom)->pixel,
        };
        scui_image_cf_by_pixel_cf(&img_inst.format, &scui_widget_surface(custom)->format);
        
        scui_coord_t offset_x = (dst_clip.w - src_clip.w);
        scui_coord_t offset_y = (dst_clip.h - src_clip.h);
        dst_clip.x  = btn_clip.x;
        dst_clip.w  = btn_clip.w;
        dst_clip.y += offset_y / 2;
        dst_clip.h -= offset_y;
        
        scui_handle_t image = scui_handle_find();
        scui_handle_set(image, &img_inst);
        scui_widget_draw_image_scale(event->object, &dst_clip, image, NULL, img_scale, pos);
        // scui_widget_draw_image(event->object, &dst_clip, image, NULL, (scui_color_t){0});
        scui_handle_set(image, NULL);
        
        scui_widget_surface_remap(custom, NULL);
        break;
    }
    case scui_event_ptr_click: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_alpha_t alpha = scui_widget_alpha_get(event->object);
        if (alpha <= scui_alpha_pct20)
            break;
        
        scui_widget_event_mask_over(event);
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(parent, event->object) - 1;
        scui_handle_t  custom = scui_ui_res_local->list_widget[index];
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
void scui_ui_scene_list_scale_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        scui_handle_t handle = event->object;
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            event->object = scui_ui_res_local->list_widget[idx];
            scui_widget_event_dispatch(event);
        }
        event->object = handle;
        
        break;
    }
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
            
            SCUI_ASSERT(scui_ui_scene_list_num != 0);
            scui_handle_t list_num = scui_ui_scene_list_num;
            scui_ui_res_local->list_widget  = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(scui_handle_t));
            scui_ui_res_local->list_surface = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(scui_surface_t *));
            scui_ui_res_local->list_refr    = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(bool));
            scui_ui_res_local->list_draw    = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(bool));
            
            memset(scui_ui_res_local->list_widget,  0, list_num * sizeof(scui_handle_t));
            memset(scui_ui_res_local->list_surface, 0, list_num * sizeof(scui_surface_t *));
            memset(scui_ui_res_local->list_refr,    0, list_num * sizeof(bool));
            memset(scui_ui_res_local->list_draw,    0, list_num * sizeof(bool));
        }
        
        if (scui_widget_event_check_prepare(event)) {
            
            scui_event_cb_node_t event_cb_node = {
                .event_cb = scui_ui_scene_scroll_draw_proc,
                .event = scui_event_draw,
            };
            scui_widget_event_add(SCUI_UI_SCENE_LIST_SCALE_SCROLL, &event_cb_node);
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
            custom_maker.widget.type        = scui_widget_type_custom;
            custom_maker.widget.style.trans = true;
            custom_maker.widget.clip.w      = SCUI_DRV_HOR_RES;
            custom_maker.widget.parent      = SCUI_UI_SCENE_LIST_SCALE_SCROLL;
            
            custom_maker.widget.style.indev_ptr = false;
            custom_maker.widget.clip.h   = SCUI_DRV_VER_RES / 2 - 10 - 72 / 2;
            custom_maker.widget.event_cb = NULL;
            scui_custom_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.style.indev_ptr = true;
            custom_maker.widget.clip.h   = 72;
            custom_maker.widget.event_cb = scui_ui_scene_item_scale_event_proc;
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++)
                scui_custom_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.style.indev_ptr = false;
            custom_maker.widget.clip.h   = SCUI_DRV_VER_RES / 2 - 10 - 72 / 2;
            custom_maker.widget.event_cb = NULL;
            scui_custom_create(&custom_maker, &custom_handle, false);
            
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                
                scui_custom_maker_t custom_maker = {0};
                scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
                custom_maker.widget.type        = scui_widget_type_custom;
                custom_maker.widget.style.trans = true;
                custom_maker.widget.clip.w      = SCUI_DRV_HOR_RES - 20 - 10;
                custom_maker.widget.clip.h      = 60;
                custom_maker.widget.child_num   = 2;
                custom_maker.widget.event_cb    = scui_ui_scene_item_event_proc;
                scui_custom_create(&custom_maker, &custom_handle, false);
                scui_ui_res_local->list_widget[idx] = custom_handle;
                
                scui_string_maker_t string_maker = {0};
                scui_handle_t string_handle             = SCUI_HANDLE_INVALID;
                string_maker.widget.type                = scui_widget_type_string;
                string_maker.widget.style.trans         = true;
                string_maker.widget.parent              = custom_handle;
                string_maker.args.gap_none              = SCUI_STRING_SPACE_WIDTH;
                string_maker.args.color.color_s.full    = 0xFFFFFFFF;
                string_maker.args.color.color_e.full    = 0xFFFFFFFF;
                string_maker.args.color.filter          = true;
                string_maker.widget.clip.x              = 52 + 8;
                string_maker.widget.clip.w              = custom_maker.widget.clip.w - (52 + 16 + 8 * 2);
                string_maker.widget.clip.h              = 58;
                string_maker.widget.clip.y              = (60 - 58) / 2;
                string_maker.font_idx                   = 1;
                string_maker.text                       = scui_ui_scene_list_text[idx];
                scui_string_create(&string_maker, &string_handle, false);
                
                scui_event_cb_node_t event_cb_node = {
                    .event_cb = scui_ui_scene_string_draw_proc,
                    .event = scui_event_draw,
                };
                scui_widget_event_add(string_handle, &event_cb_node);
                
                scui_widget_surface_remap(custom_handle, NULL);
            }
            
            scui_image_cache_clear();
            scui_ui_bar_arc_reset(SCUI_UI_SCENE_LIST_SCALE_RING);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        if (scui_widget_event_check_finish(event)) {
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                scui_widget_hide(scui_ui_res_local->list_widget[idx], false);
                scui_ui_res_local->list_widget[idx] = SCUI_HANDLE_INVALID;
                
                scui_widget_t widget_t = {.surface = scui_ui_res_local->list_surface[idx],};
                scui_handle_t handle_t = scui_handle_find();
                scui_handle_set(handle_t, &widget_t);
                scui_widget_surface_destroy(handle_t);
                scui_handle_set(handle_t, NULL);
                scui_ui_res_local->list_surface[idx] = NULL;
            }
        }
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            SCUI_MEM_FREE(scui_ui_res_local->list_draw);
            SCUI_MEM_FREE(scui_ui_res_local->list_refr);
            SCUI_MEM_FREE(scui_ui_res_local->list_surface);
            SCUI_MEM_FREE(scui_ui_res_local->list_widget);
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
    case scui_event_key_click:
        
        for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
            scui_widget_t widget_t = {.surface = scui_ui_res_local->list_surface[idx],};
            scui_handle_t handle_t = scui_handle_find();
            scui_handle_set(handle_t, &widget_t);
            scui_widget_surface_destroy(handle_t);
            scui_handle_set(handle_t, NULL);
            scui_ui_res_local->list_surface[idx] = NULL;
        }
        
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_del();
        scui_window_switch_type_cfg_set(type);
        scui_widget_event_mask_over(event);
        break;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_list_scale_mask_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw: {
        // 额外绘制一个全局遮罩
        scui_area_t clip = {0};
        
        // 内存真的不够绘制遮罩
        
        scui_handle_t image_mask_u = scui_image_prj_image_src_repeat_mask_06_big_uppng;
        clip.w = scui_image_w(image_mask_u);
        clip.h = scui_image_h(image_mask_u);
        clip.x = (466 - clip.w) / 2;
        clip.y = 0;
        // scui_widget_draw_image(event->object, &clip, image_mask_u, NULL, (scui_color_t){0});
        
        scui_handle_t image_mask_d = scui_image_prj_image_src_repeat_mask_05_big_downpng;
        clip.w = scui_image_w(image_mask_d);
        clip.h = scui_image_h(image_mask_d);
        clip.x = (466 - clip.w) / 2;
        clip.y = (466 - clip.h);
        // scui_widget_draw_image(event->object, &clip, image_mask_d, NULL, (scui_color_t){0});
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
void scui_ui_scene_list_scale_scroll_notify_event(scui_event_t *event)
{
    scui_coord_t scroll_pct = 0;
    scui_scroll_auto_percent_get(event->object, &scroll_pct);
    SCUI_LOG_INFO("pct:%d", scroll_pct);
    scui_ui_bar_arc_pct(scroll_pct);
    scui_ui_bar_arc_reset(SCUI_UI_SCENE_LIST_SCALE_RING);
}
