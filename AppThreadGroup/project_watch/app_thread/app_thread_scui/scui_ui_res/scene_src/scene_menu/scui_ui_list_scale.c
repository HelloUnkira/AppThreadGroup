/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t scroll_pct;        // 滚动偏移量
    scui_coord_t bar_wait;          // 暂停时间
    scui_alpha_t bar_alpha;         // 淡出透明度
    
    scui_handle_t    list_num;
    scui_handle_t   *list_image;
    scui_handle_t   *list_text;
    scui_handle_t   *list_jump;
    uint8_t        **list_pixel;    // 绘制画布列表
    
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
    SCUI_ASSERT(event->type == scui_event_draw);
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    if (scui_widget_event_check_finish(event)) {
        // scui_widget_clip_check(scui_widget_root(event->object), true);
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
            if (scui_ui_res_local->list_draw[idx]) {
                scui_ui_res_local->list_draw[idx] = false;
                continue;
            }
            
            scui_widget_surface_destroy(&(scui_widget_t){
                .surface = scui_ui_res_local->list_surface[idx],});
            scui_ui_res_local->list_surface[idx] = NULL;
        }
    }
    
    scui_scroll_event(event);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
static void scui_ui_scene_string_draw_proc(scui_event_t *event)
{
    SCUI_ASSERT(event->type == scui_event_draw);
    scui_widget_t *widget = scui_handle_get(event->object);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->surface == NULL) {
        scui_handle_t handle = scui_widget_root(event->object);
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
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
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_area_t   btn_clip = scui_widget_draw_clip(event->object);
        scui_color_t  btn_color_full = {.color.full = 0xFF282828,};
        scui_handle_t btn_image_full[4] = {
            scui_image_prj_image_src_repeat_card_04_r36_1bmp,
            scui_image_prj_image_src_repeat_card_05_r36_2bmp,
            scui_image_prj_image_src_repeat_card_06_r36_3bmp,
            scui_image_prj_image_src_repeat_card_07_r36_4bmp,
        };
        scui_custom_draw_rect4(event, &btn_clip, btn_image_full, btn_color_full, -1);
        
        scui_area_t   image_clip = scui_widget_draw_clip(event->object);
        scui_handle_t image_icon = scui_ui_res_local->list_image[scui_ui_res_local->list_idx];
        scui_image_t *image_inst = scui_handle_get(image_icon);
        SCUI_ASSERT(image_inst != NULL);
        image_clip.x += 30;
        image_clip.y += (image_clip.h - image_inst->pixel.height) / 2;
        scui_widget_draw_image(event->object, &image_clip, image_icon, NULL, (scui_color_t){0});
        
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
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_handle_t  parent = scui_widget_parent(event->object);
        scui_handle_t  index  = scui_widget_child_to_index(parent, event->object) - 1;
        scui_handle_t  custom = scui_ui_res_local->list_widget[index];
        scui_widget_t *widget = NULL;
        SCUI_LOG_INFO("list_idx:%d", index);
        
        scui_ui_res_local->list_idx = index;
        widget = scui_handle_get(event->object);
        SCUI_ASSERT(widget != NULL);
        /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
        if (scui_clip_empty(&widget->clip_set))
            break;
        
        widget = scui_handle_get(custom);
        SCUI_ASSERT(widget != NULL);
        
        // 创建一个独立临时的子画布,将目标绘制到一个独立子画布中
        if (scui_ui_res_local->list_surface[index] == NULL) {
            scui_widget_surface_create(widget, SCUI_PIXEL_CF_DEF_A, widget->clip.w, widget->clip.h);
            scui_ui_res_local->list_surface[index] = widget->surface;
            scui_ui_res_local->list_refr[index] = true;
        }
        scui_widget_surface_remap(widget, scui_ui_res_local->list_surface[index]);
        
        if (scui_ui_res_local->list_refr[index]) {
            scui_ui_res_local->list_refr[index] = false;
            scui_widget_show(custom, false);
            scui_widget_draw(custom, NULL, true);
        }
        scui_ui_res_local->list_draw[index] = true;
        
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        {   // 计算当前控件中心到父控件中心距离
            scui_widget_t *parent = scui_handle_get(SCUI_UI_SCENE_LIST_SCALE);
            scui_widget_t *child  = scui_handle_get(event->object);
            SCUI_ASSERT(parent != NULL && child != NULL);
            offset.x = scui_dist(parent->clip.x + parent->clip.w / 2, child->clip.x + child->clip.w / 2),
            offset.y = scui_dist(parent->clip.y + parent->clip.h / 2, child->clip.y + child->clip.h / 2),
            percent = (scui_multi_t)offset.y * 100 / parent->clip.h;
            percent = scui_min(percent, 100);
            SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        }
        
        scui_area_t  dst_clip  = scui_widget_draw_clip(event->object);
        scui_point_t img_scale = {
            .x = 1024 * (100 - percent) / 100,
            .y = 1024, // * (100 - percent) / 100,
        };
        scui_image_t img_inst  = {
            .status         = scui_image_status_mem,
            .pixel.width    = widget->clip.w,
            .pixel.height   = widget->clip.h,
            .pixel.data_mem = widget->surface->pixel,
        };
        scui_image_cf_by_pixel_cf(&img_inst.format, &widget->surface->format);
        scui_handle_t image = scui_handle_find();
        scui_handle_set(image, &img_inst);
        scui_widget_draw_image_scale(event->object, &dst_clip, image, NULL, img_scale, 1);
        // scui_widget_draw_image(event->object, &dst_clip, image, NULL, (scui_color_t){0});
        scui_handle_set(image, NULL);
        
        scui_widget_surface_remap(widget, NULL);
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
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
            event->object = scui_ui_res_local->list_widget[idx];
            scui_widget_event_dispatch(event);
        }
        event->object = handle;
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        /* 滚动中心子控件 */
        scui_handle_t handle_scroll = SCUI_HANDLE_INVALID;
        static scui_handle_t handle_scroll_rcd = SCUI_HANDLE_INVALID;
        scui_scroll_center_target(SCUI_UI_SCENE_LIST_SCALE_SCROLL, &handle_scroll);
        if (handle_scroll_rcd != handle_scroll) {
            handle_scroll_rcd  = handle_scroll;
            if (handle_scroll != SCUI_HANDLE_INVALID)
                SCUI_LOG_INFO("widget center:%d", handle_scroll);
        }
        
        if (scui_ui_res_local->bar_wait  < SCUI_UI_SCROLL_BAR_STOP_TIME)
            scui_ui_res_local->bar_wait += SCUI_ANIMA_TICK;
        else
        if (scui_ui_res_local->bar_alpha > 0) {
            scui_alpha_t tick = scui_alpha_cover / (SCUI_UI_SCROLL_BAR_FADE_TIME / SCUI_ANIMA_TICK);
            if (scui_ui_res_local->bar_alpha  > tick)
                scui_ui_res_local->bar_alpha -= tick;
            else
                scui_ui_res_local->bar_alpha  = 0;
            
            scui_widget_alpha_set(SCUI_UI_SCENE_LIST_SCALE_RING, scui_ui_res_local->bar_alpha, false);
            scui_widget_draw(SCUI_UI_SCENE_LIST_SCALE_RING, NULL, false);
        }
        break;
    }
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_widget_event_mask_keep(event);
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
            
            SCUI_ASSERT(scui_ui_scene_list_num != 0);
            scui_ui_res_local->list_num     = scui_ui_scene_list_num;
            scui_ui_res_local->list_image   = scui_ui_scene_list_image;
            scui_ui_res_local->list_text    = scui_ui_scene_list_text;
            scui_ui_res_local->list_jump    = scui_ui_scene_list_jump;
            scui_handle_t list_num = scui_ui_res_local->list_num;
            
            scui_ui_res_local->list_pixel   = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(uint8_t *));
            scui_ui_res_local->list_widget  = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(scui_handle_t));
            scui_ui_res_local->list_surface = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(scui_surface_t *));
            scui_ui_res_local->list_refr    = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(bool));
            scui_ui_res_local->list_draw    = SCUI_MEM_ALLOC(scui_mem_type_mix, list_num * sizeof(bool));
            
            memset(scui_ui_res_local->list_pixel,   0, list_num * sizeof(uint8_t *));
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
            
            custom_maker.widget.clip.h   = SCUI_DRV_VER_RES / 2 - 10 - 72 / 2;
            custom_maker.widget.event_cb = NULL;
            scui_custom_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.clip.h   = 72;
            custom_maker.widget.event_cb = scui_ui_scene_item_scale_event_proc;
            for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++)
                scui_custom_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.clip.h   = SCUI_DRV_VER_RES / 2 - 10 - 72 / 2;
            custom_maker.widget.event_cb = NULL;
            scui_custom_create(&custom_maker, &custom_handle, false);
            
            custom_maker.widget.clip.h   = 72;
            scui_area_t clip = custom_maker.widget.clip;
            for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
                
                scui_custom_maker_t custom_maker = {0};
                scui_handle_t custom_handle     = SCUI_HANDLE_INVALID;
                custom_maker.widget.type        = scui_widget_type_custom;
                custom_maker.widget.style.trans = true;
                custom_maker.widget.clip.w      = clip.w;
                custom_maker.widget.clip.h      = clip.h;
                custom_maker.widget.child_num   = 2;
                custom_maker.widget.event_cb    = scui_ui_scene_item_event_proc;
                scui_custom_create(&custom_maker, &custom_handle, false);
                scui_ui_res_local->list_widget[idx] = custom_handle;
                
                scui_string_maker_t string_maker = {0};
                scui_handle_t string_handle = SCUI_HANDLE_INVALID;
                string_maker.widget.type    = scui_widget_type_string;
                string_maker.widget.style.trans = true;
                string_maker.widget.parent  = custom_handle;
                string_maker.args.margin    = SCUI_STRING_MARGIN;
                string_maker.args.gap_none  = SCUI_STRING_SPACE_WIDTH;
                string_maker.args.color.color_s.full = 0xFFFFFFFF;
                string_maker.args.color.color_e.full = 0xFFFFFFFF;
                string_maker.args.color.filter = true;
                string_maker.widget.clip.x  = 100;
                string_maker.widget.clip.w  = SCUI_DRV_HOR_RES - 150;
                string_maker.widget.clip.y  = (clip.h - 40) / 2;
                string_maker.widget.clip.h  = 40;
                string_maker.name           = SCUI_FONT_TYPE_32_EN,
                string_maker.text           = scui_ui_res_local->list_text[idx];
                scui_string_create(&string_maker, &string_handle, false);
                
                scui_event_cb_node_t event_cb_node = {
                    .event_cb = scui_ui_scene_string_draw_proc,
                    .event = scui_event_draw,
                };
                scui_widget_event_add(string_handle, &event_cb_node);
                
                scui_widget_t *widget = scui_handle_get(custom_handle);
                SCUI_ASSERT(widget != NULL);
                scui_widget_surface_remap(widget, NULL);
            }
            
            scui_image_cache_clear();
            scui_ui_res_local->bar_wait  = 0;
            scui_ui_res_local->bar_alpha = scui_alpha_cover;
            scui_widget_alpha_set(SCUI_UI_SCENE_LIST_SCALE_RING, scui_ui_res_local->bar_alpha, false);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_widget_event_mask_keep(event);
        
        if (scui_widget_event_check_finish(event)) {
            for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
                scui_widget_hide(scui_ui_res_local->list_widget[idx], false);
                scui_ui_res_local->list_widget[idx] = SCUI_HANDLE_INVALID;
                
                scui_widget_surface_destroy(&(scui_widget_t){
                    .surface = scui_ui_res_local->list_surface[idx],});
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
            SCUI_MEM_FREE(scui_ui_res_local->list_pixel);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_key_click:
        
        for (uint8_t idx = 0; idx < scui_ui_res_local->list_num; idx++) {
            scui_widget_surface_destroy(&(scui_widget_t){
                .surface = scui_ui_res_local->list_surface[idx],});
            scui_ui_res_local->list_surface[idx] = NULL;
        }
        
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_del();
        scui_window_switch_type_cfg_set(type);
        scui_widget_event_mask_over(event);
        break;
    case scui_event_widget_scroll_s:
        SCUI_LOG_INFO("scui_event_widget_scroll_s");
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_ui_res_local->bar_wait  = 0;
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        scui_widget_alpha_set(SCUI_UI_SCENE_LIST_SCALE_RING, scui_ui_res_local->bar_alpha, false);
        scui_scroll_auto_percent_get(SCUI_UI_SCENE_LIST_SCALE_SCROLL, &scui_ui_res_local->scroll_pct);
        SCUI_LOG_INFO("pct:%d", scui_ui_res_local->scroll_pct);
        scui_widget_draw(SCUI_UI_SCENE_LIST_SCALE_RING, NULL, false);
        // scui_widget_clip_check(event->object, true);
        break;
    case scui_event_widget_scroll_c:
        SCUI_LOG_INFO("scui_event_widget_scroll_c");
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_ui_res_local->bar_wait  = 0;
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        scui_widget_alpha_set(SCUI_UI_SCENE_LIST_SCALE_RING, scui_ui_res_local->bar_alpha, false);
        scui_scroll_auto_percent_get(SCUI_UI_SCENE_LIST_SCALE_SCROLL, &scui_ui_res_local->scroll_pct);
        SCUI_LOG_INFO("pct:%d", scui_ui_res_local->scroll_pct);
        scui_widget_draw(SCUI_UI_SCENE_LIST_SCALE_RING, NULL, false);
        break;
    case scui_event_widget_scroll_e:
        SCUI_LOG_INFO("scui_event_widget_scroll_e");
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_ui_res_local->bar_wait  = 0;
        scui_ui_res_local->bar_alpha = scui_alpha_cover;
        scui_widget_alpha_set(SCUI_UI_SCENE_LIST_SCALE_RING, scui_ui_res_local->bar_alpha, false);
        scui_scroll_auto_percent_get(SCUI_UI_SCENE_LIST_SCALE_SCROLL, &scui_ui_res_local->scroll_pct);
        SCUI_LOG_INFO("pct:%d", scui_ui_res_local->scroll_pct);
        scui_widget_draw(SCUI_UI_SCENE_LIST_SCALE_RING, NULL, false);
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
void scui_ui_scene_list_scale_ring_event_proc(scui_event_t *event)
{
    static scui_coord_t pct = 0;
    static scui_coord_t way = +1;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
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
        scui_widget_draw_image(event->object, &clip, image_bg, NULL, color_black);
        
        clip.x = (466 - 462) / 2;
        clip.y = (466 - 462) / 2;
        clip.w = 462;
        clip.h = 462;
        
        scui_coord_t angle_d = 29;
        scui_coord_t angle_s = -29 + scui_ui_res_local->scroll_pct * (angle_d) / 100;
        scui_coord_t angle_e = angle_s + angle_d;
        SCUI_LOG_INFO("angle:<s:%d,e:%d>", angle_s, angle_e);
        scui_widget_draw_ring(event->object, &clip, image_ring, NULL, angle_s, color_white, angle_e, 100, image_edge);
        
        // 额外绘制一个全局遮罩
        // clip = scui_widget_draw_clip(event->object);
        // clip.x = (466 - 398) / 2;
        // scui_handle_t image_mask = scui_image_prj_image_src_repeat_mask_12_all_maskpng;
        // scui_widget_draw_image(event->object, &clip, image_mask, NULL, (scui_color_t){0});
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
