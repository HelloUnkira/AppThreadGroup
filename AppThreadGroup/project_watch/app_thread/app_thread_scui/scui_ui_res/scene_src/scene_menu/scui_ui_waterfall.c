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
void scui_ui_scene_waterfall_bar_arc_event(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_waterfall_scroll_notify_event(scui_event_t *event)
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
static void scui_ui_scene_waterfall_icon_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_ptr_click:
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(parent, event->object) - 1;
        SCUI_LOG_INFO("list_idx:%d", index);
        
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
        
        scui_handle_t image   = scui_ui_scene_list_image[index];
        scui_coord_t  ofs_min = SCUI_UI_WATERFALL_OFS_MIN;
        scui_coord_t  ofs_max = SCUI_UI_WATERFALL_OFS_MAX;
        scui_coord_t  ofs_cur = ofs_min;
        
        scui_area_t  scroll_c  = scui_widget_clip(SCUI_UI_SCENE_WATERFALL_SCROLL);
        scui_coord_t scroll_cx = scroll_c.x + scroll_c.w / 2;
        scui_coord_t scroll_cy = scroll_c.y + scroll_c.h / 2;
        
        scui_area_t  icon_c  = scui_widget_clip(event->object);
        scui_coord_t icon_cx = icon_c.x + icon_c.w / 2;
        scui_coord_t icon_cy = icon_c.y + icon_c.h / 2;
        scui_coord_t dist_cx = scui_dist(scroll_cx, icon_cx);
        scui_coord_t dist_cy = scui_dist(scroll_cy, icon_cy);
        scui_coord_t dist_cw = scroll_c.w / 2;
        scui_coord_t dist_ch = scroll_c.h / 2;
        
        #if 1
        if (dist_cy <= dist_ch)
            ofs_cur = scui_map(dist_cy, 0, dist_ch, ofs_max + 1, ofs_min - 1);
        // SCUI_LOG_WARN("ofs_cur:%d, dist_y:%d, dist_c:%d", ofs_cur, dist_y, dist_ch);
        #else
        scui_multi_t dist_x2 = dist_cx * dist_cx;
        scui_multi_t dist_y2 = dist_cy * dist_cy;
        scui_multi_t dist_w2 = dist_cw * dist_cw;
        scui_multi_t dist_h2 = dist_ch * dist_ch;
        scui_multi_t dist_cd = dist_x2 + dist_y2;
        scui_multi_t dist_cf = 0;
        scui_multi_t dist_md = dist_w2 + dist_h2;
        scui_multi_t dist_mf = 0;
        scui_sqrt(dist_cd, &dist_cd, &dist_cf, 0x8000);
        scui_sqrt(dist_md, &dist_md, &dist_mf, 0x8000);
        if (dist_cd <= dist_md)
            ofs_cur = scui_map(dist_cd, 0, dist_md, ofs_max + 1, ofs_min - 1);
        SCUI_LOG_WARN("ofs_cur:%d, dist_cd:%d, dist_md:%d", ofs_cur, dist_cd, dist_md);
        #endif
        
        if (ofs_cur < ofs_min)
            ofs_cur = ofs_min;
        if (ofs_cur > ofs_max)
            ofs_cur = ofs_max;
        
        image += ofs_cur;
        
        // 三角函数:
        // sin_a = dist_y / rad_rr;
        // cos_a = 1 - sin_a * sin_a
        // dis_x = rad_rr - rad_rr * cos_a
        scui_multi_t rad_rr = scroll_c.w / 2 - scui_image_h(image) / 2;
        scui_multi_t dist_y = scui_min(rad_rr, scui_dist(icon_cy, scroll_cy));
        
        scui_multi_t cos_a2 = (1024 * 1024) - (1024 * dist_y / rad_rr) * (1024 * dist_y / rad_rr);
        scui_multi_t cos_ia = 0;
        scui_multi_t cos_fa = 0;
        scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
        scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
        SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
        dist_x = scui_min(dist_x, icon_c.w - scui_image_w(image));
        
        scui_alpha_t alpha = scui_map(dist_y, 0, rad_rr, scui_alpha_pct100, scui_alpha_pct0);
        scui_widget_alpha_set(event->object, alpha, true);
        
        if (type == 0)
            icon_c.x += dist_x;
        if (type == 1)
            icon_c.x += (icon_c.w - scui_image_w(image)) / 2;
        if (type == 2)
            icon_c.x += (icon_c.w - scui_image_w(image)) - dist_x;
        
        icon_c.y += (icon_c.h - scui_image_h(image)) / 2;
        icon_c.h  = scui_image_h(image);
        icon_c.w  = scui_image_w(image);
        
        if (event->type == scui_event_draw)
            scui_widget_draw_image(event->object, &icon_c, image, NULL, (scui_color_t){0});
        
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
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_waterfall_event_proc(scui_event_t *event)
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
            
            // 这里画个圈,校验测试使用
            scui_widget_image_set(SCUI_UI_SCENE_WATERFALL_SCROLL,
            scui_image_prj_image_src_standby_watch_D10606001_bg_01_2bmp);
            
            scui_coord_t scroll_w = scui_widget_clip(SCUI_UI_SCENE_WATERFALL_SCROLL).w;
            scui_coord_t scroll_h = scui_widget_clip(SCUI_UI_SCENE_WATERFALL_SCROLL).w;
            
            // 取一张图(随便, 反正所有图都一样)
            scui_handle_t icon = scui_ui_scene_list_image[0];
            icon += SCUI_UI_WATERFALL_OFS_MAX;
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
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
            custom_maker.widget.type        = scui_widget_type_custom;
            custom_maker.widget.style.trans = true;
            custom_maker.widget.parent      = SCUI_UI_SCENE_WATERFALL_SCROLL;
            
            /* 上半部分空白 */
            custom_maker.widget.style.indev_ptr = false;
            custom_maker.widget.event_cb        = NULL;
            custom_maker.widget.clip.x = 0;
            custom_maker.widget.clip.y = 0;
            custom_maker.widget.clip.w = scroll_w;
            custom_maker.widget.clip.h = (scroll_h - icon_h) / 2;
            scui_widget_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.style.indev_ptr = true;
            custom_maker.widget.event_cb        = scui_ui_scene_waterfall_icon_event_proc;
            
            for (uint8_t idx = 0; idx < scui_ui_scene_list_num; idx++) {
                // 余下一个,填中间
                if (scui_ui_scene_list_num % 3 == 1 &&
                    idx == scui_ui_scene_list_num - 1) {
                    custom_maker.widget.clip = clip_m;
                    scui_widget_create(&custom_maker, &custom_handle, false);
                    clip_m.y += clip_m.h + SCUI_UI_WATERFALL_LINE_SPACE;
                    continue;
                }
                // 余下俩个,填俩边
                if (scui_ui_scene_list_num % 3 == 2 &&
                    idx == scui_ui_scene_list_num - 2) {
                    custom_maker.widget.clip = clip_l;
                    scui_widget_create(&custom_maker, &custom_handle, false);
                    clip_l.y += clip_l.h + SCUI_UI_WATERFALL_LINE_SPACE;
                    continue;
                }
                if (scui_ui_scene_list_num % 3 == 2 &&
                    idx == scui_ui_scene_list_num - 1) {
                    custom_maker.widget.clip = clip_r;
                    scui_widget_create(&custom_maker, &custom_handle, false);
                    clip_r.y += clip_r.h + SCUI_UI_WATERFALL_LINE_SPACE;
                    continue;
                }
                // 按顺序填充三列
                if (idx % 3 == 0) {
                    custom_maker.widget.clip = clip_l;
                    scui_widget_create(&custom_maker, &custom_handle, false);
                    clip_l.y += clip_l.h + SCUI_UI_WATERFALL_LINE_SPACE;
                }
                if (idx % 3 == 1) {
                    custom_maker.widget.clip = clip_m;
                    scui_widget_create(&custom_maker, &custom_handle, false);
                    clip_m.y += clip_m.h + SCUI_UI_WATERFALL_LINE_SPACE;
                }
                if (idx % 3 == 2) {
                    custom_maker.widget.clip = clip_r;
                    scui_widget_create(&custom_maker, &custom_handle, false);
                    clip_r.y += clip_r.h + SCUI_UI_WATERFALL_LINE_SPACE;
                }
            }
            
            clip_l.y -= SCUI_UI_WATERFALL_LINE_SPACE;
            clip_m.y -= SCUI_UI_WATERFALL_LINE_SPACE;
            clip_r.y -= SCUI_UI_WATERFALL_LINE_SPACE;
            
            /* 下半部分空白 */
            custom_maker.widget.style.indev_ptr = false;
            custom_maker.widget.event_cb        = NULL;
            custom_maker.widget.clip.x = 0;
            custom_maker.widget.clip.y = scui_max(clip_m.y, scui_max(clip_l.y, clip_r.y));
            custom_maker.widget.clip.w = scroll_w;
            custom_maker.widget.clip.h = (scroll_h - icon_h) / 2;
            scui_widget_create(&custom_maker, &custom_handle, false);
            
            scui_scroll_layout(SCUI_UI_SCENE_WATERFALL_SCROLL);
            scui_widget_draw(SCUI_UI_SCENE_WATERFALL, NULL, false);
            
            scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_WATERFALL_BAR_ARC;
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
