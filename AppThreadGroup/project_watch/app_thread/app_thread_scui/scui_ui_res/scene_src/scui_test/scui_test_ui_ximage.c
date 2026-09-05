/*实现目标:
 *    测试(widget ximage)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像子控件事件回调(布局在json, 参数在此注入)
 *@param event 事件
 */
void scui_test_ui_ximage_item_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_create: {
        scui_handle_t handle = event->object;
        scui_coord_t  index = scui_widget_child_to_index(event->object);
        const char *url = "https://github.com/HelloUnkira/AppThreadGroup.git";
        switch (index) {
        case 0: /* barcode */
            scui_ximage_barcode(handle, (uint8_t *)url, strlen(url),
                SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF),
                SCUI_SCALE_COF);
            break;
        case 1: /* qrcode */
            scui_ximage_qrcode(handle, (uint8_t *)url, strlen(url),
                SCUI_COLOR_MAKE32_LD(false, 0x0, 0xFF00FF00, 0xFF0000FF),
                SCUI_SCALE_COF);
            break;
        case 2: /* vedio:gif:x1 无限 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_bulb, SCUI_SCALE_COF, -1);
            break;
        case 3: /* vedio:lottie:x1 无限 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_musiclottie, SCUI_SCALE_COF, -1);
            break;
        case 4: /* vedio:gif:x2 3次 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_bulb, SCUI_SCALE_COF, 3);
            break;
        case 5: /* vedio:lottie:x2 5次 */
            scui_ximage_vedio_play(handle, scui_image_prj_vedio_comfirmlottie, SCUI_SCALE_COF, 5);
            break;
        case 6: { /* replace:6个index 1秒循环 */
            static const scui_handle_t index_list[] = {
                scui_image_prj_weather_arrow_up,
                scui_image_prj_weather_arrow_down,
                scui_image_prj_weather_up,
                scui_image_prj_weather_down,
                scui_image_prj_weather_uv,
                scui_image_prj_weather_ring_uv,
            };
            scui_coord_t index_num = sizeof(index_list) / sizeof(index_list[0]);
            scui_multi_t index_speed = SCUI_SCALE_COF * 1000 / ((scui_multi_t)index_num * SCUI_ANIMA_TICK);
            scui_ximage_replace_play(handle, (scui_handle_t *)index_list, index_num, index_speed, -1);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_ximage_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    default:
        break;
    }
}