#ifndef SCUI_WIDGET_SURFACE_H
#define SCUI_WIDGET_SURFACE_H

/*@brief 控件画布清除剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
static inline void scui_widget_surface_clip_clear(scui_widget_t *widget)
{
    widget->surface.clip = (scui_area_t){0};
}

/*@brief 控件画布追加剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
static inline void scui_widget_surface_clip_merge(scui_widget_t *widget, scui_area_t *clip)
{
    scui_area_t clip_merge = {0};
    scui_area_merge(&clip_merge, &widget->surface.clip, clip);
    widget->surface.clip = clip_merge;
}

/*@brief 控件画布剪切域为空检查
 *@param widget 控件实例
 *@retval 剪切域为空
 */
static inline bool scui_widget_surface_clip_empty(scui_widget_t *widget)
{
    return scui_area_empty(&widget->surface.clip);
}



/*@brief 控件画布在画布绘制纯色区域
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param color    源色调
 */
void scui_widget_surface_draw_color(scui_widget_t *widget, scui_area_t *dst_clip, scui_color_gradient_t color);

/*@brief 控件画布在画布绘制图像
 *@param widget   控件实例
 *@param dst_clip 控件绘制区域
 *@param handle   图像句柄
 *@param src_clip 图像源绘制区域
 *@param color    图像源色调(调色板使用)
 */
void scui_widget_surface_draw_image(scui_widget_t *widget, scui_area_t *dst_clip,
                                    scui_handle_t  handle, scui_area_t *src_clip, scui_color_gradient_t color);

#endif
