/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 线条绘制(抗锯齿)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_color   源色调
 *@param src_width   线宽
 *@param src_pos_1   坐标点
 *@param src_pos_2   坐标点
 *@param src_alpha   透明度
 */
void scui_draw_line(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                    scui_color_t    src_color,   scui_coord_t src_width,
                    scui_point_t    src_pos_1,   scui_point_t src_pos_2,
                    scui_alpha_t    src_alpha)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_width <= 0)
        src_width  = 1;
    
    SCUI_PIXEL_TYPE pixel = {0};
    SCUI_PIXEL_TYPE src_pixel = scui_pixel_by_color(src_color.color);
    scui_multi_t dst_line = dst_surface->hor_res * SCUI_PIXEL_SIZE;
    uint8_t *dst_addr = dst_surface->pixel;
    
    /* 这里变成了一个区域, 直接填色 */
    if (src_pos_1.x == src_pos_2.x || src_pos_1.y == src_pos_2.y) {
        scui_area_t src_clip = {0};
        src_clip.x1 = scui_min(src_pos_1.x, src_pos_2.x);
        src_clip.y1 = scui_min(src_pos_1.y, src_pos_2.y);
        src_clip.x2 = scui_max(src_pos_1.x, src_pos_2.x);
        src_clip.y2 = scui_max(src_pos_1.y, src_pos_2.y);
        scui_area_s_to_m(&src_clip);
        
        /* 均匀俩边算上自己是一个奇数 */
        if (src_width % 2 == 0)
            src_width++;
        
        if (src_pos_1.x == src_pos_2.x) {
            src_clip.x -= src_width / 2;
            src_clip.w += src_width / 2;
        }
        if (src_pos_1.y == src_pos_2.y) {
            src_clip.y -= src_width / 2;
            src_clip.h += src_width / 2;
        }
        
        scui_area_t dst_area = {0};
        if (!scui_area_inter(&dst_area, dst_clip, &src_clip))
             return;
        scui_draw_area_fill(dst_surface, &dst_area, &src_pixel, src_alpha);
        return;
    }
    
    /* 用于绘制skew的线段 */
    /* 下面的内容是从其他地方抄录整理 */
    
    scui_point_t pos_s = {0};
    scui_point_t pos_e = {0};
    /* 从上往下绘制目标 */
    if (src_pos_1.y < src_pos_2.y) {
        pos_s = src_pos_1;
        pos_e = src_pos_2;
    } else {
        pos_s = src_pos_2;
        pos_e = src_pos_1;
    }
    
    scui_area_t  clip_d = {0};
    scui_multi_t dx = pos_e.x - pos_s.x;
    scui_multi_t dy = pos_e.y - pos_s.y;
    
    if (dy / dx == 0) {
        /* 斜率在-45到+45之间 */
        
        /* 斜率在-45到0之间, 从左上往右下画 */
        /* 斜率在0到+45之间, 从右上往左下画 */
        
        scui_multi_t dt = pos_s.x;
        clip_d.x = pos_s.x;
        clip_d.h = src_width - 1;
        
        if (dx > 0)
            dt = pos_s.x - 1;
        else
            clip_d.x = pos_s.x + 1;
        
        for (scui_multi_t idx_j = 1; idx_j <= dy; idx_j++) {
            
            if (dx > 0) {
                clip_d.x = dt;
                dt = pos_s.x + idx_j * dx / dy;
            } else {
                dt = clip_d.x;
                clip_d.x = pos_s.x + idx_j * dx / dy;
            }
            
            clip_d.w = dt - clip_d.x;
            clip_d.y = pos_s.y + idx_j - src_width / 2;
            
            // 上下两边渐变补偿颜色打点
            for (scui_multi_t idx_i = 0; idx_i < clip_d.w; idx_i++) {
                
                scui_alpha_t alpha = 0;
                alpha = (uint16_t)scui_alpha_cover * (2 * idx_i + 1) / (2 * clip_d.w);
                alpha =  dx < 0 ? scui_alpha_cover - alpha : alpha;
                alpha = (uint16_t)alpha * src_alpha / scui_alpha_cover;
                
                scui_point_t point = {0};
                point.x = clip_d.x + idx_i;
                point.y = clip_d.y - 1;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * SCUI_PIXEL_SIZE;
                    SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                    pixel = scui_pixel_mix_with_alpha(&src_pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                    // pixel = scui_pixel_blend_with_alpha(&src_pixel, alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                }
                
                point.y = clip_d.y + clip_d.h;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * SCUI_PIXEL_SIZE;
                    SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                    pixel = scui_pixel_mix_with_alpha(&src_pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                    // pixel = scui_pixel_blend_with_alpha(&src_pixel, alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    #if 0
                    // scui_draw_area_fill(dst_surface, &dst_area, &src_pixel, src_alpha);
                    #else
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * SCUI_PIXEL_SIZE;
                        SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                        // pixel = scui_pixel_blend_with_alpha(src_pixel, src_alpha, dst_addr_ofs, dst_surface->alpha);
                        pixel = scui_pixel_mix_with_alpha(&src_pixel, src_alpha, dst_addr_ofs, scui_alpha_cover - src_alpha);
                        scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                    }
                    #endif
                }
            }
        }
    } else {
        /* 斜率在+45到+135之间 */
        scui_multi_t df = +1;
        
        clip_d.x = pos_s.x;
        clip_d.y = pos_s.y;
        clip_d.h = 0;
        clip_d.w = src_width - 1;
        
        if (dx < 0) {
            dx = -dx;
            df = -1;
        }
        
        for (scui_multi_t idx_i = 1; idx_i <= dx; idx_i++) {
            
            clip_d.y = clip_d.y + clip_d.h;
            clip_d.h = pos_s.y + idx_i * dy / dx - clip_d.y;
            clip_d.x = pos_s.x + idx_i * df - src_width / 2;
            
            // 上下两边渐变补偿颜色打点
            for (scui_multi_t idx_j = 0; idx_j < clip_d.h; idx_j++) {
                
                scui_alpha_t alpha = 0;
                alpha = (uint16_t)scui_alpha_cover * (2 * idx_j + 1) / (2 * clip_d.h);
                alpha =  df < 0 ? scui_alpha_cover - alpha : alpha;
                alpha = (uint16_t)alpha * src_alpha / scui_alpha_cover;
                
                scui_point_t point = {0};
                point.x = clip_d.x - 1;
                point.y = clip_d.y + idx_j;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * SCUI_PIXEL_SIZE;
                    SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                    pixel = scui_pixel_mix_with_alpha(&src_pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                    // pixel = scui_pixel_blend_with_alpha(&src_pixel, alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                }
                
                point.x = clip_d.x + clip_d.w;
                alpha = scui_alpha_cover - alpha;
                if (scui_area_point(dst_clip, &point)) {
                    uint8_t *dst_ofs = dst_addr + point.y * dst_line + point.x * SCUI_PIXEL_SIZE;
                    SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                    pixel = scui_pixel_mix_with_alpha(&src_pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                    // pixel = scui_pixel_blend_with_alpha(&src_pixel, alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                }
                
                scui_area_t dst_area = {0};
                if (scui_area_inter(&dst_area, dst_clip, &clip_d)) {
                    SCUI_LOG_INFO("<%d,%d,%d,%d>", dst_area.x, dst_area.y, dst_area.w, dst_area.h);
                    #if 0
                    // scui_draw_area_fill(dst_surface, &dst_area, &src_pixel, src_alpha);
                    #else
                    for (scui_multi_t idx_line = dst_area.y; idx_line < dst_area.y + dst_area.h; idx_line++)
                    for (scui_multi_t idx_item = dst_area.x; idx_item < dst_area.x + dst_area.w; idx_item++) {
                        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * SCUI_PIXEL_SIZE;
                        SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                        // pixel = scui_pixel_blend_with_alpha(src_pixel, src_alpha, dst_addr_ofs, dst_surface->alpha);
                        pixel = scui_pixel_mix_with_alpha(&src_pixel, src_alpha, dst_addr_ofs, scui_alpha_cover - src_alpha);
                        scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                    }
                    #endif
                }
            }
        }
    }
}
