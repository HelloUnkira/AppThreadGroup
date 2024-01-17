/*实现目标:
 *    控件画布(图形上下文(Graphic Context))
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 控件画布在画布绘制纯色区域
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param color    源色调
 */
void scui_widget_surface_draw_color(scui_widget_t *widget, scui_area_t *dst_clip, scui_color_gradient_t color)
{
    scui_surface_t *dst_surface = &widget->surface;
    
    if (dst_clip == NULL);
        dst_clip  = &dst_surface->clip;
    
    SCUI_PIXEL_TYPE pixel = scui_pixel_by_color(color.color);
    scui_draw_area_fill(dst_surface, dst_clip, &pixel, widget->alpha);
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
    scui_surface_t *dst_surface = &widget->surface;
    scui_image_t *image = scui_handle_get(handle);
    APP_SYS_ASSERT(image != NULL);
    
    scui_area_t image_clip = {
        .x = 0, .w = image->pixel.width,
        .y = 0, .h = image->pixel.height,
    };
    
    if (dst_clip == NULL)
        dst_clip  = &dst_surface->clip;
    if (src_clip == NULL)
        src_clip  = &image_clip;
    
    scui_image_unit_t image_unit = {.image = image,};
    scui_image_cache_load(&image_unit);
    scui_draw_image(dst_surface, dst_clip, &image_unit, src_clip, color, widget->alpha);
    scui_image_cache_unload(&image_unit);
}
