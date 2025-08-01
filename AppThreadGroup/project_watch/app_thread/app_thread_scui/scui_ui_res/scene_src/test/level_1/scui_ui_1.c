/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        SCUI_LOG_WARN("scui_event_key_click");
        static scui_window_switch_type_t switch_type = SCUI_WINDOW_SWITCH_TYPE;
        switch_type++;
        
        if (switch_type < scui_window_switch_single_s + 1)
            switch_type = scui_window_switch_single_e - 1;
        if (switch_type > scui_window_switch_single_e - 1)
            switch_type = scui_window_switch_single_s + 1;
        
        SCUI_ASSERT(switch_type > scui_window_switch_single_s);
        SCUI_ASSERT(switch_type < scui_window_switch_single_e);
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        *cfg_type = switch_type;
        
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_1_vedio_event_proc(scui_event_t *event)
{
    static scui_image_frame_t image_frame_gif = {0};
    
    static scui_image_frame_t image_frame_lottie_1 = {0};
    static scui_image_frame_t image_frame_lottie_2 = {0};
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        static uint8_t cnt = 0;
        cnt++;
        
        if (cnt % 1 == 0) {
            // 更新一个图像帧
            scui_widget_draw(event->object, NULL, false);
        }
        break;
    }
    case scui_event_create: {
        
        // 构建一个图像帧
        image_frame_gif.type = scui_image_type_gif;
        image_frame_gif.handle = scui_image_prj_image_src_vedio_bulbgif;
        image_frame_gif.gif.loop = 100;
        scui_image_frame_make(&image_frame_gif);
        
        // 构建一个图像帧
        image_frame_lottie_1.type = scui_image_type_lottie;
        image_frame_lottie_1.handle = scui_image_prj_image_src_vedio_comfirmlottiejson;
        scui_image_frame_make(&image_frame_lottie_1);
        image_frame_lottie_2.type = scui_image_type_lottie;
        image_frame_lottie_2.handle = scui_image_prj_image_src_vedio_musiclottiejson;
        scui_image_frame_make(&image_frame_lottie_2);
        break;
    }
    case scui_event_destroy: {
        
        // 销毁一个图像帧
        scui_image_frame_burn(&image_frame_gif);
        
        // 销毁一个图像帧
        scui_image_frame_burn(&image_frame_lottie_1);
        scui_image_frame_burn(&image_frame_lottie_2);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_image_frame_data(&image_frame_gif); {
            
            scui_handle_t image = image_frame_gif.frame;
            scui_area_t clip = scui_widget_clip(event->object);
            clip.x += SCUI_VER_RES / 6;
            clip.y += SCUI_VER_RES / 5 - scui_image_h(image) / 2;
            scui_widget_draw_image(event->object, &clip, image, NULL, SCUI_COLOR_UNUSED);
        }
        
        scui_image_frame_data(&image_frame_lottie_1); {
            
            image_frame_lottie_1.lottie.index++;
            if (image_frame_lottie_1.lottie.index >= image_frame_lottie_1.lottie.frame)
                image_frame_lottie_1.lottie.index  = 0;
            
            scui_handle_t image = image_frame_lottie_1.frame;
            scui_area_t clip = scui_widget_clip(event->object);
            clip.x += SCUI_VER_RES / 6 + 120;
            clip.y += SCUI_VER_RES / 5 - scui_image_h(image) / 2;
            scui_widget_draw_image(event->object, &clip, image, NULL, SCUI_COLOR_UNUSED);
        }
        
        scui_image_frame_data(&image_frame_lottie_2); {
            
            image_frame_lottie_2.lottie.index++;
            if (image_frame_lottie_2.lottie.index >= image_frame_lottie_2.lottie.frame)
                image_frame_lottie_2.lottie.index  = 0;
            
            scui_handle_t image = image_frame_lottie_2.frame;
            scui_area_t clip = scui_widget_clip(event->object);
            clip.x += SCUI_VER_RES / 6 + 120 + 120;
            clip.y += SCUI_VER_RES / 5 - scui_image_h(image) / 2;
            scui_widget_draw_image(event->object, &clip, image, NULL, SCUI_COLOR_UNUSED);
        }
        break;
    }
    default:
        break;
    }
}
