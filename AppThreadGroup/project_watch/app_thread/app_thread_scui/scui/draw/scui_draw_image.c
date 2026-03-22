/*实现目标:
 *    图像绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域图像绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_image(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    SCUI_ASSERT(draw_dsc->src_image != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = draw_dsc->src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = draw_dsc->src_image->pixel.width,
        .ver_res = draw_dsc->src_image->pixel.height,
        .alpha   = draw_dsc->src_alpha,
    };
    
    scui_draw_area_blend(true, draw_dsc->dst_surface, draw_dsc->dst_clip,
        &image_surface, draw_dsc->src_clip, draw_dsc->src_color);
    
    scui_image_cache_unload(&image);
}

/*@brief 区域图像缩放
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_image_scale(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    SCUI_ASSERT(draw_dsc->src_image != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = draw_dsc->src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = draw_dsc->src_image->pixel.width,
        .ver_res = draw_dsc->src_image->pixel.height,
        .alpha   = draw_dsc->src_alpha,
    };
    scui_point2_t src_scale2 = {
        .x = (scui_coord3_t)draw_dsc->src_scale.x / SCUI_SCALE_COF,
        .y = (scui_coord3_t)draw_dsc->src_scale.y / SCUI_SCALE_COF,
    };
    
    scui_matrix_t src_matrix = {0};
    scui_matrix_identity(&src_matrix);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = +draw_dsc->dst_offset.x, .y = +draw_dsc->dst_offset.y,});
    scui_matrix_scale(&src_matrix, &src_scale2);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = -draw_dsc->src_offset.x, .y = -draw_dsc->src_offset.y,});
    scui_matrix_t inv_matrix = src_matrix;
    scui_matrix_inverse(&inv_matrix);
    
    scui_draw_area_matrix_blend(true, draw_dsc->dst_surface, draw_dsc->dst_clip,
        &image_surface, draw_dsc->src_clip, draw_dsc->src_color, inv_matrix, src_matrix);
    
    scui_image_cache_unload(&image);
}

/*@brief 区域图像旋转
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_image_rotate(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    SCUI_ASSERT(draw_dsc->src_image != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = draw_dsc->src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = draw_dsc->src_image->pixel.width,
        .ver_res = draw_dsc->src_image->pixel.height,
        .alpha   = draw_dsc->src_alpha,
    };
    
    scui_matrix_t src_matrix = {0};
    scui_matrix_identity(&src_matrix);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = +draw_dsc->src_anchor.x, .y = +draw_dsc->src_anchor.y,});
    scui_matrix_rotate_a(&src_matrix, (scui_coord3_t)draw_dsc->src_angle / SCUI_SCALE_COF, 0x00);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = -draw_dsc->src_center.x, .y = -draw_dsc->src_center.y,});
    scui_matrix_t inv_matrix = src_matrix;
    scui_matrix_inverse(&inv_matrix);
    
    scui_draw_area_matrix_blend(true, draw_dsc->dst_surface, draw_dsc->dst_clip,
        &image_surface, draw_dsc->src_clip, draw_dsc->src_color, inv_matrix, src_matrix);
    
    scui_image_cache_unload(&image);
}

/*@brief 区域图像变换
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_image_matrix_blend(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->dst_surface != NULL && draw_dsc->dst_surface->pixel != NULL);
    SCUI_ASSERT(draw_dsc->src_image != NULL);
    
    if (draw_dsc->src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = draw_dsc->src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = draw_dsc->src_image->pixel.width,
        .ver_res = draw_dsc->src_image->pixel.height,
        .alpha   = draw_dsc->src_alpha,
    };
    
    scui_draw_area_matrix_blend(true, draw_dsc->dst_surface, draw_dsc->dst_clip,
        &image_surface, draw_dsc->src_clip, draw_dsc->src_color, draw_dsc->inv_matrix, draw_dsc->src_matrix);
    
    scui_image_cache_unload(&image);
}
