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
    float_cfg.main = SCUI_UI_SCENE_HOME;
    float_cfg.list[0] = SCUI_UI_SCENE_FLOAT_1;
    float_cfg.list[1] = SCUI_UI_SCENE_FLOAT_2;
    float_cfg.list[2] = SCUI_UI_SCENE_FLOAT_3;
    float_cfg.list[3] = SCUI_UI_SCENE_FLOAT_4;
    scui_window_float_cfg_set(&float_cfg);
}

static inline void scui_ui_scene_window_list_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(SCUI_UI_SCENE_HOME, &window_cfg);
    window_cfg.sibling[0] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[1] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[2] = SCUI_UI_SCENE_6;
    window_cfg.sibling[3] = SCUI_UI_SCENE_1;
    scui_window_cfg_set(SCUI_UI_SCENE_HOME, &window_cfg);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_home_event_proc(scui_event_t *event)
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
        scui_window_stack_add(SCUI_UI_SCENE_2, false);
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
        
        if (cnt % (100 / SCUI_ANIMA_TICK) == 0)
            scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             break;
        
        scui_indev_data_set_t *data_set = NULL;
        scui_indev_data_set(&data_set);
        
        scui_area_t   clip = scui_widget_surface_clip(event->object);
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
