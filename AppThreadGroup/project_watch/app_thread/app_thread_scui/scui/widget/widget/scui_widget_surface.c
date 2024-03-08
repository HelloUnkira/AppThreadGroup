/*实现目标:
 *    控件画布(图形上下文(Graphic Context))
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件画布剪切域刷新
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    widget->surface.clip = widget->clip;
    /* 有独立画布的根控件不记录原点偏移,控件树永远相对独立画布移动 */
    /* 没有独立画布的根控件保留原点偏移,控件树永远相对绘制画布移动 */
    if (widget->parent == SCUI_HANDLE_INVALID && scui_widget_surface_only(widget)) {
        widget->surface.clip.x = 0;
        widget->surface.clip.y = 0;
    }
    
    /* 画布的坐标区域是相对根控件(递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget->surface.clip, &widget_parent->surface.clip))
            widget->surface.clip = clip_inter;
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_surface_refr(child, recurse);
    }
}

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
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    widget->surface.pixel = surface->pixel;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_surface_change(child, surface);
    }
}

/*-------------------------------------------------*
 *separator----------------------------------------*
 *-------------------------------------------------*/

/*@brief 控件画布在画布绘制纯色区域
 *@param widget 控件实例
 *@param color  源色调
 */
void scui_widget_surface_draw_color(scui_widget_t *widget, scui_color_gradient_t color)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    scui_surface_t *dst_surface = &widget->surface;
    scui_area_t     dst_clip    = {0};
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_clip_unit_t *unit = NULL;
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        dst_clip = unit->clip;
        SCUI_PIXEL_TYPE pixel = scui_pixel_by_color(color.color);
        scui_draw_area_fill(dst_surface, &dst_clip, &pixel, widget->surface.alpha);
    }
}

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param color    图像源色调(调色板使用)
 */
void scui_widget_surface_draw_image(scui_widget_t *widget, scui_handle_t  handle,
                                    scui_area_t *src_clip, scui_color_gradient_t color)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    scui_surface_t *dst_surface = &widget->surface;
    scui_area_t     dst_clip    = {0};
    
    scui_image_t *image = scui_handle_get(handle);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {
        .x = 0, .w = image->pixel.width,
        .y = 0, .h = image->pixel.height,
    };
    
    if (src_clip == NULL)
        src_clip  = &image_clip;
    
    if (scui_area_empty(&widget->clip_set.clip))
        return;
    
    scui_image_unit_t image_unit = {.image = image,};
    scui_image_cache_load(&image_unit);
    
    scui_clip_unit_t *unit = NULL;
    scui_list_dll_btra(&widget->clip_set.dl_list, node) {
        unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
        dst_clip = unit->clip;
        scui_area_t src_area = {0};
        if (!scui_area_inter(&src_area, src_clip, &unit->clip))
             continue;
        scui_draw_image(dst_surface, &dst_clip, &image_unit, &src_area, color, widget->surface.alpha);
    }
    
    scui_image_cache_unload(&image_unit);
}
