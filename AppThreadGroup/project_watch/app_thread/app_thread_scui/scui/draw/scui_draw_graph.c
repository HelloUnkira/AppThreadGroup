/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域填充像素点
 *@param dst_surface 画布实例
 *@param dst_point   画布坐标
 *@param src_pixel   像素点
 *@param src_alpha   像素点透明度
 */
void scui_draw_pixel(scui_surface_t  *dst_surface, scui_point_t *dst_point,
                     SCUI_PIXEL_TYPE *src_pixel,   scui_alpha_t  src_alpha)
{
    scui_area_t dst_area = {
        .w = dst_surface->hor_res,
        .h = dst_surface->ver_res,
    };
    
    if (scui_area_point(&dst_area, dst_point)) {
        SCUI_PIXEL_TYPE  pixel = {0};
        SCUI_PIXEL_TYPE *dst_addr_ofs = dst_surface->pixel + dst_point->y * SCUI_PIXEL_SIZE + dst_point->x;
        // pixel = scui_pixel_blend_with_alpha(src_pixel, src_alpha, dst_addr_ofs, dst_surface->alpha);
        pixel = scui_pixel_mix_with_alpha(src_pixel, src_alpha, dst_addr_ofs, scui_alpha_cover - src_alpha);
        scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
    }
}
