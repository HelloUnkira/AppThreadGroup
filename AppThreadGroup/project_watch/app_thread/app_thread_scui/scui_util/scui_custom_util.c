/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件:插件:导航点
 *@param event       自定义控件事件
 *@param clip        剪切域(绘制区域)
 *@param wait        图像句柄(未选中)
 *@param color_wait  图像源色调(调色板使用)
 *@param focus       图像句柄(选中)
 *@param color_focus 图像源色调(调色板使用)
 *@param count       导航点数量
 *@param index       目标索引(选中)
 *@param span        间隙
 *@param dir_hor     水平方向
 */
void scui_custom_draw_image_indicator(scui_event_t *event, scui_area_t  *clip,
                                      scui_handle_t wait,  scui_color_t  color_wait,
                                      scui_handle_t focus, scui_color_t  color_focus,
                                      scui_handle_t count, scui_handle_t index,
                                      scui_handle_t span,  bool          dir_hor)
{
    SCUI_LOG_DEBUG("");
    
    if (!scui_widget_event_check_execute(event))
         return;
    
    scui_image_t *image_wait  = scui_handle_get(wait);
    scui_image_t *image_focus = scui_handle_get(focus);
    SCUI_ASSERT(image_wait != NULL);
    SCUI_ASSERT(image_focus != NULL);
    
    scui_point_t offset = {0};
    for (scui_handle_t idx = 0; idx < count; idx++) {
        if (idx == index) {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_surface_draw_image(event->object, &dst_clip, focus, NULL, color_focus);
            if (dir_hor)
                offset.x += image_focus->pixel.width  + span;
            else
                offset.y += image_focus->pixel.height + span;
        } else {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_surface_draw_image(event->object, &dst_clip, wait, NULL, color_wait);
            if (dir_hor)
                offset.x += image_wait->pixel.width  + span;
            else
                offset.y += image_wait->pixel.height + span;
        }
    }
}

/*@brief 自定义控件:插件:绕圆旋转图像
 *@param event  自定义控件事件
 *@param center 旋转中心
 *@param handle 图像句柄
 *@param color  图像源色调(调色板使用)
 *@param radius 旋转半径
 *@param angle  旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_custom_draw_image_circle_rotate(scui_event_t *event,  scui_point_t *center,
                                          scui_handle_t handle, scui_color_t  color,
                                          scui_coord_t  radius, scui_coord_t  angle)
{
    SCUI_LOG_DEBUG("");
    
    if (!scui_widget_event_check_execute(event))
         return;
    
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    scui_multi_t point_x = radius * scui_cos4096((int32_t)angle) >> 12;
    scui_multi_t point_y = radius * scui_sin4096((int32_t)angle) >> 12;
    
    scui_area_t clip = {
        .x = center->x + point_x - image->pixel.width  / 2,
        .y = center->y + point_y - image->pixel.height / 2,
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
    scui_widget_surface_draw_image(event->object, &clip, handle, NULL, color);
}

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
    
    /* 常规填色 */
    if (mix == 0) {
        scui_widget_surface_draw_color(event->object, clip, color);
        return;
    }
    
    /* 渐变填色 */
    if (mix == 1) {
        /* 无渐变填色 */
        if (color.color_s.full == color.color_e.full) {
            scui_widget_surface_draw_color(event->object, clip, color);
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
            .alpha   = scui_widget_alpha_get(event->object),
        };
        
        scui_area_t dst_clip = {0};
        scui_area_t dst_area = scui_widget_surface_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
            scui_widget_surface_draw_pattern(event->object, &dst_clip, &pattern, NULL, (scui_color_t){0});
        
        SCUI_MEM_FREE(pixel);
        return;
    }
    
    SCUI_ASSERT(false);
}
