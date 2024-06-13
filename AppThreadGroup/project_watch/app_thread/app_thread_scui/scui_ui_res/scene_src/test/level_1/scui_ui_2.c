/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static inline void scui_ui_scene_window_float_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = SCUI_UI_SCENE_2;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    scui_window_float_cfg_set(&float_cfg);
}

static inline void scui_ui_scene_window_list_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(SCUI_UI_SCENE_2, &window_cfg);
    window_cfg.sibling[0] = SCUI_UI_SCENE_LANTERN;
    window_cfg.sibling[1] = SCUI_UI_SCENE_SOCCER;
    window_cfg.sibling[2] = SCUI_UI_SCENE_1;
    window_cfg.sibling[3] = SCUI_UI_SCENE_3;
    scui_window_cfg_set(SCUI_UI_SCENE_2, &window_cfg);
}

static struct {
    scui_handle_t scroll;           // 滚动控件
    scui_coord_t  scroll_pct;       // 滚动偏移量
    scui_coord_t  bar_wait;         // 暂停时间
    scui_alpha_t  bar_alpha;        // 淡出透明度
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_2_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (scui_ui_res_local->bar_wait  < SCUI_UI_SCROLL_BAR_STOP_TIME)
            scui_ui_res_local->bar_wait += SCUI_ANIMA_TICK;
        else
        if (scui_ui_res_local->bar_alpha > 0) {
            scui_alpha_t tick = scui_alpha_cover / (SCUI_UI_SCROLL_BAR_FADE_TIME / SCUI_ANIMA_TICK);
            if (scui_ui_res_local->bar_alpha  > tick)
                scui_ui_res_local->bar_alpha -= tick;
            else
                scui_ui_res_local->bar_alpha  = 0;
            
            scui_widget_alpha_set(SCUI_UI_SCENE_2_RING, scui_ui_res_local->bar_alpha, false);
            scui_widget_draw(SCUI_UI_SCENE_2_RING, NULL, false);
        }
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        if (scui_widget_event_check_prepare(event)) {
            
            #define SCUI_SCROLL_LAYOUT_AUTO     1
            scui_scroll_maker_t scroll_maker = {0};
            scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
            scroll_maker.widget.type = scui_widget_type_scroll;
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
            scroll_maker.springback = 70;
            scroll_maker.space = 50;
            // scroll_maker.loop = true;
            scroll_maker.fling_page = 5;
            scroll_maker.route_enc = 117;
            scroll_maker.route_key = 117;
            scroll_maker.keyid_fdir = scui_event_key_val_down;
            scroll_maker.keyid_bdir = scui_event_key_val_up;
            
            #if SCUI_SCROLL_LAYOUT_AUTO
            // scroll_maker.dir = scui_event_dir_hor;
            scroll_maker.dir = scui_event_dir_ver;
            scroll_maker.pos = scui_event_pos_c;
            // scroll_maker.pos = scui_event_dir_hor;
            // scroll_maker.pos = scui_event_dir_ver;
            #else
            scroll_maker.dir = scui_event_dir_none;
            scroll_maker.pos = scui_event_pos_c;
            // scroll_maker.pos = scui_event_pos_l;
            // scroll_maker.pos = scui_event_pos_r;
            // scroll_maker.pos = scui_event_pos_u;
            // scroll_maker.pos = scui_event_pos_d;
            #endif
            scui_scroll_create(&scroll_maker, &scroll_handle, false);
            scui_ui_res_local->scroll = scroll_handle;
            
            scui_custom_maker_t custom_maker = {0};
            scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
            custom_maker.widget.type   = scui_widget_type_custom;
            custom_maker.widget.clip.w = SCUI_DRV_HOR_RES / 8;
            custom_maker.widget.clip.h = SCUI_DRV_VER_RES / 8;
            custom_maker.widget.parent = scroll_handle;
            
            scui_coord_t scroll_x = scroll_maker.widget.clip.x;
            scui_coord_t scroll_y = scroll_maker.widget.clip.y;
            scui_coord_t scroll_w = scroll_maker.widget.clip.w;
            scui_coord_t scroll_h = scroll_maker.widget.clip.h;
            scui_coord_t custom_w = custom_maker.widget.clip.w;
            scui_coord_t custom_h = custom_maker.widget.clip.h;
            
            #if SCUI_SCROLL_LAYOUT_AUTO
            // 自动布局模式
            for (uint8_t idx = 0; idx < 10; idx++) {
                custom_maker.widget.color.color.ch.a = 0xFF;
                custom_maker.widget.color.color.ch.r = scui_rand(0, 10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.g = scui_rand(0, 10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.b = scui_rand(0, 10) % 2 == 0 ? 0 : 0xFF;
                scui_custom_create(&custom_maker, &custom_handle, false);
            }
            #else
            // 自由布局模式
            for (uint8_t idx = 0; idx < 25; idx++) {
                custom_maker.widget.clip.x = scui_map(idx % 5, 0, 5, - custom_w, scroll_w + custom_w);
                custom_maker.widget.clip.y = scui_map(idx / 5, 0, 5, - custom_h, scroll_h + custom_h);
                SCUI_LOG_DEBUG("<x:%d,y:%d>", custom_maker.widget.clip.x, custom_maker.widget.clip.y);
                custom_maker.widget.color.color.ch.a = 0xFF;
                custom_maker.widget.color.color.ch.r = scui_rand(0, 10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.g = scui_rand(0, 10) % 2 == 0 ? 0 : 0xFF;
                custom_maker.widget.color.color.ch.b = scui_rand(0, 10) % 2 == 0 ? 0 : 0xFF;
                scui_custom_create(&custom_maker, &custom_handle, false);
            }
            #endif
        }
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_get:
        scui_ui_scene_window_float_cfg(event);
        scui_ui_scene_window_list_cfg(event);
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_key_click:
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_widget_scroll_s:
        if (!scui_widget_event_check_execute(event))
             break;
        SCUI_LOG_INFO("scui_event_widget_scroll_s");
        scui_ui_res_local->bar_wait  = 0;
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        scui_widget_alpha_set(SCUI_UI_SCENE_2_RING, scui_ui_res_local->bar_alpha, false);
        scui_scroll_auto_percent_get(scui_ui_res_local->scroll, &scui_ui_res_local->scroll_pct);
        SCUI_LOG_INFO("pct:%d", scui_ui_res_local->scroll_pct);
        scui_widget_draw(SCUI_UI_SCENE_2_RING, NULL, false);
        // scui_widget_clip_check(event->object, true);
        
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_widget_scroll_c:
        if (!scui_widget_event_check_execute(event))
             break;
        SCUI_LOG_INFO("scui_event_widget_scroll_c");
        scui_ui_res_local->bar_wait  = 0;
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        scui_widget_alpha_set(SCUI_UI_SCENE_2_RING, scui_ui_res_local->bar_alpha, false);
        scui_scroll_auto_percent_get(scui_ui_res_local->scroll, &scui_ui_res_local->scroll_pct);
        SCUI_LOG_INFO("pct:%d", scui_ui_res_local->scroll_pct);
        scui_widget_draw(SCUI_UI_SCENE_2_RING, NULL, false);
        // scui_widget_clip_check(event->object, true);
        
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_widget_scroll_e:
        if (!scui_widget_event_check_execute(event))
             break;
        SCUI_LOG_INFO("scui_event_widget_scroll_e");
        scui_ui_res_local->bar_wait  = 0;
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        scui_widget_alpha_set(SCUI_UI_SCENE_2_RING, scui_ui_res_local->bar_alpha, false);
        scui_scroll_auto_percent_get(scui_ui_res_local->scroll, &scui_ui_res_local->scroll_pct);
        SCUI_LOG_INFO("pct:%d", scui_ui_res_local->scroll_pct);
        scui_widget_draw(SCUI_UI_SCENE_2_RING, NULL, false);
        // scui_widget_clip_check(event->object, true);
        
        scui_widget_event_mask_keep(event);
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
void scui_ui_scene_2_ring_event_proc(scui_event_t *event)
{
    static scui_coord_t pct = 0;
    static scui_coord_t way = +1;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        
        if (!scui_widget_event_check_execute(event))
             return;
        
        scui_area_t   clip = {0};
        scui_color_t  color_black = {0};
        scui_color_t  color_white = {.filter = true,.color.full = 0xFFFFFFFF,};
        scui_handle_t image_bg   = scui_image_prj_image_src_repeat_slider_04_bgpng;     // 44 * 236
        scui_handle_t image_ring = scui_image_prj_image_src_repeat_slider_03_ringbmp;
        scui_handle_t image_edge = scui_image_prj_image_src_repeat_slider_02_dotbmp;
        
        clip.x = (SCUI_DRV_HOR_RES - 44);
        clip.y = (SCUI_DRV_VER_RES - 236) / 2;
        clip.w =  44;
        clip.h = 236;
        scui_widget_surface_draw_image(event->object, &clip, image_bg, NULL, color_black);
        
        clip.x = (466 - 462) / 2;
        clip.y = (466 - 462) / 2;
        clip.w = 462;
        clip.h = 462;
        
        scui_coord_t angle_d = 29;
        scui_coord_t angle_s = -29 + scui_ui_res_local->scroll_pct * (angle_d) / 100;
        scui_coord_t angle_e = angle_s + angle_d;
        SCUI_LOG_INFO("angle:<s:%d,e:%d>", angle_s, angle_e);
        scui_widget_surface_draw_ring(event->object, &clip, image_ring, NULL, angle_s, color_white, angle_e, 100, image_edge);
        
        scui_widget_event_mask_keep(event);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
