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
    scui_coord_t    mode        =  draw_dsc->area_dither.mode;
    scui_coord_t    level       =  draw_dsc->area_dither.level;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    bool scui_mode_dither_static  = mode == 0;
    bool scui_mode_dither_dynamic = mode == 1;
    
    /* 参数默认值(0:回写默认) */
    if (level == 0) level = 4;          /* 抖动幅度(5bit通道;6bit通道为其2倍) */
    
    /* 有序抖动核:阈值表(尺寸由表自身推导) */
    static const scui_multi_t dither_cof[][8] = {
        { 0, 48, 12, 60,  3, 51, 15, 63,},
        {32, 16, 44, 28, 35, 19, 47, 31,},
        { 8, 56,  4, 52, 11, 59,  7, 55,},
        {40, 24, 36, 20, 43, 27, 39, 23,},
        { 2, 50, 14, 62,  1, 49, 13, 61,},
        {34, 18, 46, 30, 33, 17, 45, 29,},
        {10, 58,  6, 54,  9, 57,  5, 53,},
        {42, 26, 38, 22, 41, 25, 37, 21,},
    };
    scui_coord_t dither_ver  = scui_arr_len(dither_cof);
    scui_coord_t dither_hor  = scui_arr_len(dither_cof[0]);
    scui_coord_t dither_half = dither_ver * dither_hor / 2;
    
    /* 共有部分前置:求最小交集/绘制区域/起始像素点 */
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    if (draw_area.w == 0 || draw_area.h == 0)
         return;
    
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    
    /* 静态:有序阈值直接作用(无扩散) */
    if (scui_mode_dither_static) {
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
             uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
            
            /* 取原始像素 */
            scui_color_wt_t pixel = 0;
            scui_pixel_by_cf(dst_surface->format, &pixel, dst_ofs);
            scui_coord_t ch_r = 0, ch_g = 0, ch_b = 0, ch_a = 0;
            if (dst_surface->format == scui_pixel_cf_bmp565) {
                scui_color565_t *color565 = &pixel;
                ch_r = color565->ch.r;
                ch_g = color565->ch.g;
                ch_b = color565->ch.b;
            }
            if (dst_surface->format == scui_pixel_cf_bmp8565) {
                scui_color8565_t *color8565 = &pixel;
                ch_r = color8565->ch.r;
                ch_g = color8565->ch.g;
                ch_b = color8565->ch.b;
                ch_a = color8565->ch.a;
            }
            
            /* 全黑/全透明(背景)不参与抖动 */
            if (ch_r == 0 && ch_g == 0 && ch_b == 0)
                 continue;
            if (dst_surface->format == scui_pixel_cf_bmp8565 && ch_a == 0)
                 continue;
            
            /* 抖动阈值(5bit按level;6bit按其2倍) */
            scui_coord_t factor = dither_cof[idx_line % dither_ver][idx_item % dither_hor];
            factor -= dither_half;
            
            ch_r = scui_clamp(ch_r + factor * level / dither_half, 0, (1 << 5) - 1);
            ch_g = scui_clamp(ch_g + factor * level * 2 / dither_half, 0, (1 << 6) - 1);
            ch_b = scui_clamp(ch_b + factor * level / dither_half, 0, (1 << 5) - 1);
            
            /* 写回(不抖动alpha,避免半透明噪点) */
            if (dst_surface->format == scui_pixel_cf_bmp565) {
                scui_color565_t *color565 = &pixel;
                color565->ch.r = ch_r;
                color565->ch.g = ch_g;
                color565->ch.b = ch_b;
            }
            if (dst_surface->format == scui_pixel_cf_bmp8565) {
                scui_color8565_t *color8565 = &pixel;
                color8565->ch.r = ch_r;
                color8565->ch.g = ch_g;
                color8565->ch.b = ch_b;
            }
            scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
        }
        return;
    }
    
    /* 动态:阈值偏移 + 误差扩散(只作用于有色部分,背景截断扩散) */
    if (scui_mode_dither_dynamic) {
        scui_coord_t *err_c[3] = {0};
        scui_coord_t *err_n[3] = {0};
        scui_coord_t  err_w = draw_area.w + 2;      /* 两端各留1列边界 */
        scui_multi_t  err_l = sizeof(scui_coord_t) * err_w;
        scui_coord_t *err_cb = SCUI_MEM_ZALLOC(scui_mem_type_graph, err_l * scui_arr_len(err_c));
        scui_coord_t *err_nb = SCUI_MEM_ZALLOC(scui_mem_type_graph, err_l * scui_arr_len(err_c));
        for (scui_coord_t idx = 0; idx < scui_arr_len(err_c); idx++) {
             err_c[idx] = err_cb + err_w * idx;
             err_n[idx] = err_nb + err_w * idx;
        }
        
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
             uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
             scui_coord_t ofs = idx_item + 1;
            
            /* 取原始像素 */
            scui_color_wt_t pixel = 0;
            scui_pixel_by_cf(dst_surface->format, &pixel, dst_ofs);
            scui_coord_t ch_r = 0, ch_g = 0, ch_b = 0, ch_a = 0;
            if (dst_surface->format == scui_pixel_cf_bmp565) {
                scui_color565_t *color565 = &pixel;
                ch_r = color565->ch.r;
                ch_g = color565->ch.g;
                ch_b = color565->ch.b;
            }
            if (dst_surface->format == scui_pixel_cf_bmp8565) {
                scui_color8565_t *color8565 = &pixel;
                ch_r = color8565->ch.r;
                ch_g = color8565->ch.g;
                ch_b = color8565->ch.b;
                ch_a = color8565->ch.a;
            }
            
            /* 全黑/全透明(背景)不参与抖动,亦不承载误差 */
            if ((ch_r == 0 && ch_g == 0 && ch_b == 0) ||
                (dst_surface->format == scui_pixel_cf_bmp8565 && ch_a == 0)) {
                for (scui_coord_t idx = 0; idx < scui_arr_len(err_c); idx++)
                     err_c[idx][ofs] = 0;
                 continue;
            }
            
            /* 抖动阈值(定点:与SCUI_SCALE_COF同域) */
            scui_coord_t factor = dither_cof[idx_line % dither_ver][idx_item % dither_hor];
            factor -= dither_half;
            scui_multi_t dither = factor * SCUI_SCALE_COF / dither_half;
            
            /* 逐通道:目标值=原值+邻域误差;就近量化;残差(有界)扩散 */
            scui_coord_t ch_max[3] = {(1 << 5) - 1, (1 << 6) - 1, (1 << 5) - 1};
            scui_coord_t ch_val[3] = {ch_r, ch_g, ch_b};
            for (scui_coord_t idx = 0; idx < scui_arr_len(ch_val); idx++) {
                 /* 定点域必须32位:6bit通道 63*SCUI_SCALE_COF 已超出 int16 范围 */
                 scui_multi_t v = ch_val[idx] * SCUI_SCALE_COF + err_c[idx][ofs];
                 scui_multi_t q = (v + dither + (SCUI_SCALE_COF / 2)) / SCUI_SCALE_COF;
                 scui_multi_t err = 0;
                
                q = scui_clamp(q, 0, ch_max[idx]);
                ch_val[idx] = q;
                
                /* Floyd-Steinberg:7/3/5/1(权重和16/16,残差有界) */
                err = v - q * SCUI_SCALE_COF;
                if (ofs + 1 < err_w)
                     err_c[idx][ofs + 1] += err * 7 / 16;
                if (ofs - 1 > 0)
                     err_n[idx][ofs - 1] += err * 3 / 16;
                     err_n[idx][ofs + 0] += err * 5 / 16;
                if (ofs + 1 < err_w)
                     err_n[idx][ofs + 1] += err * 1 / 16;
            }
            ch_r = ch_val[0];
            ch_g = ch_val[1];
            ch_b = ch_val[2];
            
            /* 写回(不抖动alpha,避免半透明噪点) */
            if (dst_surface->format == scui_pixel_cf_bmp565) {
                scui_color565_t *color565 = &pixel;
                color565->ch.r = ch_r;
                color565->ch.g = ch_g;
                color565->ch.b = ch_b;
            }
            if (dst_surface->format == scui_pixel_cf_bmp8565) {
                scui_color8565_t *color8565 = &pixel;
                color8565->ch.r = ch_r;
                color8565->ch.g = ch_g;
                color8565->ch.b = ch_b;
            }
            scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
        }
            
            /* 误差缓存滑动到下一行 */
            scui_draw_byte_copy(true, err_cb, err_nb, err_l * scui_arr_len(err_c));
            memset(err_nb, 0, err_l * scui_arr_len(err_c));
        }
        
        SCUI_MEM_FREE(err_cb);
        SCUI_MEM_FREE(err_nb);
        return;
    }
    
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
    scui_coord_t    mode        =  draw_dsc->area_blur.mode;
    scui_coord_t    skip        =  draw_dsc->area_blur.skip;
    scui_coord_t    radius      =  draw_dsc->area_blur.radius;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    bool scui_mode_blur_iir   = mode == 0;
    bool scui_mode_blur_gauss = mode == 1;
    
    /* 参数默认值(0:回写默认) */
    if (radius == 0) radius = 8;    /* 模糊半径 */
    if (skip   == 0) skip   = 1;    /* 像素跳跃计数(平衡性能与精度) */
    
    /* 卷积核:高斯核(尺寸由表自身推导) */
    static const scui_multi_t blur_kernel[][5] = {
        {1,  4,  7,  4, 1,},
        {4, 16, 26, 16, 4,},
        {7, 26, 41, 26, 7,},
        {4, 16, 26, 16, 4,},
        {1,  4,  7,  4, 1,},
    };
    scui_coord_t blur_scale = scui_arr_len(blur_kernel);
    
    /* 共有部分前置:求最小交集 */
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    /* IIR双向滤波 */
    if (scui_mode_blur_iir) {
        scui_area_t draw_area = {0};
        /* 适配裁剪区域为skip的整数倍 */
        draw_area.x = ((dst_clip_v.x + (skip - 1)) / skip) * skip;
        draw_area.y = ((dst_clip_v.y + (skip - 1)) / skip) * skip;
        draw_area.w = ((dst_clip_v.w - (skip - 1)) / skip) * skip;
        draw_area.h = ((dst_clip_v.h - (skip - 1)) / skip) * skip;
        if (scui_area_empty(&draw_area))
             return;
        
        uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
        scui_multi_t dis_line = draw_area.w * dst_surface->pbyte;
        
        /* 计算IIR滤波器强度系数,样本长度,跳跃像素点 */
        uint8_t *dst_col = NULL, *dst_row = NULL, *dst_cur = NULL;
        scui_coord_t intensity = (SCUI_SCALE_COF * radius) / (radius + 4);
        scui_coord_t intensity_inv = SCUI_SCALE_COF - intensity;
        scui_coord_t sample_len = scui_max(radius / 2, 1);
        scui_coord_t skip_byte = skip * dst_surface->pbyte;
        scui_multi_t x_ofs = 0, y_ofs = 0;
        
        /* 1. 垂直方向模糊: 从上到下 + 从下到上 */
        for (scui_coord_t x = draw_area.x; x < draw_area.x + draw_area.w; x += skip) {
            x_ofs = (x - dst_clip_v.x) * dst_surface->pbyte;
            dst_col = dst_addr + x_ofs + (draw_area.y - dst_clip_v.y) * dst_surface->stride;
            scui_color_wt_t sum[3] = {0}, s0 = 0, s1 = 0, s2 = 0;
            
            s0 = s1 = s2 = 0;   /* 初始化采样和 */
            for (scui_coord_t s = 0; s < sample_len; s++) {
                y_ofs = scui_clamp(s * skip, 0, draw_area.h - 1) * dst_surface->stride;
                
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = (dst_col + y_ofs);
                    s0 += color565_t->ch.r;
                    s1 += color565_t->ch.g;
                    s2 += color565_t->ch.b;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = (dst_col + y_ofs);
                    s0 += color8565_t->ch.r;
                    s1 += color8565_t->ch.g;
                    s2 += color8565_t->ch.b;
                }
            }
            sum[0] = (s0 << SCUI_SCALE_OFS) / sample_len;
            sum[1] = (s1 << SCUI_SCALE_OFS) / sample_len;
            sum[2] = (s2 << SCUI_SCALE_OFS) / sample_len;
            
            /* 从上到下处理列 */
            dst_cur = dst_col;
            for (scui_coord_t y = draw_area.y; y < draw_area.y + draw_area.h; y += skip) {
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.b * intensity_inv);
                    color565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.b * intensity_inv);
                    color8565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                
                dst_cur += dst_surface->stride * skip;
            }
            
            s0 = s1 = s2 = 0;   /* 初始化采样和 */
            for (scui_coord_t s = 0; s < sample_len; s++) {
                y_ofs = scui_clamp((draw_area.h - 1 - s * skip), 0, draw_area.h - 1) * dst_surface->stride;
                
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = (dst_col + y_ofs);
                    s0 += color565_t->ch.r;
                    s1 += color565_t->ch.g;
                    s2 += color565_t->ch.b;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = (dst_col + y_ofs);
                    s0 += color8565_t->ch.r;
                    s1 += color8565_t->ch.g;
                    s2 += color8565_t->ch.b;
                }
            }
            sum[0] = (s0 << SCUI_SCALE_OFS) / sample_len;
            sum[1] = (s1 << SCUI_SCALE_OFS) / sample_len;
            sum[2] = (s2 << SCUI_SCALE_OFS) / sample_len;
            
            /* 从下到上处理列 */
            dst_cur = dst_addr + x_ofs + (draw_area.y + draw_area.h - skip - dst_clip_v.y) * dst_surface->stride;
            for (scui_coord_t y = draw_area.y + draw_area.h - skip; y >= draw_area.y; y -= skip) {
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.b * intensity_inv);
                    color565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.b * intensity_inv);
                    color8565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                
                dst_cur -= dst_surface->stride * skip;
            }
        }
        
        /* 2. 水平方向模糊: 从左到右 + 从右到左 */
        for (scui_coord_t y = draw_area.y; y < draw_area.y + draw_area.h; y += skip) {
            y_ofs = (y - dst_clip_v.y) * dst_surface->stride;
            dst_row = dst_addr + y_ofs + (draw_area.x - dst_clip_v.x) * dst_surface->pbyte;
            scui_color_wt_t sum[3] = {0}, s0 = 0, s1 = 0, s2 = 0;
            
            s0 = s1 = s2 = 0;   /* 初始化采样和 */
            for (scui_coord_t s = 0; s < sample_len; s++) {
                x_ofs = scui_clamp(s * skip, 0, draw_area.w - 1) * dst_surface->pbyte;
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = (dst_row + x_ofs);
                    s0 += color565_t->ch.r;
                    s1 += color565_t->ch.g;
                    s2 += color565_t->ch.b;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = (dst_row + x_ofs);
                    s0 += color8565_t->ch.r;
                    s1 += color8565_t->ch.g;
                    s2 += color8565_t->ch.b;
                }
            }
            sum[0] = (s0 << SCUI_SCALE_OFS) / sample_len;
            sum[1] = (s1 << SCUI_SCALE_OFS) / sample_len;
            sum[2] = (s2 << SCUI_SCALE_OFS) / sample_len;
            
            /* 从左到右处理行 */
            uint8_t *dst_cur = dst_row;
            for (scui_coord_t x = draw_area.x; x < draw_area.x + draw_area.w; x += skip) {
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.b * intensity_inv);
                    color565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.b * intensity_inv);
                    color8565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                
                /* 填充跳过的像素 */
                if (skip > 1) {
                    for (scui_coord_t idx = 1; idx < skip && (x + idx) < (draw_area.x + draw_area.w); idx++)
                        scui_draw_byte_copy(true, dst_cur + idx * dst_surface->pbyte, dst_cur, dst_surface->pbyte);
                }
                
                dst_cur += skip_byte;
            }
            
            s0 = s1 = s2 = 0;   /* 初始化采样和 */
            for (scui_coord_t s = 0; s < sample_len; s++) {
                x_ofs = scui_clamp((draw_area.w - 1 - s * skip), 0, draw_area.w - 1) * dst_surface->pbyte;
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = (dst_row + x_ofs);
                    s0 += color565_t->ch.r;
                    s1 += color565_t->ch.g;
                    s2 += color565_t->ch.b;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = (dst_row + x_ofs);
                    s0 += color8565_t->ch.r;
                    s1 += color8565_t->ch.g;
                    s2 += color8565_t->ch.b;
                }
            }
            sum[0] = (s0 << SCUI_SCALE_OFS) / sample_len;
            sum[1] = (s1 << SCUI_SCALE_OFS) / sample_len;
            sum[2] = (s2 << SCUI_SCALE_OFS) / sample_len;
            
            /* 从右到左处理行 */
            dst_cur = dst_row + (draw_area.w - skip) * dst_surface->pbyte;
            for (scui_coord_t x = draw_area.x + draw_area.w - skip; x >= draw_area.x; x -= skip) {
                if (dst_surface->format == scui_pixel_cf_bmp565) {
                    scui_color565_t *color565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color565_t->ch.b * intensity_inv);
                    color565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                if (dst_surface->format == scui_pixel_cf_bmp8565) {
                    scui_color8565_t *color8565_t = dst_cur;
                    sum[0] = ((sum[0] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.r * intensity_inv);
                    sum[1] = ((sum[1] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.g * intensity_inv);
                    sum[2] = ((sum[2] * intensity) >> SCUI_SCALE_OFS) + (color8565_t->ch.b * intensity_inv);
                    color8565_t->ch.r = (sum[0] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.g = (sum[1] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                    color8565_t->ch.b = (sum[2] + (SCUI_SCALE_COF >> 1)) >> SCUI_SCALE_OFS;
                }
                
                /* 填充跳过的像素 */
                if (skip > 1) {
                    for (scui_coord_t idx = 1; idx < skip && (x - idx) >= draw_area.x; idx++)
                        scui_draw_byte_copy(true, dst_cur - idx * dst_surface->pbyte, dst_cur, dst_surface->pbyte);
                }
                
                dst_cur -= skip_byte;
            }
        }
        return;
    }
    
    /* 高斯卷积核 */
    if (scui_mode_blur_gauss) {
        scui_area_t draw_area = {0};
        draw_area.w = dst_clip_v.w;
        draw_area.h = dst_clip_v.h;
        if (scui_area_empty(&draw_area))
             return;
        
        uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
        scui_multi_t dis_line = draw_area.w * dst_surface->pbyte;
        
        scui_coord_t  scale  = blur_scale;
        scui_multi_t *kernel = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_multi_t) * scale * scale);
        scui_draw_byte_copy(true, kernel, blur_kernel, sizeof(scui_multi_t) * scale * scale);
        SCUI_ASSERT(scale % 2 != 0 && scale > 1);    /* 卷积核必须奇数 */
        
        /* 卷积核归一化<s> */
        scui_multi_t kernel_sum = 0;
        scui_multi_t kernel_dif = 0;
        scui_multi_t kernel_cof = 8;
        for (scui_coord_t idx_j = 0; idx_j < scale; idx_j++)
        for (scui_coord_t idx_i = 0; idx_i < scale; idx_i++)
             kernel_sum += kernel[idx_j * scale + idx_i];
        /* 归一化系数只乘一次(置于累加循环外,避免重复自乘溢出) */
        kernel_cof *= kernel_sum;
        for (scui_coord_t idx_j = 0; idx_j < scale; idx_j++)
        for (scui_coord_t idx_i = 0; idx_i < scale; idx_i++) {
             kernel[idx_j * scale + idx_i] *= kernel_cof;
             kernel_dif += kernel[idx_j * scale + idx_i];
        }
        kernel_cof *= kernel_sum;
        kernel[scale / 2 * scale + scale / 2] += kernel_cof - kernel_dif;
        /* 卷积核归一化<e> */
        
        /* 同步滑动窗口(保存未处理前数据) */
        uint8_t *pixel_buf = SCUI_MEM_ALLOC(scui_mem_type_graph, dis_line * scale);
        for (scui_multi_t idx_scale = 0; idx_scale < scale; idx_scale++) {
             scui_multi_t ofs_scale = scui_clamp(idx_scale - scale / 2, 0, draw_area.h - 1);
             
             uint8_t *dst_ofs = dst_addr + ofs_scale * dst_surface->stride;
             uint8_t *buf_ofs = pixel_buf + idx_scale * dis_line;
             scui_draw_byte_copy(true, buf_ofs, dst_ofs, dis_line);
        }
        
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
             uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
             scui_color_wt_t pixel = 0, ch_a = 0, ch_r = 0, ch_g = 0, ch_b = 0;
            
            if (dst_surface->format == scui_pixel_cf_bmp565) {
                for (int8_t idx_j = 0; idx_j < scale; idx_j++)
                for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
                    scui_coord_t pixel_ofs = idx_j * dis_line;
                    pixel_ofs += scui_clamp(idx_i + idx_item - scale / 2, 0, draw_area.w - 1) * dst_surface->pbyte;
                    
                    scui_color565_t *color565_t = &pixel_buf[pixel_ofs];
                    ch_r += (scui_color_wt_t)color565_t->ch.r * kernel[idx_j * scale + idx_i];
                    ch_g += (scui_color_wt_t)color565_t->ch.g * kernel[idx_j * scale + idx_i];
                    ch_b += (scui_color_wt_t)color565_t->ch.b * kernel[idx_j * scale + idx_i];
                }
                scui_color565_t *color565 = &pixel;
                color565->ch.r = (ch_r + kernel_cof / 2) / kernel_cof;
                color565->ch.g = (ch_g + kernel_cof / 2) / kernel_cof;
                color565->ch.b = (ch_b + kernel_cof / 2) / kernel_cof;
            }
            
            if (dst_surface->format == scui_pixel_cf_bmp8565) {
                for (int8_t idx_j = 0; idx_j < scale; idx_j++)
                for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
                    scui_coord_t pixel_ofs = idx_j * dis_line;
                    pixel_ofs += scui_clamp(idx_i + idx_item - scale / 2, 0, draw_area.w - 1) * dst_surface->pbyte;
                    
                    scui_color8565_t *color8565_t = &pixel_buf[pixel_ofs];
                    ch_a += (scui_color_wt_t)color8565_t->ch.a * kernel[idx_j * scale + idx_i];
                    ch_r += (scui_color_wt_t)color8565_t->ch.r * kernel[idx_j * scale + idx_i];
                    ch_g += (scui_color_wt_t)color8565_t->ch.g * kernel[idx_j * scale + idx_i];
                    ch_b += (scui_color_wt_t)color8565_t->ch.b * kernel[idx_j * scale + idx_i];
                }
                scui_color8565_t *color8565 = &pixel;
                /* 各通道统一按归一化系数缩放 */
                color8565->ch.a = (ch_a + kernel_cof / 2) / kernel_cof;
                color8565->ch.r = (ch_r + kernel_cof / 2) / kernel_cof;
                color8565->ch.g = (ch_g + kernel_cof / 2) / kernel_cof;
                color8565->ch.b = (ch_b + kernel_cof / 2) / kernel_cof;
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
            uint8_t *dst_ofs = dst_addr + ofs_scale * dst_surface->stride;
            uint8_t *buf_ofs = pixel_buf + idx_scale * dis_line;
            scui_draw_byte_copy(true, buf_ofs, dst_ofs, dis_line);
        }
        
        SCUI_MEM_FREE(pixel_buf);
        SCUI_MEM_FREE(kernel);
        return;
    }
    
    SCUI_LOG_ERROR("unknown blur");
    SCUI_ASSERT(false);
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
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    scui_multi_t dis_line = draw_area.w * dst_surface->pbyte;
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
        
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
    
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    scui_multi_t dis_line = draw_area.w * dst_surface->pbyte;
    
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
        uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
        
        /* 过滤色调,去色 */
        scui_color_wt_t dst_pixel = 0;
        scui_pixel_by_cf(dst_surface->format, &dst_pixel, dst_ofs);
        if (src_filter.filter && dst_pixel == filter)
            continue;
        
        /* 提取底色色调:以白色(0xFFFFFFFF)为最大浓度0xFF */
        uint8_t grey = scui_pixel_grey_by(dst_surface->format, &dst_pixel);
        
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
    
    /* 在src_surface.clip中的src_clip_v中每个像素点混合到dst_surface.clip中的dst_clip_v中 */
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    uint8_t *src_addr = scui_surface_pixel_ofs(src_surface, src_clip_v.y, src_clip_v.x);
    
    /* src_surface必须是alpha类型 */
    if (src_surface->format == scui_pixel_cf_alpha4 ||
        src_surface->format == scui_pixel_cf_alpha8) {
        scui_coord_t dst_bits = scui_pixel_bits(dst_surface->format);
        scui_coord_t src_bits = scui_pixel_bits(src_surface->format);
        scui_multi_t dst_ofs_p = scui_surface_point_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
        scui_multi_t src_ofs_p = scui_surface_point_ofs(src_surface, src_clip_v.y, src_clip_v.x);
        dst_addr = dst_surface->pixel + dst_ofs_p * dst_surface->pbyte;
        src_addr = src_surface->pixel;
        
        scui_multi_t  bits_num = 8 / src_bits;
        scui_multi_t  grey_len = 1 << src_bits;
        scui_multi_t  grey_size = sizeof(scui_alpha_t) * grey_len;
        scui_alpha_t *grey_table = SCUI_MEM_ZALLOC(scui_mem_type_graph, grey_size);
        
        /* 将src_surface中的alpha值作用到dst_surface上 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr  + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
            uint32_t idx_ofs = src_ofs_p + scui_surface_point_ofs(src_surface, idx_line, idx_item);
            uint8_t *src_ofs = src_addr  + idx_ofs / bits_num;
            
            /* 取出掩码灰度值并转换成颜色索引 */
            uint8_t grey = scui_pixel_grey_bpp_x(*src_ofs, src_bits, idx_ofs % bits_num);
            uint8_t grey_idx = SCUI_DIV_0xFF(grey * (grey_len - 1));
            
            /* 因为将来很大概率还会被复用到,保留它防止重复计算 */
            if (grey_idx != 0 && grey_table[grey_idx] == 0)
                grey_table[grey_idx] = scui_alpha_mix(src_surface->alpha, grey);
            
            scui_pixel_mix_alpha(dst_surface->format, dst_ofs, grey_table[grey_idx]);
        }
        
        SCUI_MEM_FREE(grey_table);
        return;
    }
    
    SCUI_LOG_ERROR("unsupported alpha filter:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}
