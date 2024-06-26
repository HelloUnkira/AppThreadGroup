/*实现目标:
 *    图像绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域图像绘制(可以使用DMA2D-blend加速优化)
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param src_image   图像源
 *@param src_clip    图像源绘制区域
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param src_color   图像源色调(调色板)
 */
void scui_draw_image(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                     scui_image_t   *src_image,   scui_area_t *src_clip,
                     scui_alpha_t    src_alpha,   scui_color_t src_color)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_image_cf_to_pixel_cf(&image.image->format, &image_surface.format);
    scui_draw_area_blend(dst_surface, dst_clip, &image_surface, src_clip, src_color);
    
    scui_image_cache_unload(&image);
}

/*@brief 区域图像缩放(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以放大缩小指定比例
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param src_image   图像源
 *@param src_clip    图像源绘制区域
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param scale       图形缩放比例(1024为放大系数)
 *@param dst_offset  缩放锚点
 *@param src_offset  缩放锚点
 */
void scui_draw_image_scale(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                           scui_image_t   *src_image,   scui_area_t *src_clip,
                           scui_alpha_t    src_alpha,   scui_point_t scale,
                           scui_point_t    dst_offset,  scui_point_t src_offset)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_matrix_t matrix = {0};
    scui_matrix_identity(&matrix);
    scui_matrix_translate(&matrix, &(scui_point2_t){.x = +dst_offset.x, .y = +dst_offset.y,});
    scui_matrix_scale(&matrix, &(scui_point2_t){.x = scale.x / 1024.0f, .y = scale.y / 1024.0f,});
    scui_matrix_translate(&matrix, &(scui_point2_t){.x = -src_offset.x, .y = -src_offset.y,});
    scui_matrix_inverse(&matrix);
    
    scui_image_cf_to_pixel_cf(&image.image->format, &image_surface.format);
    scui_draw_area_blit_by_matrix(dst_surface, dst_clip, &image_surface, src_clip, &matrix);
    
    scui_image_cache_unload(&image);
}

/*@brief 区域图像旋转(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以指定方向旋转指定角度
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param src_image   图像源
 *@param src_clip    图像源绘制区域
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param angle       图形旋转角度(顺时针旋转:+,逆时针旋转:-)
 *@param anchor      图形旋转轴心
 *@param center      图形旋转中心
 */
void scui_draw_image_rotate(scui_surface_t *dst_surface, scui_area_t  *dst_clip,
                            scui_image_t   *src_image,   scui_area_t  *src_clip,
                            scui_alpha_t    src_alpha,   scui_coord_t  angle,
                            scui_point_t   *anchor,      scui_point_t *center)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_matrix_t matrix = {0};
    scui_matrix_identity(&matrix);
    scui_matrix_translate(&matrix, &(scui_point2_t){.x = +anchor->x, .y = +anchor->y,});
    scui_matrix_rotate(&matrix, (float)angle, 0x00);
    scui_matrix_translate(&matrix, &(scui_point2_t){.x = -center->x, .y = -center->y,});
    scui_matrix_inverse(&matrix);
    
    scui_image_cf_to_pixel_cf(&image.image->format, &image_surface.format);
    scui_draw_area_blit_by_matrix(dst_surface, dst_clip, &image_surface, src_clip, &matrix);
    
    scui_image_cache_unload(&image);
}

/*@brief 区域图像变换(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以放大缩小指定比例
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param src_image   图像源
 *@param src_clip    图像源绘制区域
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param matrix      图像变换矩阵
 */
void scui_draw_image_blit_by_matrix(scui_surface_t *dst_surface, scui_area_t   *dst_clip,
                                    scui_image_t   *src_image,   scui_area_t   *src_clip,
                                    scui_alpha_t    src_alpha,   scui_matrix_t *matrix)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_image_cf_to_pixel_cf(&image.image->format, &image_surface.format);
    scui_draw_area_blit_by_matrix(dst_surface, dst_clip, &image_surface, src_clip, matrix);
    
    scui_image_cache_unload(&image);
}
