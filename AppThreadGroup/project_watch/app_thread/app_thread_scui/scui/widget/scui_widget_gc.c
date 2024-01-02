/*实现目标:
 *    控件图形上下文(Graphic Context)
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 控件gc重置
 *@param widget 控件实例
 */
void scui_widget_gc_reset(scui_widget_t *widget)
{
    memset(&widget->gc.clip, 0, sizeof(scui_area_t));
    widget->gc.surface = SCUI_HANDLE_INVALID;
}

/*@brief  控件gc获取画布
 *@param  widget 控件实例
 *@retval 画布句柄
 */
scui_handle_t scui_widget_gc_surface_get(scui_widget_t *widget)
{
    return widget->gc.surface;
}

/*@brief 控件gc设置画布
 *@param widget 控件实例
 *@param surface 画布句柄
 */
void scui_widget_gc_surface_set(scui_widget_t *widget, scui_handle_t surface)
{
    widget->gc.surface = surface;
}

/*@brief 控件gc获取剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_get(scui_widget_t *widget, scui_area_t *clip)
{
    *clip = widget->gc.clip;
}

/*@brief 控件gc设置剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_set(scui_widget_t *widget, scui_area_t *clip)
{
    widget->gc.clip = *clip;
}

/*@brief 控件gc追加剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_merge(scui_widget_t *widget, scui_area_t *clip)
{
    scui_area_merge(&widget->gc.clip, &widget->gc.clip, clip);
}

/*@brief 控件gc清除剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
void scui_widget_gc_clip_clear(scui_widget_t *widget)
{
    widget->gc.clip.w = 0;
    widget->gc.clip.h = 0;
}

/*@brief 控件gc剪切域为空检查
 *@param widget 控件实例
 *@retval 剪切域为空
 */
bool scui_widget_gc_clip_empty(scui_widget_t *widget)
{
    #if 0
    /* wait adapte */
    #endif
}

/*@brief 控件gc更新画布alpha
 *@param widget 控件实例
 *@param alpha  画布alpha
 */
void scui_widget_gc_alpha_update(scui_widget_t *widget, uint8_t alpha)
{
    #if 0
    /* wait adapte */
    #endif
}






/*@brief 控件gc在画布绘制纯色区域
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param color    源色调
 */
void scui_widget_gc_draw_color(scui_widget_t *widget, scui_area_t *dst_clip, scui_color_gradient_t color)
{
    scui_surface_t *dst_surface = scui_handle_get(widget->gc.surface);
    APP_SYS_ASSERT(dst_surface != NULL);
    
    if (dst_clip == NULL);
        dst_clip  = &dst_surface->clip;
    
    SCUI_PIXEL_TYPE pixel = scui_pixel_by_color(color.color);
    scui_draw_area_fill(dst_surface, dst_clip, &pixel, widget->alpha);
}


/*@brief 控件gc在画布绘制图像
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param color    图像源色调(调色板使用)
 */
void scui_widget_gc_draw_image(scui_widget_t *widget, scui_area_t *dst_clip,
                               scui_handle_t  handle, scui_area_t *src_clip, scui_color_gradient_t color)
{
    scui_image_t *image = scui_handle_get(handle);
    scui_surface_t *dst_surface = scui_handle_get(widget->gc.surface);
    APP_SYS_ASSERT(image != NULL && dst_surface != NULL);
    
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
