/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    // 使用插件连接器,加速滚动子控件渲染
    scui_plug_coupler_t *coupler;
    scui_ui_bar_arc_t    bar_arc;
} * scui_ui_res_local = NULL;

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
void scui_ui_scene_list_scale_scroll_notify_event(scui_event_t *event)
{
    scui_coord_t scroll_pct = 0;
    scui_scroll_auto_percent_get(event->object, &scroll_pct);
    SCUI_LOG_INFO("pct:%d", scroll_pct);
    scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
    scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_plug_coupler_widget_m_event_proc(scui_event_t *event)
{
    scui_plug_coupler_widget_m_event_proc(scui_ui_res_local->coupler, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_plug_coupler_widget_s_event_proc(scui_event_t *event)
{
    scui_plug_coupler_widget_s_event_proc(scui_ui_res_local->coupler, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_plug_coupler_recycle_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw: {
        
        if (scui_widget_event_check_finish(event)) {
            // 从控件树绘制结束,回收部分不使用的画布
            scui_plug_coupler_recycle(scui_ui_res_local->coupler, false);
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
static void scui_ui_scene_item_s_event_proc(scui_event_t *event)
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
        
        scui_area_t image_clip = scui_widget_clip(event->object);
        scui_handle_t image_icon = scui_ui_scene_list_image[scui_ui_res_local->coupler->list_draw_idx] + 3;
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
static void scui_ui_scene_item_m_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        scui_opt_pos_t pos = scui_opt_pos_c;
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
        
        scui_handle_t   index  = scui_ui_res_local->coupler->list_draw_idx;
        scui_handle_t  custom  = scui_ui_res_local->coupler->list_widget_s[index];
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
        scui_custom_draw_rect4(event, &btn_clip, btn_image_full, btn_color_full, -1);
        
        scui_image_t img_inst  = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        
        scui_handle_t image = scui_handle_find();
        scui_handle_set(image, &img_inst);
        scui_widget_draw_image_scale(event->object, NULL, image, NULL, img_scale, pos);
        scui_handle_set(image, NULL);
        
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
        scui_handle_t  custom = scui_ui_res_local->coupler->list_widget_s[index];
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
    case scui_event_anima_elapse:
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
            
            scui_plug_coupler_create(&scui_ui_res_local->coupler, scui_ui_scene_list_num);
            scui_ui_res_local->coupler->list_widget_m_cb = scui_ui_scene_item_m_event_proc;
        }
        
        if (scui_widget_event_check_prepare(event)) {
            
            scui_event_cb_node_t event_cb_node = {
                .event_cb = scui_ui_scene_plug_coupler_recycle_event_proc,
                .event = scui_event_draw,
            };
            scui_widget_event_add(SCUI_UI_SCENE_LIST_SCALE_SCROLL, &event_cb_node);
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
            custom_maker.widget.type        = scui_widget_type_custom;
            custom_maker.widget.style.trans = true;
            custom_maker.widget.style.sched_anima = true;
            custom_maker.widget.clip.w      = SCUI_DRV_HOR_RES;
            custom_maker.widget.parent      = SCUI_UI_SCENE_LIST_SCALE_SCROLL;
            
            custom_maker.widget.style.indev_ptr = false;
            custom_maker.widget.clip.h   = SCUI_DRV_VER_RES / 2 - 10 - 72 / 2;
            custom_maker.widget.event_cb = NULL;
            scui_custom_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.style.indev_ptr = true;
            custom_maker.widget.clip.h   = 72;
            custom_maker.widget.event_cb = scui_ui_scene_plug_coupler_widget_m_event_proc;
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                scui_custom_create(&custom_maker, &custom_handle, false);
                scui_ui_res_local->coupler->list_widget_m[idx] = custom_handle;
            }
            
            custom_maker.widget.style.indev_ptr = false;
            custom_maker.widget.clip.h   = SCUI_DRV_VER_RES / 2 - 10 - 72 / 2;
            custom_maker.widget.event_cb = NULL;
            scui_custom_create(&custom_maker, &custom_handle, false);
            
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                
                scui_coord_t scale_w = scui_image_w(scui_ui_scene_list_image[0] + 3);
                scui_coord_t scale_h = scui_image_h(scui_ui_scene_list_image[0] + 3);
                
                scui_custom_maker_t custom_maker = {0};
                scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
                custom_maker.widget.type        = scui_widget_type_custom;
                custom_maker.widget.style.trans = true;
                custom_maker.widget.clip.w      = SCUI_DRV_HOR_RES - 20 - 10;
                custom_maker.widget.clip.h      = scale_h;
                custom_maker.widget.child_num   = 1;
                custom_maker.widget.event_cb    = scui_ui_scene_item_s_event_proc;
                scui_custom_create(&custom_maker, &custom_handle, false);
                scui_ui_res_local->coupler->list_widget_s[idx] = custom_handle;
                
                scui_string_maker_t string_maker = {0};
                scui_handle_t string_handle             = SCUI_HANDLE_INVALID;
                string_maker.widget.type                = scui_widget_type_string;
                string_maker.widget.style.trans         = true;
                string_maker.widget.parent              = custom_handle;
                string_maker.widget.clip.x              = 52 + 8;
                string_maker.widget.clip.w              = custom_maker.widget.clip.w - (52 + 16 + 8 * 2);
                string_maker.widget.clip.h              = custom_maker.widget.clip.h;
                string_maker.args.align_hor             = 0;
                string_maker.args.align_ver             = 2;
                string_maker.args.color.color_s.full    = 0xFFFFFFFF;
                string_maker.args.color.color_e.full    = 0xFFFFFFFF;
                string_maker.args.color.filter          = true;
                string_maker.draw_cache                 = true;
                string_maker.font_idx                   = 1;
                string_maker.text                       = scui_ui_scene_list_text[idx];
                scui_string_create(&string_maker, &string_handle, false);
                
                scui_event_cb_node_t event_cb_node = {
                    .event_cb = scui_ui_scene_plug_coupler_widget_s_event_proc,
                    .event = scui_event_draw,
                };
                scui_widget_event_add(string_handle, &event_cb_node);
            }
            
            scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_LIST_SCALE_BAR_ARC;
            scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        if (scui_widget_event_check_finish(event)) {
            
            scui_plug_coupler_recycle(scui_ui_res_local->coupler, true);
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->coupler->list_num; idx++)
                scui_widget_hide(scui_ui_res_local->coupler->list_widget_s[idx], false);
            
            scui_plug_coupler_destroy(scui_ui_res_local->coupler);
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
    case scui_event_key_click:
        
        #if 0
        scui_plug_coupler_recycle(scui_ui_res_local->coupler, true);
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_del();
        scui_window_switch_type_cfg_set(type);
        scui_widget_event_mask_over(event);
        #endif
        break;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
