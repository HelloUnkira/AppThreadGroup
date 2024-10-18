/*实现目标:
 *    图像绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域图像绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->image.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->image.dst_clip;
    scui_image_t   *src_image   = draw_dsc->image.src_image;
    scui_area_t    *src_clip    = draw_dsc->image.src_clip;
    scui_alpha_t    src_alpha   = draw_dsc->image.src_alpha;
    scui_color_t    src_color   = draw_dsc->image.src_color;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    /* draw_dsc */ {
        scui_draw_dsc_t draw_dsc = {
            .area_blend.dst_surface = dst_surface,
            .area_blend.dst_clip    = dst_clip,
            .area_blend.src_surface = &image_surface,
            .area_blend.src_clip    = src_clip,
            .area_blend.src_color   = src_color,
        };
        scui_draw_area_blend(&draw_dsc);
    };
    scui_image_cache_unload(&image);
}

/*@brief 区域图像缩放
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image_scale(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->image_scale.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->image_scale.dst_clip;
    scui_image_t   *src_image   = draw_dsc->image_scale.src_image;
    scui_area_t    *src_clip    = draw_dsc->image_scale.src_clip;
    scui_alpha_t    src_alpha   = draw_dsc->image_scale.src_alpha;
    scui_color_t    src_color   = draw_dsc->image_scale.src_color;
    scui_point_t    src_scale   = draw_dsc->image_scale.src_scale;
    scui_point_t    dst_offset  = draw_dsc->image_scale.dst_offset;
    scui_point_t    src_offset  = draw_dsc->image_scale.src_offset;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_matrix_t src_matrix = {0};
    scui_matrix_identity(&src_matrix);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = +dst_offset.x, .y = +dst_offset.y,});
    scui_matrix_scale(&src_matrix, &(scui_point2_t){.x = src_scale.x / 1024.0f, .y = src_scale.y / 1024.0f,});
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = -src_offset.x, .y = -src_offset.y,});
    scui_matrix_t inv_matrix = src_matrix;
    scui_matrix_inverse(&inv_matrix);
    
    /* draw_dsc */ {
        scui_draw_dsc_t draw_dsc = {
            .area_blit_by_matrix.dst_surface = dst_surface,
            .area_blit_by_matrix.dst_clip    = dst_clip,
            .area_blit_by_matrix.src_surface = &image_surface,
            .area_blit_by_matrix.src_clip    = src_clip,
            .area_blit_by_matrix.src_color   = src_color,
            .area_blit_by_matrix.inv_matrix  = &inv_matrix,
            .area_blit_by_matrix.src_matrix  = &src_matrix,
        };
        scui_draw_area_blit_by_matrix(&draw_dsc);
    };
    scui_image_cache_unload(&image);
}

/*@brief 区域图像旋转
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image_rotate(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->image_rotate.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->image_rotate.dst_clip;
    scui_image_t   *src_image   = draw_dsc->image_rotate.src_image;
    scui_area_t    *src_clip    = draw_dsc->image_rotate.src_clip;
    scui_alpha_t    src_alpha   = draw_dsc->image_rotate.src_alpha;
    scui_color_t    src_color   = draw_dsc->image_rotate.src_color;
    scui_coord_t    src_angle   = draw_dsc->image_rotate.src_angle;
    scui_point_t   *src_anchor  = draw_dsc->image_rotate.src_anchor;
    scui_point_t   *src_center  = draw_dsc->image_rotate.src_center;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_matrix_t src_matrix = {0};
    scui_matrix_identity(&src_matrix);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = +src_anchor->x, .y = +src_anchor->y,});
    scui_matrix_rotate(&src_matrix, (float)src_angle, 0x00);
    scui_matrix_translate(&src_matrix, &(scui_point2_t){.x = -src_center->x, .y = -src_center->y,});
    scui_matrix_t inv_matrix = src_matrix;
    scui_matrix_inverse(&inv_matrix);
    
    /* draw_dsc */ {
        scui_draw_dsc_t draw_dsc = {
            .area_blit_by_matrix.dst_surface = dst_surface,
            .area_blit_by_matrix.dst_clip    = dst_clip,
            .area_blit_by_matrix.src_surface = &image_surface,
            .area_blit_by_matrix.src_clip    = src_clip,
            .area_blit_by_matrix.src_color   = src_color,
            .area_blit_by_matrix.inv_matrix  = &inv_matrix,
            .area_blit_by_matrix.src_matrix  = &src_matrix,
        };
        scui_draw_area_blit_by_matrix(&draw_dsc);
    };
    scui_image_cache_unload(&image);
}

/*@brief 区域图像变换
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_image_blit_by_matrix(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->image_blit_by_matrix.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->image_blit_by_matrix.dst_clip;
    scui_image_t   *src_image   = draw_dsc->image_blit_by_matrix.src_image;
    scui_area_t    *src_clip    = draw_dsc->image_blit_by_matrix.src_clip;
    scui_alpha_t    src_alpha   = draw_dsc->image_blit_by_matrix.src_alpha;
    scui_color_t    src_color   = draw_dsc->image_blit_by_matrix.src_color;
    scui_matrix_t  *inv_matrix  = draw_dsc->image_blit_by_matrix.inv_matrix;
    scui_matrix_t  *src_matrix  = draw_dsc->image_blit_by_matrix.src_matrix;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .format  = image.image->format,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    /* draw_dsc */ {
        scui_draw_dsc_t draw_dsc = {
            .area_blit_by_matrix.dst_surface = dst_surface,
            .area_blit_by_matrix.dst_clip    = dst_clip,
            .area_blit_by_matrix.src_surface = &image_surface,
            .area_blit_by_matrix.src_clip    = src_clip,
            .area_blit_by_matrix.src_color   = src_color,
            .area_blit_by_matrix.inv_matrix  = inv_matrix,
            .area_blit_by_matrix.src_matrix  = src_matrix,
        };
        scui_draw_area_blit_by_matrix(&draw_dsc);
    };
    scui_image_cache_unload(&image);
}
