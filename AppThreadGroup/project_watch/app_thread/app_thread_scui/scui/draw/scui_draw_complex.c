/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域抖动(可以使用GPU-dither加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_dither(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_dither.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_dither.dst_clip;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    #define DITHER_STATIC     1    /* 静态抖动 */
    #define DITHER_DYNAMIC    1    /* 动态误差扩散抖动 */
    
    #if 0
    #elif DITHER_STATIC
    /* 静态抖动 */
    #define  DITHER_HOR     8
    #define  DITHER_VER     8
    scui_multi_t dither_max = 64;
    scui_multi_t dither_cof[DITHER_VER][DITHER_HOR] = {
        #if 0
        #elif 1 /* Bayer抖动核 */
        { 0, 48, 12, 60,  3, 51, 15, 63},
        {32, 16, 44, 28, 35, 19, 47, 31},
        { 8, 56,  4, 52, 11, 59,  7, 55},
        {40, 24, 36, 20, 43, 27, 39, 23},
        { 2, 50, 14, 62,  1, 49, 13, 61},
        {34, 18, 46, 30, 33, 17, 45, 29},
        {10, 58,  6, 54,  9, 57,  5, 53},
        {42, 26, 38, 22, 41, 25, 37, 21},
        #else
        #endif
    };
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
         uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
         scui_multi_t factor_8 = 0, factor_5 = 0, factor_6 = 0;
        
        /* 抖动<s> */
        factor_8  = dither_cof[idx_line % DITHER_VER][idx_item % DITHER_HOR];
        factor_8 -= dither_max / 2;
        factor_5  = scui_clamp(factor_8 * 4 / (dither_max / 2), -4, +4);
        factor_6  = scui_clamp(factor_8 * 4 / (dither_max / 2), -8, +8);
        /* 抖动<e> */
        
        /* 取原始像素 */
        scui_color_wt_t pixel = 0;
        scui_pixel_by_cf(dst_surface->format, &pixel, dst_ofs);
        
        if (dst_surface->format == scui_pixel_cf_bmp565) {
            
            scui_color565_t *color565 = &pixel;
            color565->ch.r = scui_clamp(color565->ch.r + factor_5, 0, (1 << 5) - 1);
            color565->ch.g = scui_clamp(color565->ch.g + factor_6, 0, (1 << 6) - 1);
            color565->ch.b = scui_clamp(color565->ch.b + factor_5, 0, (1 << 5) - 1);
        }
        
        if (dst_surface->format == scui_pixel_cf_bmp8565) {
            
            scui_color8565_t *color8565 = &pixel;
            color8565->ch.a = scui_clamp(color8565->ch.a + factor_8, 0, (1 << 8) - 1);
            color8565->ch.r = scui_clamp(color8565->ch.r + factor_5, 0, (1 << 5) - 1);
            color8565->ch.g = scui_clamp(color8565->ch.g + factor_6, 0, (1 << 6) - 1);
            color8565->ch.b = scui_clamp(color8565->ch.b + factor_5, 0, (1 << 5) - 1);
        }
        
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
    }
    return;
    #elif DITHER_DYNAMIC
    /* 动态误差扩散抖动 */
    scui_coord_t  error_color_l = sizeof(scui_coord_t) * draw_area.w;
    scui_coord_t *error_color_c = SCUI_MEM_ZALLOC(scui_mem_type_graph, error_color_l * 4);
    scui_coord_t *error_color_n = SCUI_MEM_ZALLOC(scui_mem_type_graph, error_color_l * 4);
    
    scui_coord_t *err_c_ch_a = error_color_c + draw_area.w * 0;
    scui_coord_t *err_c_ch_r = error_color_c + draw_area.w * 1;
    scui_coord_t *err_c_ch_g = error_color_c + draw_area.w * 2;
    scui_coord_t *err_c_ch_b = error_color_c + draw_area.w * 3;
    scui_coord_t *err_n_ch_a = error_color_n + draw_area.w * 0;
    scui_coord_t *err_n_ch_r = error_color_n + draw_area.w * 1;
    scui_coord_t *err_n_ch_g = error_color_n + draw_area.w * 2;
    scui_coord_t *err_n_ch_b = error_color_n + draw_area.w * 3;
    
    err_c_ch_a[0] = 32; err_c_ch_r[1] = 8;
    err_c_ch_g[2] = 16; err_c_ch_b[3] = 8;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
         uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
         scui_coord_t ch_a = 0, ch_r = 0, ch_g = 0, ch_b = 0;
        
        /* 抖动<s> */
        scui_coord_t e_ch_a = err_c_ch_a[idx_item] + err_n_ch_a[idx_item];
        scui_coord_t e_ch_r = err_c_ch_r[idx_item] + err_n_ch_r[idx_item];
        scui_coord_t e_ch_g = err_c_ch_g[idx_item] + err_n_ch_g[idx_item];
        scui_coord_t e_ch_b = err_c_ch_b[idx_item] + err_n_ch_b[idx_item];
        /* 抖动<e> */
        
        /* 取原始像素 */
        scui_color_wt_t pixel = 0;
        scui_pixel_by_cf(dst_surface->format, &pixel, dst_ofs);
        
        if (dst_surface->format == scui_pixel_cf_bmp565) {
            
            scui_color565_t *color565 = &pixel;
            ch_r = color565->ch.r;
            ch_g = color565->ch.g;
            ch_b = color565->ch.b;
            color565->ch.r = scui_clamp(ch_r + e_ch_r, 0, (1 << 5) - 1);
            color565->ch.g = scui_clamp(ch_g + e_ch_g, 0, (1 << 6) - 1);
            color565->ch.b = scui_clamp(ch_b + e_ch_b, 0, (1 << 5) - 1);
            ch_r = color565->ch.r - ch_r;
            ch_g = color565->ch.g - ch_g;
            ch_b = color565->ch.b - ch_b;
        }
        
        if (dst_surface->format == scui_pixel_cf_bmp8565) {
            
            scui_color8565_t *color8565 = &pixel;
            ch_a = color8565->ch.a;
            ch_r = color8565->ch.r;
            ch_g = color8565->ch.g;
            ch_b = color8565->ch.b;
            color8565->ch.a = scui_clamp(ch_a + e_ch_a, 0, (1 << 8) - 1);
            color8565->ch.r = scui_clamp(ch_r + e_ch_r, 0, (1 << 5) - 1);
            color8565->ch.g = scui_clamp(ch_g + e_ch_g, 0, (1 << 6) - 1);
            color8565->ch.b = scui_clamp(ch_b + e_ch_b, 0, (1 << 5) - 1);
            ch_a = color8565->ch.a - ch_a;
            ch_r = color8565->ch.r - ch_r;
            ch_g = color8565->ch.g - ch_g;
            ch_b = color8565->ch.b - ch_b;
        }
        
        /* 误差扩散 */
        
        // (x+1,y):7/16
        if (idx_item + 1 < draw_area.w) {
            err_c_ch_a[idx_item + 1] += ch_a * 7 / 16;
            err_c_ch_r[idx_item + 1] += ch_r * 7 / 16;
            err_c_ch_g[idx_item + 1] += ch_g * 7 / 16;
            err_c_ch_b[idx_item + 1] += ch_b * 7 / 16;
        }
        if (idx_line + 1 < draw_area.h) {
            // (x,y+1):5/16
            if (idx_item + 0 < draw_area.w) {
                err_n_ch_a[idx_item + 0] += ch_a * 5 / 16;
                err_n_ch_r[idx_item + 0] += ch_r * 5 / 16;
                err_n_ch_g[idx_item + 0] += ch_g * 5 / 16;
                err_n_ch_b[idx_item + 0] += ch_b * 5 / 16;
            }
            // (x-1,y+1):3/16
            if (idx_item - 1 >= 0) {
                err_n_ch_a[idx_item - 1] += ch_a * 3 / 16;
                err_n_ch_r[idx_item - 1] += ch_r * 3 / 16;
                err_n_ch_g[idx_item - 1] += ch_g * 3 / 16;
                err_n_ch_b[idx_item - 1] += ch_b * 3 / 16;
            }
            // (x+1,y+1):1/16
            if (idx_item + 1 < draw_area.w) {
                err_n_ch_a[idx_item + 1] += ch_a * 1 / 16;
                err_n_ch_r[idx_item + 1] += ch_r * 1 / 16;
                err_n_ch_g[idx_item + 1] += ch_g * 1 / 16;
                err_n_ch_b[idx_item + 1] += ch_b * 1 / 16;
            }
        }
        
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
    }
        /* 最后一行直接跳过即可 */
        if (idx_line == draw_area.h - 1)
            continue;
        
        /* 同步误差到上一行 */
        scui_draw_byte_copy(true, err_c_ch_a, err_n_ch_a, error_color_l);
        scui_draw_byte_copy(true, err_c_ch_r, err_n_ch_r, error_color_l);
        scui_draw_byte_copy(true, err_c_ch_g, err_n_ch_g, error_color_l);
        scui_draw_byte_copy(true, err_c_ch_b, err_n_ch_b, error_color_l);
        memset(err_n_ch_a, 0, error_color_l);
        memset(err_n_ch_r, 0, error_color_l);
        memset(err_n_ch_g, 0, error_color_l);
        memset(err_n_ch_b, 0, error_color_l);
    }
    
    SCUI_MEM_FREE(error_color_c);
    SCUI_MEM_FREE(error_color_n);
    return;
    #else
    #endif
    
    SCUI_LOG_ERROR("unknown dither");
    SCUI_ASSERT(false);
}

/*@brief 区域模糊(可以使用GPU-blur加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_blur(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_blur.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_blur.dst_clip;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    #define BLUR_SCALE 5
    static const scui_multi_t blur_kernel[BLUR_SCALE][BLUR_SCALE] = {
        #if 0
        #elif 1 /* 平滑卷积核 */
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        #elif 1 /* 高斯卷积核 */
        {1,  4,  7,  4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1,  4,  7,  4, 1},
        #else
        #endif
    };
    
    scui_coord_t  scale  = BLUR_SCALE;
    scui_multi_t *kernel = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_multi_t) * scale * scale);
    scui_draw_byte_copy(true, kernel, blur_kernel, sizeof(scui_multi_t) * scale * scale);
    SCUI_ASSERT(scale % 2 != 0 && scale > 1);    /* 卷积核必须奇数 */
    
    /* 卷积核<s> */
    scui_multi_t kernel_sum = 0;
    scui_multi_t kernel_dif = 0;
    scui_multi_t kernel_cof = 8;
    for (scui_coord_t idx_j = 0; idx_j < scale; idx_j++)
    for (scui_coord_t idx_i = 0; idx_i < scale; idx_i++)
         kernel_sum += kernel[idx_j * scale + idx_i];
         kernel_cof *= kernel_sum;
    for (scui_coord_t idx_j = 0; idx_j < scale; idx_j++)
    for (scui_coord_t idx_i = 0; idx_i < scale; idx_i++) {
         kernel[idx_j * scale + idx_i] *= kernel_cof;
         kernel_dif += kernel[idx_j * scale + idx_i];
    }
    kernel_cof *= kernel_sum;
    kernel[scale / 2 * scale + scale / 2] += kernel_cof - kernel_dif;
    /* 卷积核<e> */
    
    /* 同步滑动窗口(保存未处理前数据) */
    uint8_t *pixel_buf = SCUI_MEM_ALLOC(scui_mem_type_graph, dis_line * scale);
    for (scui_multi_t idx_scale = 0; idx_scale < scale; idx_scale++) {
         scui_multi_t ofs_scale = scui_clamp(idx_scale - scale / 2, 0, draw_area.h - 1);
         
         uint8_t *dst_ofs = dst_addr + ofs_scale * dst_line;
         uint8_t *buf_ofs = pixel_buf + idx_scale * dis_line;
         scui_draw_byte_copy(true, buf_ofs, dst_ofs, dis_line);
    }
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
         uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
         scui_color_wt_t pixel = 0, ch_a = 0, ch_r = 0, ch_g = 0, ch_b = 0;
        
        if (dst_surface->format == scui_pixel_cf_bmp565) {
            for (int8_t idx_j = 0; idx_j < scale; idx_j++)
            for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
                scui_coord_t pixel_ofs = idx_j * dis_line;
                pixel_ofs += scui_clamp(idx_i + idx_item - scale / 2, 0, draw_area.w - 1) * dst_byte;
                
                scui_color565_t *color565_t = &pixel_buf[pixel_ofs];
                ch_r += (uint32_t)color565_t->ch.r * kernel[idx_j * scale + idx_i];
                ch_g += (uint32_t)color565_t->ch.g * kernel[idx_j * scale + idx_i];
                ch_b += (uint32_t)color565_t->ch.b * kernel[idx_j * scale + idx_i];
            }
            scui_color565_t *color565 = &pixel;
            color565->ch.r = ch_r / kernel_cof;
            color565->ch.g = ch_g / kernel_cof;
            color565->ch.b = ch_b / kernel_cof;
        }
        
        if (dst_surface->format == scui_pixel_cf_bmp8565) {
            for (int8_t idx_j = 0; idx_j < scale; idx_j++)
            for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
                scui_coord_t pixel_ofs = idx_j * dis_line;
                pixel_ofs += scui_clamp(idx_i + idx_item - scale / 2, 0, draw_area.w - 1) * dst_byte;
                
                scui_color8565_t *color8565_t = &pixel_buf[pixel_ofs];
                ch_a += (uint32_t)color8565_t->ch.a * kernel[idx_j * scale + idx_i];
                ch_r += (uint32_t)color8565_t->ch.r * kernel[idx_j * scale + idx_i];
                ch_g += (uint32_t)color8565_t->ch.g * kernel[idx_j * scale + idx_i];
                ch_b += (uint32_t)color8565_t->ch.b * kernel[idx_j * scale + idx_i];
            }
            scui_color8565_t *color8565 = &pixel;
            color8565->ch.a = ch_a / kernel_sum;
            color8565->ch.r = ch_r / kernel_sum;
            color8565->ch.g = ch_g / kernel_sum;
            color8565->ch.b = ch_b / kernel_sum;
        }
        
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
    }
        /* 最后一行直接跳过即可 */
        if (idx_line == draw_area.h - 1)
            continue;
        
        /* 向上滑动一行 */
        for (scui_multi_t idx_scale = 1; idx_scale < scale; idx_scale++) {
             uint8_t *buf_ofs0 = pixel_buf + (idx_scale - 1) * dis_line;
             uint8_t *buf_ofs1 = pixel_buf + (idx_scale + 0) * dis_line;
             scui_draw_byte_copy(true, buf_ofs0, buf_ofs1, dis_line);
        }
        /* 更新最后一行 */
        scui_multi_t idx_scale = scale - 1;
        scui_multi_t ofs_scale = scui_clamp(idx_line + scale / 2 + 1, 0, draw_area.h - 1);
        uint8_t *dst_ofs = dst_addr + ofs_scale * dst_line;
        uint8_t *buf_ofs = pixel_buf + idx_scale * dis_line;
        scui_draw_byte_copy(true, buf_ofs, dst_ofs, dis_line);
    }
    
    SCUI_MEM_FREE(pixel_buf);
    SCUI_MEM_FREE(kernel);
}

/*@brief 区域填充渐变像素点(可以使用DMA-fill-grad加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_grad(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_grad.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_grad.dst_clip;
    scui_area_t    *src_clip    = &draw_dsc->area_grad.src_clip;
    scui_color_t    src_color   =  draw_dsc->area_grad.src_color;
    scui_alpha_t    src_alpha   =  draw_dsc->area_grad.src_alpha;
    scui_coord_t    src_way     =  draw_dsc->area_grad.src_way;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_clip != NULL && (src_way == 0 || src_way == 1));
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    
    scui_color_wt_t src_pixel_s = 0;
    scui_color_wt_t src_pixel_e = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel_s, src_color.color_s);
    scui_pixel_by_color(dst_surface->format, &src_pixel_e, src_color.color_e);
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        uint8_t pct_100 = 0;    /* 水平渐变:0; 垂直渐变:1; */
        if (src_way == 0) pct_100 = idx_item * 100 / src_clip->w;
        if (src_way == 1) pct_100 = idx_line * 100 / src_clip->h;
        
        /* 这一部分会重复计算多次,这里不做优化,否则对src_way分开 */
        scui_color_wt_t src_pixel_a = src_pixel_s;
        scui_pixel_mix_with(dst_surface->format, &src_pixel_a,
            dst_surface->format, &src_pixel_e, scui_alpha_pct(pct_100));
        
        if (src_alpha == scui_alpha_cover)
            scui_pixel_by_cf(dst_surface->format, dst_ofs, &src_pixel_a);
        else
            scui_pixel_mix_with(dst_surface->format, dst_ofs,
                dst_surface->format, &src_pixel_a, src_alpha);
        }
    }
}

/*@brief 区域序列渐变像素点(暂未知优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_grads(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_grads.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_grads.dst_clip;
    scui_color_t   *src_grad_s  =  draw_dsc->area_grads.src_grad_s;
    scui_coord_t    src_grad_n  =  draw_dsc->area_grads.src_grad_n;
    scui_color_t    src_filter  =  draw_dsc->area_grads.src_filter;
    scui_alpha_t    src_alpha   =  draw_dsc->area_grads.src_alpha;
    scui_coord_t    src_way     =  draw_dsc->area_grads.src_way;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_grad_s != NULL && src_grad_n >= 2);
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    scui_color_wt_t  filter = 0;
    scui_color_wt_t *src_grad_l = SCUI_MEM_ALLOC(scui_mem_type_mix, src_grad_n * sizeof(scui_color_wt_t));
    scui_pixel_by_color(dst_surface->format, &filter, src_filter.color_f);
    for (scui_coord_t idx = 0; idx < src_grad_n; idx++)
        scui_pixel_by_color(dst_surface->format, &src_grad_l[idx], src_grad_s[idx].color);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    /* 注意区域对齐坐标 */
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        /* 过滤色调,去色 */
        scui_color_wt_t dst_pixel = 0;
        scui_pixel_by_cf(dst_surface->format, &dst_pixel, dst_ofs);
        if (src_filter.filter && dst_pixel == filter)
            continue;
        
        /* 提取底色色调:以白色(0xFFFFFFFF)为最大浓度0xFF */
        uint8_t grey = scui_grey_by_pixel(dst_surface->format, &dst_pixel);
        
        scui_multi_t pct_scale = 0;
        scui_multi_t idx_grad = -1;
        if (src_way == 0) {
            pct_scale = idx_item * (src_grad_n - 1) * SCUI_SCALE_COF / draw_area.w;
            idx_grad  = idx_item * (src_grad_n - 1) / draw_area.w;
        }
        if (src_way == 1) {
            pct_scale = idx_line * (src_grad_n - 1) * SCUI_SCALE_COF / draw_area.h;
            idx_grad  = idx_line * (src_grad_n - 1) / draw_area.h;
        }
        
        scui_multi_t idx_pct = ((pct_scale - (idx_grad << SCUI_SCALE_OFS)) * 100) >> SCUI_SCALE_OFS;
        SCUI_ASSERT(idx_grad >= 0 && idx_grad <= src_grad_n - 2);
        
        scui_color_wt_t src_pixel_a = src_grad_l[idx_grad];
        scui_pixel_mix_with(dst_surface->format, &src_pixel_a,
            dst_surface->format, &src_grad_l[idx_grad + 1], scui_alpha_pct(idx_pct));
        
        scui_pixel_mix_alpha(dst_surface->format, &src_pixel_a, grey);
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &src_pixel_a);
    }
    
    SCUI_MEM_FREE(src_grad_l);
}

/*@brief 区域透明过滤像素点(暂未知优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_afilter(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_afilter.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_afilter.dst_clip;
    scui_surface_t *src_surface =  draw_dsc->area_afilter.src_surface;
    scui_area_t    *src_clip    = &draw_dsc->area_afilter.src_clip;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    if (src_surface->alpha == scui_alpha_trans)
        return;
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   /* v:vaild */
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    
    /* 在dst_surface.clip中的dst_clip_v中每个像素点混合到src_surface.clip中的src_clip_v中 */
    scui_coord_t dst_bits = scui_pixel_bits(dst_surface->format);
    scui_coord_t src_bits = scui_pixel_bits(src_surface->format);
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_coord_t src_byte = scui_pixel_bits(src_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t src_line = src_surface->hor_res * src_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * dst_byte;
    uint8_t *src_addr = src_surface->pixel + src_clip_v.y * src_line + src_clip_v.x * src_byte;
    
    /* src_surface必须是alpha类型 */
    if (src_surface->format == scui_pixel_cf_alpha4 ||
        src_surface->format == scui_pixel_cf_alpha8) {
        
        scui_multi_t dst_pixel_ofs = dst_clip_v.y * dst_surface->hor_res + dst_clip_v.x;
        scui_multi_t src_pixel_ofs = src_clip_v.y * src_surface->hor_res + src_clip_v.x;
        dst_addr = dst_surface->pixel + dst_pixel_ofs * dst_byte;
        src_addr = src_surface->pixel;
        
        scui_multi_t  alpha_len = 1 << src_bits;
        scui_alpha_t *alpha_table = SCUI_MEM_ZALLOC(scui_mem_type_graph, sizeof(scui_alpha_t) * alpha_len);
        
        /* 将src_surface中的alpha值作用到dst_surface上 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
            uint32_t idx_ofs = src_pixel_ofs + idx_line * src_surface->hor_res + idx_item;
            uint8_t *src_ofs = src_addr + idx_ofs / (8 / src_bits);
            uint8_t  grey = scui_grey_bpp_x(*src_ofs, src_bits, idx_ofs % (8 / src_bits));
            uint8_t  grey_idx = (uint16_t)grey * (alpha_len - 1) / 0xFF;
            
            if (grey_idx != 0 && alpha_table[grey_idx] == 0)
                alpha_table[grey_idx] = scui_alpha_mix(src_surface->alpha, grey);
            
            scui_pixel_mix_alpha(dst_surface->format, dst_ofs, alpha_table[grey_idx]);
        }
        
        SCUI_MEM_FREE(alpha_table);
        return;
    }
    
    SCUI_LOG_ERROR("unsupported alpha filter:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}
