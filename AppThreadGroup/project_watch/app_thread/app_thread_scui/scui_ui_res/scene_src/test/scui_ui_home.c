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
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_key_click: {
        scui_ui_scene_list_cfg(0);
        scui_window_switch_type_t type = scui_window_switch_type_cfg_get();
        scui_window_switch_type_cfg_set(scui_window_switch_none);
        scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
        scui_window_switch_type_cfg_set(type);
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
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        static uint8_t cnt = 0;
        cnt++;
        
        if (cnt % (1000 / SCUI_ANIMA_TICK) == 0)
            scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_indev_data_set_t *data_set = NULL;
        scui_indev_data_set(&data_set);
        
        scui_area_t   clip = scui_widget_draw_clip(event->object);
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
        scui_custom_draw_image_keep(event, &clip, image, color, span, 5, false);
        
        #if 0   // 仅仅测试,不美观
        clip.x = (SCUI_DRV_HOR_RES) / 12;
        clip.y = (SCUI_DRV_VER_RES - height) / 2;
        clip.w =  56;
        clip.h =  height;
        scui_custom_draw_image_keep(event, &clip, image, color, span, 5, true);
        #endif
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
