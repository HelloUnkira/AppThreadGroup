/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    #if 1 /* test:index image */
    case scui_event_draw_graph: {
        
        scui_handle_t heart_src = scui_image_prj_image_src_00_theme_icon_00_heart_06_05png;
        scui_handle_t heart_idx = scui_image_prj_image_src_00_theme_icon_00_heart_index_06_05png;
        scui_handle_t spo2_src  = scui_image_prj_image_src_00_theme_icon_01_spo2_06_05png;
        scui_handle_t spo2_idx  = scui_image_prj_image_src_00_theme_icon_01_spo2_index_06_05png;
        scui_handle_t sun_src   = scui_image_prj_image_src_02_weather_icon_weather_big_00_sun_01_00bmp;
        scui_handle_t sun_idx   = scui_image_prj_image_src_02_weather_icon_weather_big_00_sun_index_01_00bmp;
        scui_handle_t bfly_src  = scui_image_prj_image_src_00_3d_Butterfly_SwingDown0png;
        scui_handle_t bfly_idx  = scui_image_prj_image_src_00_3d_Butterfly_index_SwingDown0png;
        
        scui_coord_t x0 = SCUI_HOR_RES * 1 / 8;     /* 原图列 */
        scui_coord_t x1 = SCUI_HOR_RES * 5 / 8;     /* index列 */
        scui_coord_t y  = SCUI_VER_RES * 1 / 8;
        scui_coord_t step = SCUI_VER_RES / 6;
        
        /* 心率图标: 原图 vs index */
        scui_area_t clip_icon_src = {.x = x0, .y = y + step * 0, .w = scui_image_w(heart_src), .h = scui_image_h(heart_src)};
        scui_area_t clip_icon_idx = {.x = x1, .y = y + step * 0, .w = scui_image_w(heart_idx), .h = scui_image_h(heart_idx)};
        scui_widget_draw_image(event->object, &clip_icon_src, heart_src, NULL, SCUI_COLOR_UNUSED);
        scui_widget_draw_image(event->object, &clip_icon_idx, heart_idx, NULL, SCUI_COLOR_UNUSED);
        
        /* 血氧图标: 原图 vs index */
        scui_area_t clip_spo2_src = {.x = x0, .y = y + step * 1, .w = scui_image_w(spo2_src), .h = scui_image_h(spo2_src)};
        scui_area_t clip_spo2_idx = {.x = x1, .y = y + step * 1, .w = scui_image_w(spo2_idx), .h = scui_image_h(spo2_idx)};
        scui_widget_draw_image(event->object, &clip_spo2_src, spo2_src, NULL, SCUI_COLOR_UNUSED);
        scui_widget_draw_image(event->object, &clip_spo2_idx, spo2_idx, NULL, SCUI_COLOR_UNUSED);
        
        /* 太阳天气图: 原图 vs index */
        scui_area_t clip_sun_src = {.x = x0, .y = y + step * 2, .w = scui_image_w(sun_src), .h = scui_image_h(sun_src)};
        scui_area_t clip_sun_idx = {.x = x1, .y = y + step * 2, .w = scui_image_w(sun_idx), .h = scui_image_h(sun_idx)};
        scui_widget_draw_image(event->object, &clip_sun_src, sun_src, NULL, SCUI_COLOR_UNUSED);
        scui_widget_draw_image(event->object, &clip_sun_idx, sun_idx, NULL, SCUI_COLOR_UNUSED);
        
        /* 蝴蝶渐变图: 原图 vs index */
        scui_area_t clip_bfly_src = {.x = x0, .y = y + step * 3, .w = scui_image_w(bfly_src), .h = scui_image_h(bfly_src)};
        scui_area_t clip_bfly_idx = {.x = x1, .y = y + step * 3, .w = scui_image_w(bfly_idx), .h = scui_image_h(bfly_idx)};
        scui_widget_draw_image(event->object, &clip_bfly_src, bfly_src, NULL, SCUI_COLOR_UNUSED);
        scui_widget_draw_image(event->object, &clip_bfly_idx, bfly_idx, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    #endif
    default:
        break;
    }
}
