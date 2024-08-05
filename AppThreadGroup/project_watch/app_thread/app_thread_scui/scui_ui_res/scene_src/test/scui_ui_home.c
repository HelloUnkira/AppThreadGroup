/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_home_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
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
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_key_click: {
        
        scui_window_switch_type_t switch_type = 0;
        switch_type = scui_window_switch_type_cfg_get();
        
        static scui_ui_scene_list_type_t type = 0;
        type++;
        
        if (type < scui_ui_scene_list_type_s + 1)
            type = scui_ui_scene_list_type_e - 1;
        if (type > scui_ui_scene_list_type_e - 1)
            type = scui_ui_scene_list_type_s + 1;
        
        SCUI_ASSERT(type > scui_ui_scene_list_type_s);
        SCUI_ASSERT(type < scui_ui_scene_list_type_e);
        
        switch (type) {
        case scui_ui_scene_list_type_list_scale: { // 缩放列表
            scui_ui_scene_list_cfg(type);
            scui_window_switch_type_cfg_set(scui_window_switch_none);
            scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
            scui_window_switch_type_cfg_set(switch_type);
            break;
        }
        case scui_ui_scene_list_type_list_arc: // 弧形列表
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_LIST_ARC, false);
            break;
        case scui_ui_scene_list_type_honeycomb: // 蜂窝
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_HONEYCOMB, false);
            break;
        case scui_ui_scene_list_type_waterfall: // 瀑布
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_WATERFALL, false);
            break;
        case scui_ui_scene_list_type_themewheel: // 波轮
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_THUMBWHEEL, false);
            break;
        case scui_ui_scene_list_type_spread: // 扩散
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_SPREAD, false);
            break;
        default:
            break;
        }
        
        scui_widget_event_mask_over(event);
        break;
    }
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
void scui_ui_scene_home_c_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        static uint8_t cnt = 0;
        cnt++;
        
        
        static uint32_t cache_cnt = 0;
        cache_cnt++;
        
        #if 0   // font glyph cache test
        scui_font_glyph_unit_t glyph_unit = {0};
        glyph_unit.name = SCUI_FONT_TYPE_32_ZH;
        glyph_unit.glyph.unicode_letter = 0x4E00 + cache_cnt % (0x9FFF - 0x4E00);
        scui_font_glyph_cache_load(&glyph_unit);
        scui_font_glyph_cache_unload(&glyph_unit);
        #endif
        
        #if 0   // graph cache test
        scui_image_unit_t image_unit = {0};
        scui_handle_t image = SCUI_HANDLE_OFFSET_IMAGE + 1;
        image += cache_cnt % 3350;
        image_unit.image = scui_handle_get(image);
        scui_image_cache_load(&image_unit);
        scui_image_cache_unload(&image_unit);
        #endif
        
        
        
        if (cnt % (1000 / SCUI_ANIMA_TICK) == 0)
            scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_indev_data_set_t *data_set = NULL;
        scui_indev_data_set(&data_set);
        
        scui_area_t   clip = scui_widget_clip(event->object);
        scui_coord_t  span = 0;
        scui_handle_t image[5] = {
            scui_image_prj_image_src_num_102_white_56x76_04_03png + data_set->sys_time_h / 10,
            scui_image_prj_image_src_num_102_white_56x76_04_03png + data_set->sys_time_h % 10,
            scui_image_prj_image_src_num_102_white_56x76_14_13png,
            scui_image_prj_image_src_num_102_white_56x76_04_03png + data_set->sys_time_m / 10,
            scui_image_prj_image_src_num_102_white_56x76_04_03png + data_set->sys_time_m % 10,
        };
        scui_coord_t width  = span * 4;
        scui_coord_t height = span * 4;
        scui_color_t color  = {.color.full = 0xFFFFFFFF};
        scui_image_list_calc(image, 5, &width, false);
        scui_image_list_calc(image, 5, &height, true);
        
        clip.x = (SCUI_DRV_HOR_RES - width) / 2;
        clip.y = (SCUI_DRV_VER_RES) / 8;
        clip.w =  width;
        clip.h =  76;
        scui_custom_draw_text(event, &clip, image, color, span, 5, false);
        
        #if 0   // 仅仅测试,不美观
        clip.x = (SCUI_DRV_HOR_RES) / 12;
        clip.y = (SCUI_DRV_VER_RES - height) / 2;
        clip.w =  56;
        clip.h =  height;
        scui_custom_draw_text(event, &clip, image, color, span, 5, true);
        #endif
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
