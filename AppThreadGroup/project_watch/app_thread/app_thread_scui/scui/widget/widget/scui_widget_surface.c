/*实现目标:
 *    控件画布(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget)
{
    if (widget->surface.pixel == NULL ||
        widget->surface.pixel == scui_surface_fb_draw()->pixel ||
        widget->surface.pixel == scui_surface_fb_refr()->pixel)
        return false;
    
    return true;
}

/*@brief 控件画布为独立画布
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_change(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_LOG_INFO("widget %u", widget->myself);
    widget->surface.pixel = surface->pixel;
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_widget_surface_change(child, surface);
        }
}

/*-------------------------------------------------*
 *separator----------------------------------------*
 *-------------------------------------------------*/

/*@brief 控件画布在画布绘制纯色区域
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param color    源色调
 */
void scui_widget_surface_draw_color(scui_widget_t *widget, scui_area_t *dst_clip, scui_color_gradient_t color)
{
    SCUI_LOG_INFO("widget %u", widget->myself);
    scui_surface_t *dst_surface = &widget->surface;
    
    if (dst_clip == NULL);
        dst_clip  = &dst_surface->clip;
    
    scui_point_t point = {
        .x = dst_surface->clip.x,
        .y = dst_surface->clip.y,
    };
    if (widget->parent == SCUI_HANDLE_INVALID && scui_widget_surface_only(widget)) {
        dst_surface->clip.x = 0;
        dst_surface->clip.y = 0;
    }
    
    SCUI_PIXEL_TYPE pixel = scui_pixel_by_color(color.color);
    scui_draw_area_fill(dst_surface, dst_clip, &pixel, widget->surface.alpha);
    
    dst_surface->clip.x = point.x;
    dst_surface->clip.y = point.y;
}

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param color    图像源色调(调色板使用)
 */
void scui_widget_surface_draw_image(scui_widget_t *widget, scui_area_t *dst_clip,
                                    scui_handle_t  handle, scui_area_t *src_clip, scui_color_gradient_t color)
{
    SCUI_LOG_INFO("widget %u", widget->myself);
    scui_surface_t *dst_surface = &widget->surface;
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {
        .x = 0, .w = image->pixel.width,
        .y = 0, .h = image->pixel.height,
    };
    
    if (dst_clip == NULL)
        dst_clip  = &dst_surface->clip;
    if (src_clip == NULL)
        src_clip  = &image_clip;
    
    scui_point_t point = {
        .x = dst_surface->clip.x,
        .y = dst_surface->clip.y,
    };
    if (widget->parent == SCUI_HANDLE_INVALID && scui_widget_surface_only(widget)) {
        dst_surface->clip.x = 0;
        dst_surface->clip.y = 0;
    }
    
    scui_image_unit_t image_unit = {.image = image,};
    scui_image_cache_load(&image_unit);
    scui_draw_image(dst_surface, dst_clip, &image_unit, src_clip, color, widget->surface.alpha);
    scui_image_cache_unload(&image_unit);
    
    dst_surface->clip.x = point.x;
    dst_surface->clip.y = point.y;
}
