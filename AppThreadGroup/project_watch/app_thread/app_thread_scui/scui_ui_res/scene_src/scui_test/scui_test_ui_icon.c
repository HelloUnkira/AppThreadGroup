/*实现目标:
 *    测试(widget icon)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_icon_event_proc(scui_event_t *event)
{
    static bool ptr_long_jump = false;
    
    switch (event->type) {
    case scui_event_create: {
        
        #if 1
        scui_xvedio_maker_define(xvedio_maker);
        scui_handle_t xvedio_handle = SCUI_HANDLE_INVALID;
        
        /* 垂直居中, 水平居中靠摆一排 */
        scui_coord_t xvedio_gap  = 15;
        scui_coord_t xvedio_w[3] = {80, 100, 100};
        scui_coord_t xvedio_total = 0;
        for (uint8_t idx = 0; idx < 3; idx++)
            xvedio_total += xvedio_w[idx] + xvedio_gap;
        xvedio_total -= xvedio_gap;
        
        xvedio_maker.widget.parent  = event->object;
        
        xvedio_maker.widget.clip.x   = (SCUI_HOR_RES - xvedio_total) / 2;
        xvedio_maker.widget.clip.y   = SCUI_VER_RES / 2 - 80 / 2;
        xvedio_maker.widget.clip.w   = 80;
        xvedio_maker.widget.clip.h   = 80;
        xvedio_maker.iframe.type     = scui_image_type_gif;
        xvedio_maker.iframe.handle   = scui_image_prj_vedio_bulb;
        xvedio_maker.iframe.gif.loop = 100;
        scui_widget_create(&xvedio_maker, &xvedio_handle);
        
        xvedio_maker.widget.clip.x  += 80 + xvedio_gap;
        xvedio_maker.widget.clip.y   = SCUI_VER_RES / 2 - 100 / 2;
        xvedio_maker.widget.clip.w   = 100;
        xvedio_maker.widget.clip.h   = 100;
        xvedio_maker.iframe.type     = scui_image_type_lottie;
        xvedio_maker.iframe.handle   = scui_image_prj_vedio_musiclottie;
        scui_widget_create(&xvedio_maker, &xvedio_handle);
        
        xvedio_maker.widget.clip.x  += 100 + xvedio_gap;
        xvedio_maker.widget.clip.y   = SCUI_VER_RES / 2 - 100 / 2;
        xvedio_maker.widget.clip.w   = 100;
        xvedio_maker.widget.clip.h   = 100;
        xvedio_maker.iframe.type     = scui_image_type_lottie;
        xvedio_maker.iframe.handle   = scui_image_prj_vedio_comfirmlottie;
        scui_widget_create(&xvedio_maker, &xvedio_handle);
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_draw_graph: {
        /* 直接 draw 一排 index 图标(不建控件, 保持间隔) */
        scui_coord_t gap  = 14;
        scui_coord_t size = 60;
        scui_handle_t index_img[] = {
            scui_image_prj_weather_arrow_up,
            scui_image_prj_weather_arrow_down,
            scui_image_prj_weather_up,
            scui_image_prj_weather_down,
            scui_image_prj_weather_uv,
            scui_image_prj_weather_ring_uv,
        };
        uint8_t num = sizeof(index_img) / sizeof(index_img[0]);
        scui_coord_t total = num * size + (num - 1) * gap;
        scui_coord_t pos_y = SCUI_VER_RES * 3 / 4;
        scui_coord_t pos_x = (SCUI_HOR_RES - total) / 2;
        for (uint8_t i = 0; i < num; i++) {
            scui_area_t area = {
                .x = pos_x + i * (size + gap),
                .y = pos_y,
                .w = size,
                .h = size,
            };
            scui_widget_draw_image(event->object, &area, index_img[i], NULL, SCUI_COLOR_UNUSED);
        }
        break;
    }
    case scui_event_ptr_down:
        ptr_long_jump = false;
        break;
    case scui_event_ptr_hold:
        if (event->ptr_tick > 3000) {
            if (!ptr_long_jump) SCUI_LOG_WARN("ptr long hold");
            ptr_long_jump = true;
        }
        break;
    case scui_event_ptr_up:
        if (ptr_long_jump)
            scui_ui_scene_return();
        break;
    default:
        break;
    }
}
