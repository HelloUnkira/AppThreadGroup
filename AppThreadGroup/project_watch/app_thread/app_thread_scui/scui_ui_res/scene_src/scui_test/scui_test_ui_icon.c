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
        scui_ximage_maker_define(ximage_maker);
        scui_handle_t ximage_handle = SCUI_HANDLE_INVALID;
        
        /* 垂直居中, 水平居中靠摆一排 */
        scui_coord_t ximage_gap  = 15;
        scui_coord_t ximage_w[4] = {80, 100, 80, 100};
        scui_coord_t ximage_total = 0;
        for (uint8_t idx = 0; idx < 4; idx++)
            ximage_total += ximage_w[idx] + ximage_gap;
        ximage_total -= ximage_gap;
        
        ximage_maker.widget.parent  = event->object;
        
        /* gif:x1 无限 */
        ximage_maker.widget.clip.x   = (SCUI_HOR_RES - ximage_total) / 2;
        ximage_maker.widget.clip.y   = SCUI_VER_RES / 2 - 80 / 2;
        ximage_maker.widget.clip.w   = 80;
        ximage_maker.widget.clip.h   = 80;
        scui_widget_create(&ximage_maker, &ximage_handle);
        scui_ximage_vedio_play(ximage_handle, scui_image_prj_vedio_bulb, SCUI_SCALE_COF, -1);
        
        /* lottie:x1 无限 */
        ximage_maker.widget.clip.x  += 80 + ximage_gap;
        ximage_maker.widget.clip.y   = SCUI_VER_RES / 2 - 100 / 2;
        ximage_maker.widget.clip.w   = 100;
        ximage_maker.widget.clip.h   = 100;
        scui_widget_create(&ximage_maker, &ximage_handle);
        scui_ximage_vedio_play(ximage_handle, scui_image_prj_vedio_musiclottie, SCUI_SCALE_COF, -1);
        
        /* gif:x2 3次 */
        ximage_maker.widget.clip.x  += 100 + ximage_gap;
        ximage_maker.widget.clip.y   = SCUI_VER_RES / 2 - 80 / 2;
        ximage_maker.widget.clip.w   = 80;
        ximage_maker.widget.clip.h   = 80;
        scui_widget_create(&ximage_maker, &ximage_handle);
        scui_ximage_vedio_play(ximage_handle, scui_image_prj_vedio_bulb, SCUI_SCALE_COF, 3);
        
        /* lottie:x2 5次 */
        ximage_maker.widget.clip.x  += 80 + ximage_gap;
        ximage_maker.widget.clip.y   = SCUI_VER_RES / 2 - 100 / 2;
        ximage_maker.widget.clip.w   = 100;
        ximage_maker.widget.clip.h   = 100;
        scui_widget_create(&ximage_maker, &ximage_handle);
        scui_ximage_vedio_play(ximage_handle, scui_image_prj_vedio_comfirmlottie, SCUI_SCALE_COF, 5);
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
