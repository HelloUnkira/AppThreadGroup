/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域卷积(可以使用GPU加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_convolution(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_convolution.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_convolution.dst_clip;
    scui_multi_t   *kernel      =  draw_dsc->area_convolution.kernel;
    scui_coord_t    scale       =  draw_dsc->area_convolution.scale;
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
    
    scui_multi_t     pixel_num = scale * scale;
    scui_color_wt_t *pixel_mat = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_color_wt_t) * pixel_num);
    scui_multi_t    *pixel_ofs = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_multi_t) * pixel_num);
    scui_multi_t    *convo_mat = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(scui_multi_t) * pixel_num);
    scui_coord_t     convo_ofs = scale / 2;
    scui_multi_t     convo_sum = 0;
    
    /* 卷积核<s> */
    scui_draw_byte_copy(true, convo_mat, kernel, sizeof(scui_multi_t) * pixel_num);
    
    for (int8_t idx_j = 0; idx_j < scale; idx_j++)
    for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
         pixel_ofs[idx_j * scale + idx_i]  = -dst_line * (idx_j + convo_ofs - scale);
         pixel_ofs[idx_j * scale + idx_i] += -dst_byte * (idx_i + convo_ofs - scale);
         convo_sum += convo_mat[idx_j * scale + idx_i];
    }
    for (int8_t idx_j = 0; idx_j < scale; idx_j++)
    for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
         convo_mat[idx_j * scale + idx_i] *= SCUI_SCALE_COF;
         convo_mat[idx_j * scale + idx_i] /= convo_sum;
    }
    /* 卷积核<e> */
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
         uint8_t *dst_ofs = dst_addr + idx_line * dst_line + idx_item * dst_byte;
         scui_color_wt_t pixel = 0;
        
        /* 卷积<s> */
        for (int8_t idx_j = 0; idx_j < scale; idx_j++)
        for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
             scui_point_t point = {
                 .y = idx_line + idx_j + convo_ofs - scale,
                 .x = idx_item + idx_i + convo_ofs - scale,
             };
             pixel_mat[idx_j * scale + idx_i] = 0;
             if (scui_area_point(&draw_area, &point)) {
                 uint8_t *pixel_ptr = dst_ofs + pixel_ofs[idx_j * scale + idx_i];
                 scui_pixel_by_cf(dst_surface->format, &pixel_mat[idx_j * scale + idx_i], pixel_ptr);
             }
        }
        /* 卷积<e> */
        
        if (dst_surface->format == scui_pixel_cf_bmp565) {
            uint32_t ch_r = 0, ch_g = 0, ch_b = 0;
            for (int8_t idx_j = 0; idx_j < scale; idx_j++)
            for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
                scui_color565_t *color565_ij = &pixel_mat[idx_j * scale + idx_i];
                ch_r += (uint32_t)color565_ij->ch.r * convo_mat[idx_j * scale + idx_i];
                ch_g += (uint32_t)color565_ij->ch.g * convo_mat[idx_j * scale + idx_i];
                ch_b += (uint32_t)color565_ij->ch.b * convo_mat[idx_j * scale + idx_i];
            }
            scui_color565_t *color565 = &pixel;
            color565->ch.r = ch_r >> SCUI_SCALE_OFS;
            color565->ch.g = ch_g >> SCUI_SCALE_OFS;
            color565->ch.b = ch_b >> SCUI_SCALE_OFS;
        }
        
        if (dst_surface->format == scui_pixel_cf_bmp8565) {
            uint32_t ch_a = 0, ch_r = 0, ch_g = 0, ch_b = 0;
            for (int8_t idx_j = 0; idx_j < scale; idx_j++)
            for (int8_t idx_i = 0; idx_i < scale; idx_i++) {
                scui_color8565_t *color8565_ij = &pixel_mat[idx_j * scale + idx_i];
                ch_a += (uint32_t)color8565_ij->ch.a * convo_mat[idx_j * scale + idx_i];
                ch_r += (uint32_t)color8565_ij->ch.r * convo_mat[idx_j * scale + idx_i];
                ch_g += (uint32_t)color8565_ij->ch.g * convo_mat[idx_j * scale + idx_i];
                ch_b += (uint32_t)color8565_ij->ch.b * convo_mat[idx_j * scale + idx_i];
            }
            scui_color8565_t *color8565 = &pixel;
            color8565->ch.a = ch_a >> SCUI_SCALE_OFS;
            color8565->ch.r = ch_r >> SCUI_SCALE_OFS;
            color8565->ch.g = ch_g >> SCUI_SCALE_OFS;
            color8565->ch.b = ch_b >> SCUI_SCALE_OFS;
        }
        
        scui_pixel_by_cf(dst_surface->format, dst_ofs, &pixel);
    }
    
    SCUI_MEM_FREE(convo_mat);
    SCUI_MEM_FREE(pixel_mat);
    SCUI_MEM_FREE(pixel_ofs);
}

/*@brief 区域抖动(可以使用GPU-blur加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_dither(scui_draw_dsc_t *draw_dsc, uint8_t type)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_dither.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_dither.dst_clip;
    /* draw dsc args<e> */
    /* */
    
    #define  DITHER_SCALA     8
    scui_multi_t dither_kernel[DITHER_SCALA][DITHER_SCALA] = {
        #if 0
        #elif 1 /* Bayer抖动核 */
        { 0, 48, 12, 60,  3, 51, 15, 63},
        {32, 16, 44, 28, 35, 19, 47, 31},
        { 8, 56,  4, 52, 11, 59,  7, 55},
        {40, 24, 36, 20, 43, 27, 39, 23},
        { 2, 50, 14, 62,  1, 49, 13, 61},
        {34, 18, 46, 30, 33, 17, 45, 29},
        {10, 58,  6, 54,  9, 57,  5, 53},
        #else
        #error "unknown convolution kernel"
        #endif
    };
    
    scui_draw_area_convolution(true, dst_surface, *dst_clip, dither_kernel, DITHER_SCALA);
}

/*@brief 区域模糊(可以使用GPU-blur加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_blur(scui_draw_dsc_t *draw_dsc, uint8_t type)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_blur.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_blur.dst_clip;
    /* draw dsc args<e> */
    /* */
    
    #define  BLUR_SCALA     5
    scui_multi_t blur_kernel[BLUR_SCALA][BLUR_SCALA] = {
        #if 0
        #elif 0 /* 平滑模糊核 */
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        #elif 1 /* 高斯模糊核 */
        {1,  4,  7,  4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1,  4,  7,  4, 1},
        #else
        #endif
    };
    
    scui_draw_area_convolution(true, dst_surface, *dst_clip, blur_kernel, BLUR_SCALA);
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
