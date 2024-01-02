#ifndef SCUI_DRAW_IMAGE_H
#define SCUI_DRAW_IMAGE_H

/*@brief 区域图像绘制(可以使用DMA2D-blend加速优化)
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param color        图像源色调(调色板使用)
 *@param alpha        图像透明度(非图像自带透明度)
 */
void scui_draw_image(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                     scui_image_unit_t *image_unit,  scui_area_t *src_clip,
                     scui_color_gradient_t color,    scui_alpha_t alpha);

#endif
