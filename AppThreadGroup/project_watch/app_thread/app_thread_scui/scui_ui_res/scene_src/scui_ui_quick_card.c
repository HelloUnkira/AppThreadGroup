/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t linear;
    scui_handle_t list_num;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_item_s_event_proc(scui_event_t *event)
{
    // 特殊的固定调用
    scui_widget_event_shift(event);
    
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
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        /* 绘制背景 */
        scui_area_t clip = scui_widget_clip(event->object);
        scui_widget_draw_color(event->object, &clip, SCUI_COLOR_MAKE32(false, 0x0, 0xFF404040));
        
        
        
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
    scui_widget_event_shift(event);
    
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
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        
        
        
        scui_multi_t percent = 100;
        scui_area_t  clip_p  = scui_widget_clip(scui_widget_parent(event->object));
        scui_area_t  clip_w  = scui_widget_clip(event->object);
        if ((clip_p.y + clip_p.h / 2 < clip_w.y + clip_w.h / 2)) {
            // 计算当前控件中心到父控件中心距离
            scui_point_t offset = {0};
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
        
        
        
        scui_area_t   btn_clip = clip_w;
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
        
        scui_event_mask_over(event);
        scui_handle_t parent = scui_widget_parent(event->object);
        scui_handle_t index  = scui_widget_child_to_index(parent, event->object) - 1;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(scui_ui_res_local->linear, &linear_item);
        
        scui_handle_t  custom = linear_item.handle_m;
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
        
        scui_linear_m_maker_t linear_m_maker = {0};
        scui_linear_s_maker_t linear_s_maker = {0};
        scui_handle_t linear_m_handle = SCUI_HANDLE_INVALID;
        scui_handle_t linear_s_handle = SCUI_HANDLE_INVALID;
        
        // 通用属性配置
        linear_m_maker.widget.type              = scui_widget_type_linear_m;
        linear_m_maker.widget.style.sched_anima = true;
        linear_m_maker.widget.clip.w            = SCUI_HOR_RES;
        linear_m_maker.widget.parent            = event->object;
        linear_s_maker.widget.type              = scui_widget_type_linear_s;
        linear_s_maker.widget.clip.w            = 410;
        linear_s_maker.widget.clip.h            = 180;
        linear_s_maker.widget.child_num         = 5;
        linear_s_maker.widget.event_cb          = scui_ui_scene_item_s_event_proc;
        
        // 上半部分留白占用
        linear_m_maker.widget.style.indev_ptr = false;
        linear_m_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 180 / 2;
        linear_m_maker.widget.event_cb        = NULL;
        scui_widget_create(&linear_m_maker, &linear_m_handle);
        
        // list的各个子控件
        linear_m_maker.widget.style.indev_ptr = true;
        linear_m_maker.widget.clip.h          = 180;
        linear_m_maker.widget.event_cb        = scui_ui_scene_item_m_event_proc;
        
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
            // 创建子控件(主)(从)
            scui_widget_create(&linear_m_maker, &linear_m_handle);
            linear_s_maker.handle_m = linear_m_handle;
            scui_widget_create(&linear_s_maker, &linear_s_handle);
            // 绑定子控件(主)(从)
            scui_linear_item_t linear_item = {.draw_idx = idx,};
            linear_item.handle_m = linear_m_handle;
            linear_item.handle_s = linear_s_handle;
            scui_linear_item_sets(event->object, &linear_item);
        }
        
        // 下半部分留白占用
        linear_m_maker.widget.style.indev_ptr = false;
        linear_m_maker.widget.clip.h          = SCUI_VER_RES / 2 - 10 - 180 / 2;
        linear_m_maker.widget.event_cb        = NULL;
        scui_widget_create(&linear_m_maker, &linear_m_handle);
        
        // list的各个子控件树
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
            scui_linear_item_t linear_item = {.draw_idx = idx,};
            scui_linear_item_gets(event->object, &linear_item);
            linear_s_handle = linear_item.handle_s;
        }
        
        scui_ui_res_local->linear = event->object;
        break;
    }
    case scui_event_widget_scroll_layout:
    case scui_event_widget_scroll_start:
    case scui_event_widget_scroll_keep:
    case scui_event_widget_scroll_over: {
        
        scui_area_t   clip_p = scui_widget_clip(event->object);
        scui_handle_t list_m = scui_widget_child_num(event->object);
        scui_handle_t handle_t = SCUI_HANDLE_INVALID;
        
        #if 0
        // 寻找到的最后一个完全可显示控件
        for (scui_handle_t idx = 0; idx < list_m; idx++) {
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
        scui_handle_t index = scui_widget_child_to_index(event->object, handle_t);
        for (scui_handle_t idx = index + 1; idx < list_m; idx++) {
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
    
    
    
    // 转移至控件调度
    scui_widget_event_shift(event);
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
        
        scui_linear_maker_t linear_maker = {0};
        scui_handle_t linear_handle = SCUI_HANDLE_INVALID;
        linear_maker.widget.type = scui_widget_type_linear;
        linear_maker.widget.style.order_draw   = true;
        linear_maker.widget.style.sched_widget = true;
        linear_maker.widget.style.indev_ptr    = true;
        linear_maker.widget.style.indev_enc    = true;
        linear_maker.widget.style.indev_key    = true;
        linear_maker.widget.clip.w = SCUI_HOR_RES;
        linear_maker.widget.clip.h = SCUI_VER_RES;
        linear_maker.widget.parent = event->object;
        linear_maker.widget.child_num  = 50;
        linear_maker.widget.event_cb   = scui_ui_scene_quick_card_scroll_event;
        linear_maker.scroll.pos        = scui_opt_pos_c;
        linear_maker.scroll.dir        = scui_opt_dir_ver;
        linear_maker.scroll.skip       = scui_opt_pos_all;
        linear_maker.scroll.space      = 3;
        linear_maker.scroll.route_enc  = 117;
        linear_maker.scroll.route_key  = 117;
        linear_maker.scroll.keyid_fdir = SCUI_WIDGET_SCROLL_KEY_FDIR;
        linear_maker.scroll.keyid_bdir = SCUI_WIDGET_SCROLL_KEY_BDIR;
        linear_maker.scroll.springback = 70;
        linear_maker.list_num          = scui_ui_res_local->list_num;
        scui_widget_create(&linear_maker, &linear_handle);
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        // 这里画个圈,校验测试使用
        #if 0
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += 4; clip.y += 4;
        scui_widget_draw_image(event->object, &clip,
        scui_image_prj_image_src_03_activity_ring_big_max_03_bj_01bmp,
        NULL, SCUI_COLOR_MAKE32(true, 0xFF000000, 0xFF404040));
        #endif
        
        break;
    }
    default:
        break;
    }
}
