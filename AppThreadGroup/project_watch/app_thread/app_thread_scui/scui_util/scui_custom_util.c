/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件:插件:区域绘制
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_s,.color_e,)
 *@param mix   绘制模式(0:常规绘制;1:渐变绘制;)
 *@param way   绘制方向(渐变绘制)(0:水平渐变;1:垂直渐变;)
 */
void scui_custom_draw_area(scui_event_t *event, scui_area_t *clip,
                           scui_color_t  color, uint8_t mix, bool way)
{
    SCUI_LOG_DEBUG("");
    
    if (!scui_widget_event_check_execute(event))
         return;
    
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_custom);
    
    /* 常规填色 */
    if (mix == 0) {
        scui_widget_surface_draw_color(widget, clip, color);
        return;
    }
    
    /* 渐变填色 */
    if (mix == 1) {
        /* 无渐变填色 */
        if (color.color_s.full == color.color_e.full) {
            scui_widget_surface_draw_color(widget, clip, color);
            return;
        }
        
        uintptr_t pixel_size = SCUI_PIXEL_SIZE * clip->w * clip->h;
        SCUI_PIXEL_TYPE *pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
        SCUI_PIXEL_TYPE  color_s = scui_pixel_by_color(color.color_s);
        SCUI_PIXEL_TYPE  color_e = scui_pixel_by_color(color.color_e);
        SCUI_PIXEL_TYPE  pixel_a = {0};
        
        if (pixel == NULL) {
            SCUI_LOG_WARN("memory deficit was caught");
            scui_image_cache_clear();
        }
        if (pixel == NULL) {
            pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
            if (pixel == NULL) {
                scui_image_cache_visit();
                SCUI_ASSERT(false);
            }
        }
        
        if (way) {
            /* 垂直渐变 */
            for (scui_coord_t y = 0; y < clip->h; y++) {
                uint8_t percent = (scui_multi_t)y * 100 / clip->h;
                scui_alpha_t alpha_1 = scui_alpha_by_percent(100 - percent);
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                pixel_a = scui_pixel_mix_with_alpha(&color_s, alpha_1, &color_e, alpha_2);
            for (scui_coord_t x = 0; x < clip->w; x++) {
                pixel[y * clip->w + x] = pixel_a;
            }
            }
        } else {
            /* 水平渐变 */
            for (scui_coord_t x = 0; x < clip->w; x++) {
                uint8_t percent = (scui_multi_t)x * 100 / clip->w;
                scui_alpha_t alpha_1 = scui_alpha_by_percent(100 - percent);
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                pixel_a = scui_pixel_mix_with_alpha(&color_s, alpha_1, &color_e, alpha_2);
            for (scui_coord_t y = 0; y < clip->h; y++) {
                pixel[y * clip->w + x] = pixel_a;
            }
            }
        }
        
        scui_surface_t pattern = {
            .pixel   = pixel,
            .format  = SCUI_PIXEL_FORMAT,
            .hor_res = clip->w,
            .ver_res = clip->h,
            .alpha   = widget->alpha,
        };
        
        scui_point_t offset = {0};
        scui_area_t dst_clip = scui_widget_draw_clip(handle);
        scui_area_t dst_area = {0};
        if (scui_area_inter(&dst_area, &dst_clip, clip)) {
            offset.x = dst_area.x;
            offset.y = dst_area.y;
            scui_color_t color_none = {0};
            scui_widget_surface_draw_pattern(widget, &offset, &pattern, NULL, color_none);
        }
        
        SCUI_MEM_FREE(pixel);
        return;
    }
    
    SCUI_ASSERT(false);
}

/*@brief 自定义控件:插件:画线
 *@param event 自定义控件事件
 *@param color 颜色(.color,)
 *@param width 线条宽
 *@param pos_1 位置1
 *@param pos_2 位置2
 */
void scui_custom_draw_line(scui_event_t *event, scui_coord_t width,
                           scui_point_t  pos_1, scui_point_t pos_2,
                           scui_color_t  color)
{
    SCUI_LOG_DEBUG("");
    
    if (!scui_widget_event_check_execute(event))
         return;
    
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_custom);
    
    scui_widget_surface_draw_line(widget, width, pos_1, pos_2, color);
}
