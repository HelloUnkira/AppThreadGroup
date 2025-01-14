/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_mask_all_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw: {
        // 额外绘制一个全局遮罩
        scui_area_t clip = {0};
        
        // 内存真的不够绘制遮罩
        
        scui_handle_t image_mask_u = scui_image_prj_image_src_repeat_mask_06_big_uppng;
        clip.w = scui_image_w(image_mask_u);
        clip.h = scui_image_h(image_mask_u);
        clip.x = (SCUI_DRV_HOR_RES - clip.w) / 2;
        clip.y = 0;
        // scui_widget_draw_image(event->object, &clip, image_mask_u, NULL, SCUI_COLOR_UNUSED);
        
        scui_handle_t image_mask_d = scui_image_prj_image_src_repeat_mask_05_big_downpng;
        clip.w = scui_image_w(image_mask_d);
        clip.h = scui_image_h(image_mask_d);
        clip.x = (SCUI_DRV_HOR_RES - clip.w) / 2;
        clip.y = (SCUI_DRV_VER_RES - clip.h);
        // scui_widget_draw_image(event->object, &clip, image_mask_d, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
