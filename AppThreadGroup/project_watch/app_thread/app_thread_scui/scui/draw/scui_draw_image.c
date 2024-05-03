/*实现目标:
 *    图像绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

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
                     scui_color_gradient_t color,    scui_alpha_t alpha)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(image_unit != NULL && image_unit->image != NULL && image_unit->data != NULL);
    
    if (alpha == scui_alpha_trans)
        return;
    
    scui_surface_t image_surface = {
        .pixel   = image_unit->data,
        .hor_res = image_unit->image->pixel.width,
        .ver_res = image_unit->image->pixel.height,
        .alpha   = alpha,
    };
    
    switch (image_unit->image->format) {
    case scui_image_format_rgb565:
        image_surface.format = scui_pixel_format_rgb565;
        break;
    case scui_image_format_rgb888:
        image_surface.format = scui_pixel_format_rgb888;
        break;
    case scui_image_format_argb8565:
        image_surface.format = scui_pixel_format_argb8565;
        break;
    case scui_image_format_argb8888:
        image_surface.format = scui_pixel_format_argb8888;
        break;
    case scui_image_format_p4:
        image_surface.format = scui_pixel_format_p4;
        break;
    case scui_image_format_p8:
        image_surface.format = scui_pixel_format_p8;
        break;
    default:
        SCUI_LOG_ERROR("unsupported image pixel");
        return;
    }
    
    scui_surface_t *src_surface = &image_surface;
    scui_draw_area_blend(dst_surface, dst_clip, src_surface, src_clip, color);
}

/*@brief 区域图像缩放(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以放大缩小指定比例
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param alpha        图像透明度(非图像自带透明度)
 *@param zoom         图形缩放比例(放大:+,缩小:-)
 */
void scui_draw_image_zoom(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                          scui_image_unit_t *image_unit,  scui_area_t *src_clip,
                          scui_alpha_t       alpha,       scui_coord_t zoom)
{
}

/*@brief 区域图像旋转(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以指定方向旋转指定角度
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param alpha        图像透明度(非图像自带透明度)
 *@param angle        图形旋转角度(顺时针旋转:+,逆时针旋转:-)
 *@param anchor       图形旋转轴心
 *@param center       图形旋转中心
 */
void scui_draw_image_rotate(scui_surface_t    *dst_surface, scui_area_t  *dst_clip,
                            scui_image_unit_t *image_unit,  scui_area_t  *src_clip,
                            scui_alpha_t       alpha,       scui_coord_t  angle,
                            scui_point_t      *anchor,      scui_point_t *center)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(image_unit != NULL && image_unit->image != NULL && image_unit->data != NULL);
    
    if (alpha == scui_alpha_trans)
        return;
    
    scui_surface_t image_surface = {
        .pixel   = image_unit->data,
        .hor_res = image_unit->image->pixel.width,
        .ver_res = image_unit->image->pixel.height,
        .alpha   = alpha,
    };
    
    switch (image_unit->image->format) {
    case scui_image_format_rgb565:
        image_surface.format = scui_pixel_format_rgb565;
        break;
    case scui_image_format_rgb888:
        image_surface.format = scui_pixel_format_rgb888;
        break;
    case scui_image_format_argb8565:
        image_surface.format = scui_pixel_format_argb8565;
        break;
    case scui_image_format_argb8888:
        image_surface.format = scui_pixel_format_argb8888;
        break;
    case scui_image_format_p4:
        image_surface.format = scui_pixel_format_p4;
        break;
    case scui_image_format_p8:
        image_surface.format = scui_pixel_format_p8;
        break;
    default:
        SCUI_LOG_ERROR("unsupported image pixel");
        return;
    }
    
    scui_surface_t *src_surface = &image_surface;
    
    scui_matrix_t inv_matrix = {0};
    scui_matrix_identity(&inv_matrix);
    scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = +anchor->x, .y = +anchor->y,});
    scui_matrix_rotate(&inv_matrix, (float)angle, 0x00);
    scui_matrix_translate(&inv_matrix, &(scui_point2_t){.x = -center->x, .y = -center->y,});
    scui_matrix_inverse(&inv_matrix);
    scui_draw_area_blit_by_matrix(dst_surface, dst_clip, src_surface, src_clip, &inv_matrix);
}
