/*实现目标:
 *    调色板圆环绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域图像绘制
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param src_image_r 图像源
 *@param src_clip_r  图像源绘制区域
 *@param src_image_e 图像源
 *@param src_clip_e  图像源绘制区域
 *@param src_angle_s 起始角度
 *@param src_angle_e 结束角度
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param src_color   图像源色调(调色板使用)
 */
void scui_draw_ring(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                    scui_image_t   *src_image,   scui_area_t *src_clip,
                    scui_coord_t    src_angle_s, scui_alpha_t src_alpha,
                    scui_coord_t    src_angle_e, scui_color_t src_color,
                    scui_image_t   *src_image_edge)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL && src_image_edge != NULL);
    
    /* 只支持调色板位图 */
    SCUI_ASSERT(src_image->format == scui_image_format_p4 ||
                src_image->format == scui_image_format_p8);
    
    /* 限制要求,调色板位图为正方形图像 */
    SCUI_ASSERT(src_image->pixel.width == src_image->pixel.height);
    
    /* 只支持调色板位图 */
    SCUI_ASSERT(src_image_edge->format == scui_image_format_p4 ||
                src_image_edge->format == scui_image_format_p8);
    
    /* 限制要求,调色板位图为正方形图像 */
    SCUI_ASSERT(src_image_edge->pixel.width == src_image_edge->pixel.height);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    /* 整环绘制: */
    if ((src_angle_e - src_angle_s) % 360 == 0) {
        scui_draw_image(dst_surface, dst_clip, src_image, src_clip, src_alpha, src_color);
        return;
    }
    
    /* 角度限制(-360, +360): */
    src_angle_s %= 360;
    src_angle_e %= 360;
    
    /* 角度调整(-360, +360): */
    if (src_angle_s >  src_angle_e)
        src_angle_s -= 360;
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_unit_t image_edge = {.image = src_image_edge,};
    scui_image_cache_load(&image);
    scui_image_cache_load(&image_edge);
    SCUI_ASSERT(image.data != NULL);
    SCUI_ASSERT(image_edge.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_surface_t image_edge_surface = {
        .pixel   = image_edge.data,
        .hor_res = src_image_edge->pixel.width,
        .ver_res = src_image_edge->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_area_t edge_clip = {
        .w = image_edge_surface.hor_res,
        .h = image_edge_surface.ver_res,
    };
    
    scui_image_format_to_pixel_format(&image.image->format, &image_surface.format);
    scui_image_format_to_pixel_format(&image_edge.image->format, &image_edge_surface.format);
    
    /* 按四象限绘制: */
    /* 1.绘制俩个端点 */
    /* 2.绘制四个完整象限 */
    /* 3.绘制最多俩个不完整象限 */
    
    /* 绘制起始点 */
    /* 绘制结束点 */
    scui_multi_t dx_ofs = (dst_clip->x - src_clip->x);
    scui_multi_t dy_ofs = (dst_clip->y - src_clip->y);
    scui_multi_t radius = (image_surface.hor_res - image_edge_surface.hor_res) / 2;
    scui_multi_t center_e_sx = radius * scui_cos4096((int32_t)src_angle_s) >> 12;
    scui_multi_t center_e_sy = radius * scui_sin4096((int32_t)src_angle_s) >> 12;
    scui_multi_t center_e_ex = radius * scui_cos4096((int32_t)src_angle_e) >> 12;
    scui_multi_t center_e_ey = radius * scui_sin4096((int32_t)src_angle_e) >> 12;
    scui_area_t edge_clip_s = edge_clip;
    scui_area_t edge_clip_e = edge_clip;
    edge_clip_s.x = dx_ofs + image_surface.hor_res / 2 + center_e_sx - image_edge_surface.hor_res / 2;
    edge_clip_s.y = dy_ofs + image_surface.ver_res / 2 + center_e_sy - image_edge_surface.ver_res / 2;
    edge_clip_e.x = dx_ofs + image_surface.hor_res / 2 + center_e_ex - image_edge_surface.hor_res / 2;
    edge_clip_e.y = dy_ofs + image_surface.ver_res / 2 + center_e_ey - image_edge_surface.ver_res / 2;
    scui_area_t dst_edge_clip_s = {0};
    scui_area_t dst_edge_clip_e = {0};
    if (scui_area_inter(&dst_edge_clip_s, dst_clip, &edge_clip_s))
        scui_draw_image(dst_surface, &dst_edge_clip_s, src_image_edge, &edge_clip, src_alpha, src_color);
    if (scui_area_inter(&dst_edge_clip_e, dst_clip, &edge_clip_e))
        scui_draw_image(dst_surface, &dst_edge_clip_e, src_image_edge, &edge_clip, src_alpha, src_color);
    
    
    
    
    
    
    
    
    scui_image_cache_unload(&image_edge);
    scui_image_cache_unload(&image);
}
