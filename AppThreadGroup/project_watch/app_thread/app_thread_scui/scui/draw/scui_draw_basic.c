/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字节拷贝(可以使用DMA-copy加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_byte_copy(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    void *dst_addr = draw_dsc->byte_copy.dst_addr;
    void *src_addr = draw_dsc->byte_copy.src_addr;
    uint32_t   len = draw_dsc->byte_copy.len;
    /* draw dsc args<e> */
    //
    
    memcpy(dst_addr, src_addr, len);
}

/*@brief 区域模糊(可以使用GPU-blur加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_blur(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_blur.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_blur.dst_clip;
    /* draw dsc args<e> */
    //
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
    
    #define  BLUR_SCALA         8
    uint32_t blur_sum = 0;
    uint32_t blur_cof[BLUR_SCALA][BLUR_SCALA] = {
        #if 0
        #elif 0 // 平滑模糊核
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        #elif 1 // dither模糊核
        { 0, 48, 12, 60,  3, 51, 15, 63},
        {32, 16, 44, 28, 35, 19, 47, 31},
        { 8, 56,  4, 52, 11, 59,  7, 55},
        {40, 24, 36, 20, 43, 27, 39, 23},
        { 2, 50, 14, 62,  1, 49, 13, 61},
        {34, 18, 46, 30, 33, 17, 45, 29},
        {10, 58,  6, 54,  9, 57,  5, 53},
        {42, 26, 38, 22, 41, 25, 37, 21},
        #elif 1 // 高斯模糊核
        {1,  4,  7,  4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1,  4,  7,  4, 1},
        #else
        #endif
    };
    
    uintptr_t pixel_ofs[BLUR_SCALA][BLUR_SCALA] = {0};
    scui_color_wt_t pixel_mat[BLUR_SCALA][BLUR_SCALA] = {0};
    scui_coord_t blur_ofs = BLUR_SCALA / 2;
    
    /* 模糊核<s> */
    for (int8_t idx_j = 0; idx_j < BLUR_SCALA; idx_j++)
    for (int8_t idx_i = 0; idx_i < BLUR_SCALA; idx_i++) {
         pixel_ofs[idx_j][idx_i] = 0;
         pixel_ofs[idx_j][idx_i] += - dst_line * (idx_j + blur_ofs - BLUR_SCALA);
         pixel_ofs[idx_j][idx_i] += - dst_byte * (idx_i + blur_ofs - BLUR_SCALA);
         blur_sum += blur_cof[idx_j][idx_i];
    }
    for (int8_t idx_j = 0; idx_j < BLUR_SCALA; idx_j++)
    for (int8_t idx_i = 0; idx_i < BLUR_SCALA; idx_i++) {
         blur_cof[idx_j][idx_i] *= SCUI_SCALE_COF;
         blur_cof[idx_j][idx_i] /= blur_sum;
    }
    /* 模糊核<e> */
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
         uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
         scui_color_wt_t pixel = 0;
        
        /* 模糊<s> */
        for (int8_t idx_j = 0; idx_j < BLUR_SCALA; idx_j++)
        for (int8_t idx_i = 0; idx_i < BLUR_SCALA; idx_i++) {
             scui_point_t point = {
                 .y = idx_line + idx_j + blur_ofs - BLUR_SCALA,
                 .x = idx_item + idx_i + blur_ofs - BLUR_SCALA,
             };
             pixel_mat[idx_j][idx_i] = 0;
             if (scui_area_point(&draw_area, &point)) {
                 uint8_t *pixel_ptr = dst_ofs + pixel_ofs[idx_j][idx_i];
                 scui_pixel_by_cf(dst_surface->format, &pixel_mat[idx_j][idx_i], pixel_ptr);
             }
        }
        /* 模糊<e> */
        
        if (dst_surface->format == scui_pixel_cf_bmp565) {
            uint32_t ch_r = 0, ch_g = 0, ch_b = 0;
            for (int8_t idx_j = 0; idx_j < BLUR_SCALA; idx_j++)
            for (int8_t idx_i = 0; idx_i < BLUR_SCALA; idx_i++) {
                scui_color565_t *color565_ij = &pixel_mat[idx_j][idx_i];
                ch_r += (uint32_t)color565_ij->ch.r * blur_cof[idx_j][idx_i];
                ch_g += (uint32_t)color565_ij->ch.g * blur_cof[idx_j][idx_i];
                ch_b += (uint32_t)color565_ij->ch.b * blur_cof[idx_j][idx_i];
            }
            scui_color565_t *color565 = &pixel;
            color565->ch.r = ch_r >> SCUI_SCALE_OFS;
            color565->ch.g = ch_g >> SCUI_SCALE_OFS;
            color565->ch.b = ch_b >> SCUI_SCALE_OFS;
        }
        
        if (dst_surface->format == scui_pixel_cf_bmp8565) {
            uint32_t ch_a = 0, ch_r = 0, ch_g = 0, ch_b = 0;
            for (int8_t idx_j = 0; idx_j < BLUR_SCALA; idx_j++)
            for (int8_t idx_i = 0; idx_i < BLUR_SCALA; idx_i++) {
                scui_color8565_t *color8565_ij = &pixel_mat[idx_j][idx_i];
                ch_a += (uint32_t)color8565_ij->ch.a * blur_cof[idx_j][idx_i];
                ch_r += (uint32_t)color8565_ij->ch.r * blur_cof[idx_j][idx_i];
                ch_g += (uint32_t)color8565_ij->ch.g * blur_cof[idx_j][idx_i];
                ch_b += (uint32_t)color8565_ij->ch.b * blur_cof[idx_j][idx_i];
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
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_fill.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_fill.dst_clip;
    scui_alpha_t    src_alpha   = draw_dsc->area_fill.src_alpha;
    scui_color_t    src_color   = draw_dsc->area_fill.src_color;
    /* draw dsc args<e> */
    //
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
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    scui_multi_t dis_line = draw_area.w * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    if (src_alpha == scui_alpha_cover) {
        /* 先填充第一行像素点 */
        for (scui_multi_t idx = 0; idx < draw_area.w; idx++)
            scui_pixel_by_cf(dst_surface->format, dst_addr + idx * dst_byte, &src_pixel);
        /* 后使用第一行像素点填充其他行像素点 */
        for (scui_multi_t idx = 1; idx < draw_area.h; idx++) {
            scui_draw_dsc_t draw_dsc = {
                .byte_copy.dst_addr = dst_addr + idx * dst_line,
                .byte_copy.src_addr = dst_addr,
                .byte_copy.len      = dis_line,
            };
            scui_draw_byte_copy(&draw_dsc);
        }
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

/*@brief 区域填充渐变像素点(可以使用DMA-fill-grad加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill_grad(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_fill_grad.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_fill_grad.dst_clip;
    scui_color_t    src_color   = draw_dsc->area_fill_grad.src_color;
    scui_area_t    *src_clip    = draw_dsc->area_fill_grad.src_clip;
    scui_alpha_t    src_alpha   = draw_dsc->area_fill_grad.src_alpha;
    scui_coord_t    src_way     = draw_dsc->area_fill_grad.src_way;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_clip != NULL && (src_way == 0 || src_way == 1));
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_color_wt_t src_pixel_s = 0;
    scui_color_wt_t src_pixel_e = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel_s, src_color.color_s);
    scui_pixel_by_color(dst_surface->format, &src_pixel_e, src_color.color_e);
    /* 在dst_surface.clip中的draw_area中填满pixel */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++) {
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
        
        uint8_t pct_100 = 0;
        if (src_way == 0)   /* 水平渐变 */
            pct_100 = idx_item * 100 / src_clip->w;
        if (src_way == 1)   /* 垂直渐变 */
            pct_100 = idx_line * 100 / src_clip->h;
        
        /* 这一部分会重复计算多次,这里不做优化,否则对src_way分开 */
        scui_alpha_t alpha_1 = scui_alpha_pct(100 - pct_100);
        scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
        scui_color_wt_t src_pixel_a = src_pixel_s;
        scui_pixel_mix_with(dst_surface->format, &src_pixel_a, alpha_1,
                            dst_surface->format, &src_pixel_e, alpha_2);
        
        if (src_alpha == scui_alpha_cover)
            scui_pixel_by_cf(dst_surface->format, dst_ofs, &src_pixel_a);
        else
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                                dst_surface->format, &src_pixel_a, src_alpha);
        }
    }
}

/*@brief 区域序列渐变像素点(可以使用DMA-fill-grad加速优化?存疑中???)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill_grads(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_fill_grads.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_fill_grads.dst_clip;
    scui_color_t   *src_grad_s  = draw_dsc->area_fill_grads.src_grad_s;
    scui_coord_t    src_grad_n  = draw_dsc->area_fill_grads.src_grad_n;
    scui_color_t    src_filter  = draw_dsc->area_fill_grads.src_filter;
    scui_alpha_t    src_alpha   = draw_dsc->area_fill_grads.src_alpha;
    scui_coord_t    src_way     = draw_dsc->area_fill_grads.src_way;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_grad_s != NULL && src_grad_n >= 2);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t draw_area = {0};
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&draw_area, &dst_area, dst_clip))
         return;
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip->y * dst_line + dst_clip->x * dst_byte;
    
    scui_color_wt_t  filter = 0;
    scui_color_wt_t *src_grad_l = SCUI_MEM_ALLOC(scui_mem_type_mix, src_grad_n * sizeof(scui_color_wt_t));
    scui_pixel_by_color(dst_surface->format, &filter, src_filter.color_f);
    for (scui_coord_t idx = 0; idx < src_grad_n; idx++)
        scui_pixel_by_color(dst_surface->format, &src_grad_l[idx], src_grad_s[idx].color);
    
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
        uint8_t grey = 0;
        scui_palette_by_pixel(dst_surface->format, &dst_pixel, &grey);
        
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
        scui_alpha_t alpha_1 = scui_alpha_pct(100 - idx_pct);
        scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
        
        scui_color_wt_t src_pixel_a = src_grad_l[idx_grad];
        scui_pixel_mix_with(dst_surface->format, &src_pixel_a, alpha_1,
                            dst_surface->format, &src_grad_l[idx_grad + 1], alpha_2);
        scui_pixel_mix_alpha(dst_surface->format, &src_pixel_a, grey);
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &src_pixel_a);
    }
    
    SCUI_MEM_FREE(src_grad_l);
}

/*@brief 区域拷贝(可以使用DMA-copy加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_copy(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_copy.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_copy.dst_clip;
    scui_surface_t *src_surface = draw_dsc->area_copy.src_surface;
    scui_area_t    *src_clip    = draw_dsc->area_copy.src_clip;
    /* draw dsc args<e> */
    //
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
    for (scui_multi_t idx = 0; idx < draw_area.h; idx++) {
        scui_draw_dsc_t draw_dsc = {
            .byte_copy.dst_addr = dst_addr + idx * dst_line,
            .byte_copy.src_addr = src_addr + idx * src_line,
            .byte_copy.len      = dis_line,
        };
        scui_draw_byte_copy(&draw_dsc);
    }
}

/*@brief 区域混合(可以使用DMA2D-blend加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_blend(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_blend.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_blend.dst_clip;
    scui_surface_t *src_surface = draw_dsc->area_blend.src_surface;
    scui_area_t    *src_clip    = draw_dsc->area_blend.src_clip;
    scui_color_t    src_color   = draw_dsc->area_blend.src_color;
    /* draw dsc args<e> */
    //
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    if (src_surface->alpha == scui_alpha_trans)
        return;
    
    /* 全覆盖混合:直接copy */
    if (dst_surface->alpha  == scui_alpha_cover     && src_surface->alpha  == scui_alpha_cover      &&
       (dst_surface->format == scui_pixel_cf_bmp565 || dst_surface->format == scui_pixel_cf_bmp888) &&
        dst_surface->format == src_surface->format  && !src_color.filter) {
        scui_draw_dsc_t draw_dsc = {
            .area_copy.dst_surface = dst_surface,
            .area_copy.dst_clip    = dst_clip,
            .area_copy.src_surface = src_surface,
            .area_copy.src_clip    = src_clip,
        };
        scui_draw_area_copy(&draw_dsc);
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
    
    /* pixel cover:(调色板) */
    if (src_surface->format == scui_pixel_cf_alpha4 ||
        src_surface->format == scui_pixel_cf_alpha8) {
        scui_multi_t dst_pixel_ofs = dst_clip_v.y * dst_surface->hor_res + dst_clip_v.x;
        scui_multi_t src_pixel_ofs = src_clip_v.y * src_surface->hor_res + src_clip_v.x;
        dst_addr = dst_surface->pixel + dst_pixel_ofs * dst_byte;
        src_addr = src_surface->pixel;
        
        /* 调色板数组(为空时计算,有时直接取): */
        scui_multi_t grey_len = 1 << src_bits;
        scui_color_wt_t *grey_table = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_color_wt_t) * grey_len);
        scui_color_wt_t filter = 0;
        memset(grey_table, 0, grey_len * sizeof(scui_color_wt_t));
        /* 起始色调和结束色调固定 */
        scui_pixel_by_color(dst_surface->format, &grey_table[0], src_color.color_e);
        scui_pixel_by_color(dst_surface->format, &grey_table[grey_len - 1], src_color.color_s);
        scui_pixel_by_color(dst_surface->format, &filter, src_color.color_f);
        bool pixel_no_grad = grey_table[0] == grey_table[grey_len - 1];
        uint16_t mask = (1 << src_bits) - 1;
        
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
            uint32_t idx_ofs = src_pixel_ofs + idx_line * src_surface->hor_res + idx_item;
            uint8_t *src_ofs = src_addr + idx_ofs / (8 / src_bits);
            uint8_t  grey = scui_grey_bpp_x(*src_ofs, src_bits, idx_ofs % (8 / src_bits));
            uint8_t  grey_idx = pixel_no_grad ? 0 : (uint16_t)grey * (grey_len - 1) / 0xFF;
            
            if (grey_idx != 0 && grey_idx != grey_len - 1)
            if (grey_table[grey_idx] == 0x00) {
                uint32_t *pixel_1 = &grey_table[0];
                uint32_t *pixel_2 = &grey_table[grey_len - 1];
                scui_alpha_t alpha_1 = grey;
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                
                grey_table[grey_idx] = grey_table[0];
                scui_pixel_mix_with(dst_surface->format, &grey_table[grey_idx], alpha_1,
                                    dst_surface->format, &grey_table[grey_len - 1], alpha_2);
            }
            
            scui_color_wt_t grey_color = grey_table[grey_idx];
            scui_pixel_mix_alpha(dst_surface->format, &grey_color, grey);
            /* 过滤色调,去色 */
            if (src_color.filter && grey_color == filter)
                continue;
            
            scui_alpha_t alpha = (uint32_t)src_surface->alpha * grey / scui_alpha_cover;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                dst_surface->format, &grey_table[grey_idx], alpha);
        }
        
        SCUI_MEM_FREE(grey_table);
        return;
    }
    
    SCUI_LOG_ERROR("unsupported blend:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}

/*@brief 图形变换填色(可以使用VGLITE-blit加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_fill_by_matrix(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_fill_by_matrix.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_fill_by_matrix.dst_clip;
    scui_area_t    *src_clip    = draw_dsc->area_blit_by_matrix.src_clip;
    scui_alpha_t    src_alpha   = draw_dsc->area_fill_by_matrix.src_alpha;
    scui_color_t    src_color   = draw_dsc->area_fill_by_matrix.src_color;
    scui_matrix_t  *inv_matrix  = draw_dsc->area_fill_by_matrix.inv_matrix;
    scui_matrix_t  *src_matrix  = draw_dsc->area_fill_by_matrix.src_matrix;
    /* draw dsc args<e> */
    //
    #if SCUI_DRAW_MISC_USE_MATRIX == 0
    SCUI_ASSERT(false);
    #endif
    
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_clip != NULL);
    SCUI_ASSERT(inv_matrix  != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_t src_area = *src_clip;
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    // 利用原图进行一次源初变换,以修饰限制目标区域
    if (src_matrix != NULL) {
        scui_face2_t face2 = {0};
        scui_face3_t face3 = {0};
        scui_area2_by_area(&face2, &src_area);
        
        dst_area.x1 = scui_coord_max;
        dst_area.y1 = scui_coord_max;
        dst_area.x2 = scui_coord_min;
        dst_area.y2 = scui_coord_min;
        /* 对四个图像的角进行一次正变换 */
        for (uint8_t idx = 0; idx < 4; idx++) {
            scui_point3_by_point2(&face3.point3[idx], &face2.point2[idx]);
            scui_point3_transform_by_matrix(&face3.point3[idx], src_matrix);
            scui_point3_to_point2(&face3.point3[idx], &face2.point2[idx]);
            
            dst_area.x1 = scui_min(dst_area.x1, face2.point2[idx].x - 1.5);
            dst_area.y1 = scui_min(dst_area.y1, face2.point2[idx].y - 1.5);
            dst_area.x2 = scui_max(dst_area.x2, face2.point2[idx].x + 1.5);
            dst_area.y2 = scui_max(dst_area.y2, face2.point2[idx].y + 1.5);
        }
        scui_area_m_by_s(&dst_area);
        
        if (!scui_area_inter2(&dst_clip_v, &dst_area))
             return;
    }
    
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    /* 在dst_surface.clip中的dst_clip_v中每个像素点混合到src_surface.clip中的src_clip_v中 */
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_multi_t dst_line = dst_surface->hor_res * dst_byte;
    uint8_t *dst_addr = dst_surface->pixel + dst_clip_v.y * dst_line + dst_clip_v.x * dst_byte;
    
    /* 注意区域对齐坐标 */
    for (scui_multi_t idx_line = dst_clip_v.y; idx_line < dst_clip_v.y + dst_clip_v.h; idx_line++)
    for (scui_multi_t idx_item = dst_clip_v.x; idx_item < dst_clip_v.x + dst_clip_v.w; idx_item++) {
        scui_point_t  point  = {0};
        scui_point2_t point2 = {0};
        scui_point3_t point3 = {0};
        point2.y = idx_line - 0.5;
        point2.x = idx_item - 0.5;
        /* 反扫描结果坐标对每一个坐标进行逆变换 */
        scui_point3_by_point2(&point3, &point2);
        scui_point3_transform_by_matrix(&point3, inv_matrix);
        scui_point3_to_point2(&point3, &point2);
        point2.y += src_clip_v.y - 0.5;
        point2.x += src_clip_v.x - 0.5;
        point.y = (scui_coord_t)point2.y;
        point.x = (scui_coord_t)point2.x;
        
        /* 这里使用点对点上色 */
        /* 逆变换的结果落在的源区域, 取样上色 */
        if (scui_area_point(&src_clip_v, &point)) {
            uint8_t *dst_ofs = dst_surface->pixel + idx_line * dst_line + idx_item * dst_byte;
            
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_alpha,
                                dst_surface->format, &src_pixel, src_alpha);
            continue;
        }
    }
    return;
    SCUI_LOG_ERROR("unsupported fill:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_ASSERT(false);
}

/*@brief 图形变换迁移(可以使用VGLITE-blit加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_area_blit_by_matrix(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface = draw_dsc->area_blit_by_matrix.dst_surface;
    scui_area_t    *dst_clip    = draw_dsc->area_blit_by_matrix.dst_clip;
    scui_surface_t *src_surface = draw_dsc->area_blit_by_matrix.src_surface;
    scui_area_t    *src_clip    = draw_dsc->area_blit_by_matrix.src_clip;
    scui_color_t    src_color   = draw_dsc->area_blit_by_matrix.src_color;
    scui_matrix_t  *inv_matrix  = draw_dsc->area_blit_by_matrix.inv_matrix;
    scui_matrix_t  *src_matrix  = draw_dsc->area_blit_by_matrix.src_matrix;
    /* draw dsc args<e> */
    //
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
    
    // 利用原图进行一次源初变换,以修饰限制目标区域
    if (src_matrix != NULL) {
        scui_face2_t face2 = {0};
        scui_face3_t face3 = {0};
        scui_area2_by_area(&face2, &src_area);
        
        dst_area.x1 = scui_coord_max;
        dst_area.y1 = scui_coord_max;
        dst_area.x2 = scui_coord_min;
        dst_area.y2 = scui_coord_min;
        /* 对四个图像的角进行一次正变换 */
        for (uint8_t idx = 0; idx < 4; idx++) {
            scui_point3_by_point2(&face3.point3[idx], &face2.point2[idx]);
            scui_point3_transform_by_matrix(&face3.point3[idx], src_matrix);
            scui_point3_to_point2(&face3.point3[idx], &face2.point2[idx]);
            
            dst_area.x1 = scui_min(dst_area.x1, face2.point2[idx].x - 1.5);
            dst_area.y1 = scui_min(dst_area.y1, face2.point2[idx].y - 1.5);
            dst_area.x2 = scui_max(dst_area.x2, face2.point2[idx].x + 1.5);
            dst_area.y2 = scui_max(dst_area.y2, face2.point2[idx].y + 1.5);
        }
        scui_area_m_by_s(&dst_area);
        
        if (!scui_area_inter2(&dst_clip_v, &dst_area))
             return;
    }
    
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
        
        scui_color_wt_t filter = 0;
        scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
        
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
            // 这里使用双线性插值求平均
            scui_coord_t dy = (point2.y < point.y ? 0 : point2.y - point.y) * SCUI_SCALE_COF;
            scui_coord_t dx = (point2.x < point.x ? 0 : point2.x - point.x) * SCUI_SCALE_COF;
            scui_point_t pos4[4] = {
                {.x = point.x + 0, .y = point.y + 0,},
                {.x = point.x + 1, .y = point.y + 0,},
                {.x = point.x + 0, .y = point.y + 1,},
                {.x = point.x + 1, .y = point.y + 1,},
            };
            scui_point_t avg4[4] = {
                {.x = SCUI_SCALE_COF - dx, .y = SCUI_SCALE_COF - dy,},
                {.x = dx,                  .y = SCUI_SCALE_COF - dy,},
                {.x = SCUI_SCALE_COF - dx, .y = dy,},
                {.x = dx,                  .y = dy,},
            };
            
            scui_color_wt_t del_a = 0;
            scui_color_wt_t del_r = 0;
            scui_color_wt_t del_g = 0;
            scui_color_wt_t del_b = 0;
            
            uint8_t val_cnt = 0;
            for (uint8_t idx = 0; idx < 4; idx++)
            if (scui_area_point(&src_clip_v, &pos4[idx])) {
                val_cnt++;
                
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
            
            if (val_cnt == 4) {
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
                continue;
            }
            #endif
            
            /* 这里使用点对点上色 */
            /* 逆变换的结果落在的源区域, 取样上色 */
            if (scui_area_point(&src_clip_v, &point)) {
                uint8_t *dst_ofs = dst_surface->pixel + idx_line * dst_line + idx_item * dst_byte;
                uint8_t *src_ofs = src_surface->pixel + point.y * src_line + point.x * src_byte;
                
                if (src_color.filter) {
                    scui_color_wt_t color = 0;
                    scui_pixel_by_cf(src_surface->format, &color, src_ofs);
                    if (color == filter)
                        continue;
                }
                
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - src_surface->alpha,
                                    src_surface->format, src_ofs, src_surface->alpha);
                continue;
            }
        }
        return;
    }
    
    SCUI_LOG_ERROR("unsupported blit:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}
