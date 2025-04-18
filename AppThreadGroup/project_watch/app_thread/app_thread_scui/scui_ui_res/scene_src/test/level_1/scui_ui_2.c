/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t     scroll;   // 滚动控件
    scui_ui_bar_arc_t bar_arc;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_2_bar_arc_event_proc(scui_event_t *event)
{
    scui_ui_bar_arc_event_proc(&scui_ui_res_local->bar_arc, event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_2_scroll_event(scui_event_t *event)
{
    // 转移至控件调度
    if (event->type < scui_event_widget_s ||
        event->type > scui_event_widget_e) {
        scui_widget_map_t *widget_map = NULL;
        scui_widget_map_find(scui_widget_type(event->object), &widget_map);
        if (widget_map->invoke != NULL)
            widget_map->invoke(event);
        
        return;
    }
    
    scui_coord_t scroll_pct = 0;
    scui_scroll_percent_get(event->object, &scroll_pct);
    SCUI_LOG_INFO("pct:%d", scroll_pct);
    scui_ui_res_local->bar_arc.bar_pct = scroll_pct;
    scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_2_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 滚动中心子控件 */
        #if 0
        scui_handle_t handle_scroll = SCUI_HANDLE_INVALID;
        static scui_handle_t handle_scroll_rcd = SCUI_HANDLE_INVALID;
        scui_scroll_center_target_get(scui_ui_res_local->scroll, &handle_scroll);
        if (handle_scroll_rcd != handle_scroll) {
            handle_scroll_rcd  = handle_scroll;
            if (handle_scroll != SCUI_HANDLE_INVALID)
                SCUI_LOG_WARN("widget center:%d", handle_scroll);
        }
        #endif
        break;
    }
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        #if 1
        if (scui_event_check_prepare(event)) {
            
            #define SCUI_SCROLL_LAYOUT_AUTO     1
            scui_scroll_maker_t scroll_maker = {0};
            scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
            scroll_maker.widget.type = scui_widget_type_scroll;
            scroll_maker.widget.style.sched_widget = true;
            scroll_maker.widget.style.indev_ptr = true;
            scroll_maker.widget.style.indev_enc = true;
            scroll_maker.widget.style.indev_key = true;
            scroll_maker.widget.clip.x = SCUI_DRV_HOR_RES / 6;
            scroll_maker.widget.clip.y = SCUI_DRV_VER_RES / 6;
            scroll_maker.widget.clip.w = SCUI_DRV_HOR_RES * 4 / 6;
            scroll_maker.widget.clip.h = SCUI_DRV_VER_RES * 4 / 6;
            scroll_maker.widget.parent = SCUI_UI_SCENE_2;
            scroll_maker.widget.child_num = 50;
            scroll_maker.widget.color.color.full = 0xFF4F4F4F;
            scroll_maker.widget.event_cb = scui_ui_scene_2_scroll_event;
            scroll_maker.springback = 70;
            scroll_maker.space = 50;
            // scroll_maker.loop = true;
            scroll_maker.fling_page = 5;
            scroll_maker.route_enc = 117;
            scroll_maker.route_key = 117;
            scroll_maker.keyid_fdir = SCUI_WIDGET_SCROLL_KEY_FDIR;
            scroll_maker.keyid_bdir = SCUI_WIDGET_SCROLL_KEY_BDIR;
            
            #if SCUI_SCROLL_LAYOUT_AUTO
            scroll_maker.pos = scui_opt_pos_c;
            // scroll_maker.pos = scui_opt_dir_hor;
            // scroll_maker.pos = scui_opt_dir_ver;
            // scroll_maker.dir = scui_opt_dir_hor;
            scroll_maker.dir = scui_opt_dir_ver;
            #else
            scroll_maker.pos = scui_opt_pos_c;
            // scroll_maker.pos = scui_opt_pos_l;
            // scroll_maker.pos = scui_opt_pos_r;
            // scroll_maker.pos = scui_opt_pos_u;
            // scroll_maker.pos = scui_opt_pos_d;
            scroll_maker.dir = scui_opt_dir_none;
            #endif
            scui_widget_create(&scroll_maker, &scroll_handle, false);
            scui_ui_res_local->scroll = scroll_handle;
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
            custom_maker.widget.type   = scui_widget_type_custom;
            custom_maker.widget.clip.w = SCUI_DRV_HOR_RES / 8;
            custom_maker.widget.clip.h = SCUI_DRV_VER_RES / 8;
            custom_maker.widget.parent = scroll_handle;
            
            #if SCUI_SCROLL_LAYOUT_AUTO
            // 自动布局模式
            for (uint8_t idx = 0; idx < 10; idx++) {
                custom_maker.widget.color.color.ch.a = 0xFF;
                custom_maker.widget.color.color.ch.r = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.g = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.b = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
                scui_widget_create(&custom_maker, &custom_handle, false);
            }
            #else
            scui_coord_t scroll_w = scroll_maker.widget.clip.w;
            scui_coord_t scroll_h = scroll_maker.widget.clip.h;
            scui_coord_t custom_w = custom_maker.widget.clip.w;
            scui_coord_t custom_h = custom_maker.widget.clip.h;
            // 自由布局模式
            for (uint8_t idx = 0; idx < 25; idx++) {
                custom_maker.widget.clip.x = scui_map(idx % 5, 0, 5, - custom_w, scroll_w + custom_w);
                custom_maker.widget.clip.y = scui_map(idx / 5, 0, 5, - custom_h, scroll_h + custom_h);
                SCUI_LOG_DEBUG("<x:%d,y:%d>", custom_maker.widget.clip.x, custom_maker.widget.clip.y);
                custom_maker.widget.color.color.ch.a = 0xFF;
                custom_maker.widget.color.color.ch.r = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.g = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.b = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
                scui_widget_create(&custom_maker, &custom_handle, false);
            }
            #endif
        }
        #endif
        
        scui_ui_res_local->bar_arc.bar_handle = SCUI_UI_SCENE_2_BAR_ARC;
        scui_ui_bar_arc_reset(&scui_ui_res_local->bar_arc);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        /* 界面数据转存回收 */
        if (scui_event_check_finish(event)) {
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
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        scui_ui_scene_popup_exec(SCUI_MULTI_LANG_0X002f, NULL);
        scui_event_mask_over(event);
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_2_ring_event_proc(scui_event_t *event)
{
    static scui_coord_t spinner_pct  = 0;
    static scui_coord_t spinner_cnt  = 0;
    static scui_coord_t spinner_tick = 1500 / SCUI_ANIMA_TICK;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        spinner_cnt++;
        if (spinner_cnt >  spinner_tick)
            spinner_cnt -= spinner_tick;
        
        spinner_pct = scui_map(spinner_cnt, 0, spinner_tick, 0, 100);
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        scui_handle_t image_edge = scui_image_prj_image_src_400X400px_dotbmp;
        scui_handle_t image_ring = scui_image_prj_image_src_400X400pxbmp;
        // scui_handle_t image_ring = scui_image_prj_image_src_400X400pxpng;
        
        scui_area_t clip = scui_widget_clip(event->object);
        clip.x += (clip.w - scui_image_w(image_ring)) / 2;
        clip.y += (clip.h - scui_image_h(image_ring)) / 2;
        clip.w = scui_image_w(image_ring);
        clip.h = scui_image_h(image_ring);
        
        // scui_color_t color_black = {0};
        // scui_widget_draw_color(event->object, &clip, color_black);
        
        scui_color_t color = {.filter = true,};
        scui_custom_draw_spinner(event, &clip, image_ring, color, image_edge,
                                 spinner_pct, 270, 60, +1);
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
