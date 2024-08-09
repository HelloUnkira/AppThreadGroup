/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 行拷贝(可以使用DMA-copy加速优化)
 *@param dst_addr 目标地址
 *@param src_addr 源地址
 *@param len 数据字节长度
 */
void scui_draw_line_copy(void *dst_addr, void *src_addr, uint32_t len)
{
    memcpy(dst_addr, src_addr, len);
}

/*@brief 区域模糊(可以使用GPU-blur加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 */
void scui_draw_area_blur(scui_surface_t *dst_surface, scui_area_t *dst_clip)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    #define GAUSS_SCALA     5
    SCUI_ASSERT(GAUSS_SCALA % 2 != 0);
    /* 高斯模糊核<s> */
     int32_t gauss_ofs = GAUSS_SCALA / 2;
    uint32_t gauss_sum = 0;
    uint32_t gauss_cof[GAUSS_SCALA][GAUSS_SCALA] = {
        {1,  4,  7,  4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1,  4,  7,  4, 1},
    };
    uintptr_t pixel_ofs[GAUSS_SCALA][GAUSS_SCALA] = {0};
    scui_color_wt_t pixel_mat[GAUSS_SCALA][GAUSS_SCALA] = {0};
    
    for (int8_t idx_j = 0; idx_j < GAUSS_SCALA; idx_j++)
    for (int8_t idx_i = 0; idx_i < GAUSS_SCALA; idx_i++) {
         pixel_ofs[idx_j][idx_i] = 0;
         pixel_ofs[idx_j][idx_i] += - dst_line * (idx_j + gauss_ofs - (GAUSS_SCALA - 1));
         pixel_ofs[idx_j][idx_i] += - dst_byte * (idx_i + gauss_ofs - (GAUSS_SCALA - 1));
         gauss_sum += gauss_cof[idx_j][idx_i];
    }
    for (int8_t idx_j = 0; idx_j < GAUSS_SCALA; idx_j++)
    for (int8_t idx_i = 0; idx_i < GAUSS_SCALA; idx_i++) {
         gauss_cof[idx_j][idx_i] *= SCUI_SCALE_COF;
         gauss_cof[idx_j][idx_i] /= gauss_sum;
    }
    /* 高斯模糊核<e> */
    
    for (scui_multi_t idx_line = gauss_ofs; idx_line < draw_area.h - gauss_ofs; idx_line++)
    for (scui_multi_t idx_item = gauss_ofs; idx_item < draw_area.w - gauss_ofs; idx_item++) {
         uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
         scui_color_wt_t pixel = 0;
        
        /* 高斯模糊<s> */
        for (int8_t idx_j = 0; idx_j < GAUSS_SCALA; idx_j++)
        for (int8_t idx_i = 0; idx_i < GAUSS_SCALA; idx_i++) {
             uint8_t *pixel_ptr = dst_ofs + pixel_ofs[idx_j][idx_i];
             scui_pixel_by_cf(dst_surface->format, &pixel_mat[idx_j][idx_i], pixel_ptr);
        }
        /* 高斯模糊<e> */
        
        if (dst_surface->format == scui_pixel_cf_bmp565) {
            uint32_t ch_r = 0, ch_g = 0, ch_b = 0;
            for (int8_t idx_j = 0; idx_j < GAUSS_SCALA; idx_j++)
            for (int8_t idx_i = 0; idx_i < GAUSS_SCALA; idx_i++) {
                scui_color565_t *color565_ij = &pixel_mat[idx_j][idx_i];
                ch_r += (uint32_t)color565_ij->ch.r * gauss_cof[idx_j][idx_i];
                ch_g += (uint32_t)color565_ij->ch.g * gauss_cof[idx_j][idx_i];
                ch_b += (uint32_t)color565_ij->ch.b * gauss_cof[idx_j][idx_i];
            }
            scui_color565_t *color565 = &pixel;
            color565->ch.r = ch_r >> SCUI_SCALE_OFS;
            color565->ch.g = ch_g >> SCUI_SCALE_OFS;
            color565->ch.b = ch_b >> SCUI_SCALE_OFS;
        }
        
        if (dst_surface->format == scui_pixel_cf_bmp8565) {
            uint32_t ch_a = 0, ch_r = 0, ch_g = 0, ch_b = 0;
            for (int8_t idx_j = 0; idx_j < GAUSS_SCALA; idx_j++)
            for (int8_t idx_i = 0; idx_i < GAUSS_SCALA; idx_i++) {
                scui_color8565_t *color8565_ij = &pixel_mat[idx_j][idx_i];
                ch_a += (uint32_t)color8565_ij->ch.a * gauss_cof[idx_j][idx_i];
                ch_r += (uint32_t)color8565_ij->ch.r * gauss_cof[idx_j][idx_i];
                ch_g += (uint32_t)color8565_ij->ch.g * gauss_cof[idx_j][idx_i];
                ch_b += (uint32_t)color8565_ij->ch.b * gauss_cof[idx_j][idx_i];
            }
            scui_color8565_t *color8565 = &pixel;
            color8565->ch.a = ch_a >> SCUI_SCALE_OFS;
            color8565->ch.r = ch_r >> SCUI_SCALE_OFS;
            color8565->ch.g = ch_g >> SCUI_SCALE_OFS;
            color8565->ch.b = ch_b >> SCUI_SCALE_OFS;
        }
        
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
    }
}

/*@brief 区域填充像素点(可以使用DMA-fill加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_pixel   像素点
 *@param src_alpha   像素点透明度
 */
void scui_draw_area_fill(scui_surface_t  *dst_surface, scui_area_t *dst_clip,
                         scui_color_t     src_color,   scui_alpha_t src_alpha)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    /* 在src_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    if (src_alpha == scui_alpha_cover) {
        /* 先填充第一行像素点 */
        for (scui_multi_t idx = 0; idx < draw_area.w; idx++)
            scui_pixel_by_cf(dst_surface->format, dst_addr + idx * dst_byte, &src_pixel);
        /* 后使用第一行像素点填充其他行像素点 */
        for (scui_multi_t idx = 1; idx < draw_area.h; idx++)
            scui_draw_line_copy(dst_addr + idx * dst_line, dst_addr, dis_line);
    } else {
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                                dst_surface->format, &src_pixel, src_alpha);
        }
    }
}

/*@brief 区域拷贝(可以使用DMA-copy加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 */
void scui_draw_area_copy(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                         scui_surface_t *src_surface, scui_area_t *src_clip)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    if (dst_surface->alpha  != scui_alpha_cover     ||
        src_surface->alpha  != scui_alpha_cover     ||
        dst_surface->format != src_surface->format  ||
       (dst_surface->format != scui_pixel_cf_bmp565 &&
        dst_surface->format != scui_pixel_cf_bmp888)) {
        SCUI_LOG_ERROR("unsupported copy");
        return;
    }
    
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    SCUI_ASSERT(dst_clip->x + draw_area.w <= dst_surface->hor_res);
    SCUI_ASSERT(dst_clip->y + draw_area.h <= dst_surface->ver_res);
    
    if (scui_area_empty(&draw_area))
        return;
    
    /* 在dst_surface.clip中的dst_clip_v中拷贝到src_surface.clip中的src_clip_v中 */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_coord_t src_byte = scui_pixel_bits(src_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t src_line = src_surface->hor_res * src_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * dst_byte;
    uint8_t *src_addr = src_surface->pixel + src_clip_v.y * src_line + src_clip_v.x * src_byte;
    /* 注意区域对齐坐标 */
    for (scui_multi_t idx = 0; idx < draw_area.h; idx++)
        scui_draw_line_copy(dst_addr + idx * dst_line, src_addr + idx * src_line, dis_line);
}

/*@brief 区域混合(可以使用DMA2D-blend加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 *@param src_color   画布源色调(调色板)
 */
void scui_draw_area_blend(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                          scui_surface_t *src_surface, scui_area_t *src_clip,
                          scui_color_t    src_color)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    if (src_surface->alpha == scui_alpha_trans)
        return;
    
    /* 全覆盖混合:直接copy */
    if (dst_surface->alpha  == scui_alpha_cover     &&
        src_surface->alpha  == scui_alpha_cover     &&
        dst_surface->format == src_surface->format  &&
       (dst_surface->format == scui_pixel_cf_bmp565 ||
        dst_surface->format == scui_pixel_cf_bmp888)) {
        scui_draw_area_copy(dst_surface, dst_clip, src_surface, src_clip);
        return;
    }
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    SCUI_ASSERT(dst_clip->x + draw_area.w <= dst_surface->hor_res);
    SCUI_ASSERT(dst_clip->y + draw_area.h <= dst_surface->ver_res);
    
    if (scui_area_empty(&draw_area))
        return;
    
    /* 在dst_surface.clip中的dst_clip_v中每个像素点混合到src_surface.clip中的src_clip_v中 */
    scui_coord_t dst_bits = scui_pixel_bits(dst_surface->format);
    scui_coord_t src_bits = scui_pixel_bits(src_surface->format);
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_coord_t src_byte = scui_pixel_bits(src_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t src_line = src_surface->hor_res * src_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * dst_byte;
    uint8_t *src_addr = src_surface->pixel + src_clip_v.y * src_line + src_clip_v.x * src_byte;
    
    /* 像素格式不带透明度, 像素格式带透明度 */
    if ((dst_surface->format == scui_pixel_cf_bmp565 || dst_surface->format == scui_pixel_cf_bmp8565  ||
         dst_surface->format == scui_pixel_cf_bmp888 || dst_surface->format == scui_pixel_cf_bmp8888) &&
        (src_surface->format == scui_pixel_cf_bmp565 || src_surface->format == scui_pixel_cf_bmp8565  ||
         src_surface->format == scui_pixel_cf_bmp888 || src_surface->format == scui_pixel_cf_bmp8888)) {
        
        scui_color_wt_t filter = 0;
        scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
        
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
            uint8_t *src_ofs = src_addr + idx_line * src_line + idx_item * src_byte;
            
            if (src_color.filter) {
                scui_color_wt_t color = 0;
                scui_pixel_by_cf(src_surface->format, &color, src_ofs);
                if (color == filter)
                    continue;
            }
            
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                src_surface->format, src_ofs, src_surface->alpha);
        }
        return;
    }
    
    scui_alpha_t alpha = 0;
    scui_multi_t dst_pixel_ofs = dst_clip_v.y * dst_surface->hor_res + dst_clip_v.x;
    scui_multi_t src_pixel_ofs = src_clip_v.y * src_surface->hor_res + src_clip_v.x;
    dst_addr = dst_surface->pixel + dst_pixel_ofs * dst_byte;
    src_addr = src_surface->pixel + src_pixel_ofs * src_bits / 8;
    
    /* pixel cover:(调色板) */
    if (src_surface->format == scui_pixel_cf_palette4) {
        /* 调色板数组(为空时计算,有时直接取): */
        scui_multi_t palette_len = 1 << 4;
        scui_color_wt_t palette_table[1 << 4] = {0};
        scui_color_wt_t filter = 0;
        /* 起始色调和结束色调固定 */
        scui_pixel_by_color(SCUI_PIXEL_CF_DEF, &palette_table[0], src_color.color_e);
        scui_pixel_by_color(SCUI_PIXEL_CF_DEF, &palette_table[palette_len - 1], src_color.color_s);
        scui_pixel_by_color(SCUI_PIXEL_CF_DEF, &filter, src_color.color_f);
        /* 无渐变时: */
        if (palette_table[0] == palette_table[palette_len - 1]) {
            if (src_color.filter && palette_table[0] == filter)
                return;
            for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
                uint8_t *src_ofs = src_addr + (idx_line * src_surface->hor_res + idx_item) * src_bits / 8;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = (*src_addr_ofs >> (idx_item % 2 == 0 ? 0 : 4)) & 0xF;
                alpha = (uint32_t)src_surface->alpha * palette * 0x11 / scui_alpha_cover;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                    SCUI_PIXEL_CF_DEF, &palette_table[0], alpha);
            }
        } else {
            for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
                uint8_t *src_ofs = src_addr + (idx_line * src_surface->hor_res + idx_item) * src_bits / 8;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = (*src_addr_ofs >> (idx_item % 2 == 0 ? 0 : 4)) & 0xF;
                if (palette != 0 && palette != 0xF)
                if (palette_table[palette] == 0x00) {
                    uint32_t *pixel_1 = &palette_table[0];
                    uint32_t *pixel_2 = &palette_table[palette_len - 1];
                    scui_alpha_t alpha_1 = palette * 0x11;
                    scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                    
                    palette_table[palette] = palette_table[0];
                    scui_pixel_mix_with(SCUI_PIXEL_CF_DEF, &palette_table[palette], alpha_1,
                                        SCUI_PIXEL_CF_DEF, &palette_table[palette_len - 1], alpha_2);
                }
                
                if (src_color.filter && palette_table[palette] == filter)
                    continue;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                    SCUI_PIXEL_CF_DEF, &palette_table[palette], src_surface->alpha);
            }
        }
        return;
    }
    if (src_surface->format == scui_pixel_cf_palette8) {
        /* 调色板数组(为空时计算,有时直接取): */
        scui_multi_t palette_len = 1 << 8;
        scui_color_wt_t palette_table[1 << 8] = {0};
        scui_color_wt_t filter = 0;
        /* 起始色调和结束色调固定 */
        scui_pixel_by_color(SCUI_PIXEL_CF_DEF, &palette_table[0], src_color.color_e);
        scui_pixel_by_color(SCUI_PIXEL_CF_DEF, &palette_table[palette_len - 1], src_color.color_s);
        scui_pixel_by_color(SCUI_PIXEL_CF_DEF, &filter, src_color.color_f);
        /* 无渐变时: */
        if (palette_table[0] == palette_table[palette_len - 1]) {
            if (src_color.filter && palette_table[0] == filter)
                return;
            for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
                uint8_t *src_ofs = src_addr + (idx_line * src_surface->hor_res + idx_item) * src_bits / 8;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = *src_addr_ofs;
                alpha = (uint32_t)src_surface->alpha * palette / scui_alpha_cover;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                    SCUI_PIXEL_CF_DEF, &palette_table[0], alpha);
            }
        } else {
            for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
                uint8_t *src_ofs = src_addr + (idx_line * src_surface->hor_res + idx_item) * src_bits / 8;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = *src_addr_ofs;
                if (palette != 0 && palette != 0xFF)
                if (palette_table[palette] == 0x00) {
                    
                    uint32_t *pixel_1 = &palette_table[0];
                    uint32_t *pixel_2 = &palette_table[palette_len - 1];
                    scui_alpha_t alpha_1 = palette;
                    scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                    
                    palette_table[palette] = palette_table[0];
                    scui_pixel_mix_with(dst_surface->format, &palette_table[palette], alpha_1,
                                        dst_surface->format, &palette_table[palette_len - 1], alpha_2);
                }
                
                if (src_color.filter && palette_table[palette] == filter)
                    continue;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                    SCUI_PIXEL_CF_DEF, &palette_table[palette], src_surface->alpha);
            }
        }
        return;
    }
    
    SCUI_LOG_ERROR("unsupported blend:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}

/*@brief 图形变换迁移(可以使用VGLITE-blit加速优化)
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_surface 画布实例
 *@param src_clip    画布绘制区域
 *@param inv_matrix  变换矩阵的逆矩阵
 *@param matrix      变换矩阵的原矩阵
 */
void scui_draw_area_blit_by_matrix(scui_surface_t *dst_surface, scui_area_t   *dst_clip,
                                   scui_surface_t *src_surface, scui_area_t   *src_clip,
                                   scui_matrix_t  *inv_matrix,  scui_matrix_t *matrix)
{
    #if SCUI_DRAW_MISC_USE_MATRIX == 0
    SCUI_ASSERT(false);
    #endif
    
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    SCUI_ASSERT(inv_matrix  != NULL);
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    #if 1
    // 利用原图进行一次源初变换,以修饰限制目标区域
    if (matrix != NULL) {
        scui_face2_t face2 = {0};
        scui_face3_t face3 = {0};
        scui_area2_by_area(&face2, src_clip);
        
        dst_area.x1 = scui_coord_max;
        dst_area.y1 = scui_coord_max;
        dst_area.x2 = scui_coord_min;
        dst_area.y2 = scui_coord_min;
        /* 对四个图像的角进行一次正变换 */
        for (uint8_t idx = 0; idx < 4; idx++) {
            scui_point3_by_point2(&face3.point3[idx], &face2.point2[idx]);
            scui_point3_transform_by_matrix(&face3.point3[idx], matrix);
            scui_point3_to_point2(&face3.point3[idx], &face2.point2[idx]);
            
            dst_area.x1 = scui_min(dst_area.x1, face2.point2[idx].x - 1);
            dst_area.y1 = scui_min(dst_area.y1, face2.point2[idx].y - 1);
            dst_area.x2 = scui_max(dst_area.x2, face2.point2[idx].x + 1);
            dst_area.y2 = scui_max(dst_area.y2, face2.point2[idx].y + 1);
        }
        scui_area_m_by_s(&dst_area);
        
        scui_area_t clip_inter = {0};
        if (!scui_area_inter(&clip_inter, &dst_area, &dst_clip_v))
             return;
        dst_clip_v = clip_inter;
    }
    #endif
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    SCUI_ASSERT(dst_clip->x + draw_area.w <= scui_disp_get_hor_res());
    SCUI_ASSERT(dst_clip->y + draw_area.h <= scui_disp_get_ver_res());
    
    if (scui_area_empty(&draw_area))
        return;
    
    /* 在dst_surface.clip中的dst_clip_v中每个像素点混合到src_surface.clip中的src_clip_v中 */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_coord_t src_byte = scui_pixel_bits(src_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t src_line = src_surface->hor_res * src_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * dst_byte;
    uint8_t *src_addr = src_surface->pixel + src_clip_v.y * src_line + src_clip_v.x * src_byte;
    
    /* 像素格式不带透明度, 像素格式带透明度 */
    if ((dst_surface->format == scui_pixel_cf_bmp565 || dst_surface->format == scui_pixel_cf_bmp8565  ||
         dst_surface->format == scui_pixel_cf_bmp888 || dst_surface->format == scui_pixel_cf_bmp8888) &&
        (src_surface->format == scui_pixel_cf_bmp565 || src_surface->format == scui_pixel_cf_bmp8565  ||
         src_surface->format == scui_pixel_cf_bmp888 || src_surface->format == scui_pixel_cf_bmp8888)) {
        
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = dst_clip_v.y; idx_line < dst_clip_v.y + dst_clip_v.h; idx_line++)
        for (scui_multi_t idx_item = dst_clip_v.x; idx_item < dst_clip_v.x + dst_clip_v.w; idx_item++) {
            scui_point_t  point  = {0};
            scui_point2_t point2 = {0};
            scui_point3_t point3 = {0};
            point2.y = idx_line;
            point2.x = idx_item;
            /* 反扫描结果坐标对每一个坐标进行逆变换 */
            scui_point3_by_point2(&point3, &point2);
            scui_point3_transform_by_matrix(&point3, inv_matrix);
            scui_point3_to_point2(&point3, &point2);
            point2.y += src_clip_v.y;
            point2.x += src_clip_v.x;
            point.y = (scui_coord_t)point2.y;
            point.x = (scui_coord_t)point2.x;
            
            #if 1
            /* 这里使用点对点上色 */
            /* 逆变换的结果落在的源区域, 取样上色 */
            if (scui_area_point(&src_clip_v, &point)) {
                uint8_t *dst_ofs = dst_surface->pixel + idx_line * dst_line + idx_item * dst_byte;
                uint8_t *src_ofs = src_surface->pixel + point.y * src_line + point.x * src_byte;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                    src_surface->format, src_ofs, src_surface->alpha);
            }
            #else
            scui_coord_t dy = (point2.y - point.y) * SCUI_SCALE_COF;
            scui_coord_t dx = (point2.x - point.x) * SCUI_SCALE_COF;
            // 这里使用双线性插值求平均
            scui_point_t pos4[4] = {
                {.x = point.x + 0, .y = point.y + 0,},
                {.x = point.x + 1, .y = point.y + 0,},
                {.x = point.x + 0, .y = point.y + 1,},
                {.x = point.x + 1, .y = point.y + 1,},
            };
            scui_point_t avg4[4] = {
                {.x = dx,                  .y = dy,},
                {.x = SCUI_SCALE_COF - dx, .y = dy,},
                {.x = dx,                  .y = SCUI_SCALE_COF - dy,},
                {.x = SCUI_SCALE_COF - dx, .y = SCUI_SCALE_COF - dy,},
            };
            
            scui_color_wt_t del_a = 0;
            scui_color_wt_t del_r = 0;
            scui_color_wt_t del_g = 0;
            scui_color_wt_t del_b = 0;
            
            bool val_cnt = false;
            for (uint8_t idx = 0; idx < 4; idx++)
                if (scui_area_point(&src_clip_v, &pos4[idx])) {
                    val_cnt = true;
                    
                    uint8_t *src_ofs = src_surface->pixel + pos4[idx].y * src_line + pos4[idx].x * src_byte;
                    scui_multi_t cof_dxy = avg4[idx].x * avg4[idx].y;
                    
                    if (src_surface->format == scui_pixel_cf_bmp565) {
                        scui_color565_t *color565 = src_ofs;
                        del_r += color565->ch.r * cof_dxy;
                        del_g += color565->ch.g * cof_dxy;
                        del_b += color565->ch.b * cof_dxy;
                    }
                    
                    if (src_surface->format == scui_pixel_cf_bmp8565) {
                        scui_color8565_t *color8565 = src_ofs;
                        del_a += color8565->ch.a * cof_dxy;
                        del_r += color8565->ch.r * cof_dxy;
                        del_g += color8565->ch.g * cof_dxy;
                        del_b += color8565->ch.b * cof_dxy;
                    }
                }
            
            if (val_cnt) {
                del_a >>= SCUI_SCALE_OFS; del_a >>= SCUI_SCALE_OFS;
                del_r >>= SCUI_SCALE_OFS; del_r >>= SCUI_SCALE_OFS;
                del_g >>= SCUI_SCALE_OFS; del_g >>= SCUI_SCALE_OFS;
                del_b >>= SCUI_SCALE_OFS; del_b >>= SCUI_SCALE_OFS;
                
                uint8_t *dst_ofs = dst_surface->pixel + idx_line * dst_line + idx_item * dst_byte;
                
                if (src_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t color565 = {.ch.r = del_r,.ch.g = del_g,.ch.b = del_b,};
                    
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                        src_surface->format, &color565, src_surface->alpha);
                }
                
                if (src_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t color8565 = {.ch.a = del_a,.ch.r = del_r,.ch.g = del_g,.ch.b = del_b,};
                    
                    scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                        src_surface->format, &color8565, src_surface->alpha);
                }
            }
            #endif
        }
        return;
    }
    
    SCUI_LOG_ERROR("unsupported blit:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}
