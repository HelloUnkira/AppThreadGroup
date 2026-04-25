/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域点采样
 *@param surface 画布实例
 *@param clip    画布剪切域
 *@param point2  近似点
 *@param color   采样值
 *@retval 成功失败
 */
static bool scui_draw_ctx_sample(
    scui_surface_t *surface, scui_area_t     *clip,
    scui_point2_t  *point2,  scui_color_wt_t *sample_color)
{
    #if 1
    /* 双线性插值采样 */
    scui_point_t point = {.x = point2->x,.y = point2->y,};
    scui_coord_t dx = (point2->x < point.x ? 0 : point2->x - point.x) * SCUI_SCALE_COF;
    scui_coord_t dy = (point2->y < point.y ? 0 : point2->y - point.y) * SCUI_SCALE_COF;
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
    for (scui_coord_t idx = 0; idx < 4 && scui_area_point(clip, &pos4[idx]); idx++) {
        uint8_t *buf_ofs = scui_surface_pixel_ofs(surface, pos4[idx].y, pos4[idx].x);
        scui_multi_t cof_dxy = avg4[idx].x * avg4[idx].y;
        
        if (surface->format == scui_pixel_cf_bmp565) {
            scui_color565_t *color565 = buf_ofs;
            del_r += color565->ch.r * cof_dxy;
            del_g += color565->ch.g * cof_dxy;
            del_b += color565->ch.b * cof_dxy;
        }
        
        if (surface->format == scui_pixel_cf_bmp8565) {
            scui_color8565_t *color8565 = buf_ofs;
            del_a += color8565->ch.a * cof_dxy;
            del_r += color8565->ch.r * cof_dxy;
            del_g += color8565->ch.g * cof_dxy;
            del_b += color8565->ch.b * cof_dxy;
        }
        
        val_cnt++;
    }
    
    if (val_cnt == 4) {
        del_a >>= SCUI_SCALE_OFS; del_a >>= SCUI_SCALE_OFS;
        del_r >>= SCUI_SCALE_OFS; del_r >>= SCUI_SCALE_OFS;
        del_g >>= SCUI_SCALE_OFS; del_g >>= SCUI_SCALE_OFS;
        del_b >>= SCUI_SCALE_OFS; del_b >>= SCUI_SCALE_OFS;
        
        if (surface->format == scui_pixel_cf_bmp565) {
            scui_color565_t *color565 = sample_color;
            color565->ch.r = del_r;
            color565->ch.g = del_g;
            color565->ch.b = del_b;
            return true;
        }
        if (surface->format == scui_pixel_cf_bmp8565) {
            scui_color8565_t *color8565 = sample_color;
            color8565->ch.a = del_a;
            color8565->ch.r = del_r;
            color8565->ch.g = del_g;
            color8565->ch.b = del_b;
            return true;
        }
    }
    
    return false;
    #endif
    
    SCUI_LOG_ERROR("unsupported sample");
    SCUI_ASSERT(false);
}

/*@brief 绘制字节拷贝(可以使用DMA-copy加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_byte_copy(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    void   *dst_addr = draw_dsc->byte_copy.dst_addr;
    void   *src_addr = draw_dsc->byte_copy.src_addr;
    uint32_t src_len = draw_dsc->byte_copy.src_len;
    /* draw dsc args<e> */
    /* */
    
    memcpy(dst_addr, src_addr, src_len);
}

/*@brief 区域填充像素点(可以使用DMA-fill加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_fill(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_fill.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_fill.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->area_fill.src_alpha;
    scui_color_t    src_color   =  draw_dsc->area_fill.src_color;
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
    
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    /* 在dst_surface.clip中的draw_area中填满pixel */
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip->y, dst_clip->x);
    scui_multi_t dis_line = draw_area.w * dst_surface->pbyte;
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_alpha == scui_alpha_cover) {
        /* 先填充第一行像素点 */
        for (scui_multi_t idx = 0; idx < draw_area.w; idx++) {
             uint8_t *dst_ofs = dst_addr + idx * dst_surface->pbyte;
             scui_pixel_by_cf(dst_surface->format, dst_ofs, &src_pixel);
        }
        /* 后使用第一行像素点填充其他行像素点 */
        for (scui_multi_t idx = 1; idx < draw_area.h; idx++) {
             uint8_t *dst_ofs = dst_addr + idx * dst_surface->stride;
             scui_draw_byte_copy(true, dst_ofs, dst_addr, dis_line);
        }
    } else {
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
            scui_pixel_mix_with(dst_surface->format, dst_ofs,
                dst_surface->format, &src_pixel, src_alpha);
        }
    }
}

/*@brief 区域拷贝(可以使用DMA-copy加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_copy(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_copy.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_copy.dst_clip;
    scui_surface_t *src_surface =  draw_dsc->area_copy.src_surface;
    scui_area_t    *src_clip    = &draw_dsc->area_copy.src_clip;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    /* scui_pixel_alpha_in(dst_surface->format) */
    if (dst_surface->alpha  != scui_alpha_cover ||
        src_surface->alpha  != scui_alpha_cover ||
        dst_surface->format != src_surface->format) {
        SCUI_LOG_ERROR("unsupported copy");
        return;
    }
    
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
    
    /* 在dst_surface.clip中的dst_clip_v中拷贝到src_surface.clip中的src_clip_v中 */
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    uint8_t *src_addr = scui_surface_pixel_ofs(src_surface, src_clip_v.y, src_clip_v.x);
    scui_multi_t dis_line = draw_area.w * dst_surface->pbyte;
    
    /* 跨度对齐为单次拷贝 */
    if (dis_line == dst_surface->stride &&
        dis_line == src_surface->stride) {
        dis_line *= draw_area.h;
        SCUI_ASSERT(dst_clip_v.x == 0);
        SCUI_ASSERT(src_clip_v.x == 0);
        scui_draw_byte_copy(true, dst_addr, src_addr, dis_line);
        return;
    }
    
    /* 注意区域对齐坐标 */
    for (scui_multi_t idx = 0; idx < draw_area.h; idx++) {
         uint8_t *dst_ofs = dst_addr + idx * dst_surface->stride;
         uint8_t *src_ofs = src_addr + idx * src_surface->stride;
         scui_draw_byte_copy(true, dst_ofs, src_ofs, dis_line);
    }
}

/*@brief 区域混合(可以使用DMA2D-blend加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_blend(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_blend.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_blend.dst_clip;
    scui_surface_t *src_surface =  draw_dsc->area_blend.src_surface;
    scui_area_t    *src_clip    = &draw_dsc->area_blend.src_clip;
    scui_color_t    src_color   =  draw_dsc->area_blend.src_color;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    if (src_surface->alpha == scui_alpha_trans)
        return;
    
    /* 全覆盖混合:直接copy */ 
    if (dst_surface->alpha  == scui_alpha_cover &&
        src_surface->alpha  == scui_alpha_cover &&
        dst_surface->format == src_surface->format &&
       !scui_pixel_alpha_in(src_surface->format) && !src_color.filter) {
        scui_draw_area_copy(true, dst_surface, *dst_clip, src_surface, *src_clip);
        return;
    }
    
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
    
    /* 像素格式不带透明度, 像素格式带透明度 */
    if (scui_pixel_type_bmp(dst_surface->format) && scui_pixel_type_bmp(src_surface->format)) {
        
        scui_color_wt_t filter = 0;
        scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
        
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
            uint8_t *src_ofs = src_addr + scui_surface_pbyte_ofs(src_surface, idx_line, idx_item);
            
            if (src_color.filter) {
                scui_color_wt_t color = 0;
                scui_pixel_by_cf(src_surface->format, &color, src_ofs);
                if (color == filter)
                    continue;
            }
            
            scui_pixel_mix_with(dst_surface->format, dst_ofs,
                src_surface->format, src_ofs, src_surface->alpha);
        }
        return;
    }
    
    /* pixel cover:(调色板) */
    if (src_surface->format == scui_pixel_cf_alpha4 ||
        src_surface->format == scui_pixel_cf_alpha8) {
        scui_coord_t dst_bits = scui_pixel_bits(dst_surface->format);
        scui_coord_t src_bits = scui_pixel_bits(src_surface->format);
        scui_multi_t dst_ofs_p = scui_surface_point_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
        scui_multi_t src_ofs_p = scui_surface_point_ofs(src_surface, src_clip_v.y, src_clip_v.x);
        dst_addr = dst_surface->pixel + dst_ofs_p * dst_surface->pbyte;
        src_addr = src_surface->pixel;
        
        /* 调色板数组(为空时计算,有时直接取): */
        scui_multi_t grey_len = 1 << src_bits;
        scui_multi_t grey_size = sizeof(scui_color_wt_t) * grey_len;
        scui_color_wt_t *grey_table = SCUI_MEM_ZALLOC(scui_mem_type_graph, grey_size);
        scui_color_wt_t  filter = 0;
        /* 起始色调和结束色调固定 */
        scui_pixel_by_color(dst_surface->format, &grey_table[0], src_color.color_e);
        scui_pixel_by_color(dst_surface->format, &grey_table[grey_len - 1], src_color.color_s);
        scui_pixel_by_color(dst_surface->format, &filter, src_color.color_f);
        bool pixel_no_grad = grey_table[0] == grey_table[grey_len - 1];
        uint16_t mask = (1 << src_bits) - 1;
        
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr  + scui_surface_pbyte_ofs(dst_surface, idx_line, idx_item);
            uint32_t idx_ofs = src_ofs_p + scui_surface_point_ofs(src_surface, idx_line, idx_item);
            uint8_t *src_ofs = src_addr  + idx_ofs / (8 / src_bits);
            uint8_t  grey = scui_pixel_grey_bpp_x(*src_ofs, src_bits, idx_ofs % (8 / src_bits));
            uint8_t  grey_idx = pixel_no_grad ? 0 : (uint16_t)grey * (grey_len - 1) / 0xFF;
            
            if (grey_idx != 0 && grey_idx != grey_len - 1)
            if (grey_table[grey_idx] == 0x00) {
                uint32_t *pixel_1 = &grey_table[0];
                uint32_t *pixel_2 = &grey_table[grey_len - 1];
                
                grey_table[grey_idx] = grey_table[0];
                scui_pixel_mix_with(dst_surface->format, &grey_table[grey_idx],
                    dst_surface->format, &grey_table[grey_len - 1], scui_alpha_cover - grey);
            }
            
            scui_color_wt_t grey_color = grey_table[grey_idx];
            scui_pixel_mix_alpha(dst_surface->format, &grey_color, grey);
            /* 过滤色调,去色 */
            if (src_color.filter && grey_color == filter)
                continue;
            
            scui_alpha_t alpha = (uint32_t)src_surface->alpha * grey / scui_alpha_cover;
            scui_pixel_mix_with(dst_surface->format, dst_ofs,
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

/*@brief 图形变换迁移(可以使用DMA2D-blend加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_2d_blend(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_2d_blend.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_2d_blend.dst_clip;
    scui_surface_t *src_surface =  draw_dsc->area_2d_blend.src_surface;
    scui_area_t    *src_clip    = &draw_dsc->area_2d_blend.src_clip;
    scui_color_t    src_color   =  draw_dsc->area_2d_blend.src_color;
    scui_point_t   *src_scale   = &draw_dsc->area_2d_blend.src_scale;
    scui_multi_t    src_angle   =  draw_dsc->area_2d_blend.src_angle;
    scui_point_t   *dst_anchor  = &draw_dsc->area_2d_blend.dst_anchor;
    scui_point_t   *src_center  = &draw_dsc->area_2d_blend.src_center;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   /* v:vaild */
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    scui_coord3_t src_angle3 = (scui_coord3_t)src_angle / SCUI_SCALE_COF;
    scui_point2_t src_scale2 = {
        .x = (scui_coord3_t)src_scale->x / SCUI_SCALE_COF,
        .y = (scui_coord3_t)src_scale->y / SCUI_SCALE_COF,
    };
    scui_coord3_t sin_a = scui_sin(SCUI_RAD_BY_A(src_angle3));
    scui_coord3_t cos_a = scui_cos(SCUI_RAD_BY_A(src_angle3));
    
    /* 利用原图进行一次源初变换,以修饰限制目标区域 */
    if (1) {
        scui_face2_t face2 = {0};
        scui_area2_by_area(&face2, &src_area);
        
        dst_area.x1 = scui_coord_max;
        dst_area.y1 = scui_coord_max;
        dst_area.x2 = scui_coord_min;
        dst_area.y2 = scui_coord_min;
        /* 对四个图像的角进行一次正变换 */
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_coord3_t ty = face2.point2[idx].y - src_center->y;
            scui_coord3_t tx = face2.point2[idx].x - src_center->x;
            scui_coord3_t sy = ty * src_scale2.y;
            scui_coord3_t sx = tx * src_scale2.x;
            scui_coord3_t ry = + sx * sin_a + sy * cos_a;
            scui_coord3_t rx = + sx * cos_a - sy * sin_a;
            ty = ry + dst_anchor->y;
            tx = rx + dst_anchor->x;
            
            dst_area.x1 = scui_min(dst_area.x1, tx - 1.5f);
            dst_area.y1 = scui_min(dst_area.y1, ty - 1.5f);
            dst_area.x2 = scui_max(dst_area.x2, tx + 1.5f);
            dst_area.y2 = scui_max(dst_area.y2, ty + 1.5f);
        }
        
        scui_area_m_by_s(&dst_area, &dst_area);
        if (!scui_area_inter2(&dst_clip_v, &dst_area))
             return;
    }
    
    /* 在src_surface.clip中的src_clip_v中每个像素点混合到dst_surface.clip中的dst_clip_v中 */
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    uint8_t *src_addr = scui_surface_pixel_ofs(src_surface, src_clip_v.y, src_clip_v.x);
    
    if (scui_pixel_type_bmp(dst_surface->format) && scui_pixel_type_bmp(src_surface->format)) {
        
        scui_color_wt_t filter = 0;
        scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
        
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = dst_clip_v.y; idx_line < dst_clip_v.y + dst_clip_v.h; idx_line++)
        for (scui_multi_t idx_item = dst_clip_v.x; idx_item < dst_clip_v.x + dst_clip_v.w; idx_item++) {
            scui_point2_t point2 = {0};
            /* 反扫描结果坐标对每一个坐标进行逆变换 */
            scui_coord3_t y = idx_line - dst_anchor->y;
            scui_coord3_t x = idx_item - dst_anchor->x;
            point2.y = - x * sin_a + y * cos_a;
            point2.x = + x * cos_a + y * sin_a;
            point2.y /= src_scale2.y;
            point2.x /= src_scale2.x;
            point2.y += src_center->y;
            point2.x += src_center->x;
            point2.y += src_clip_v.y;
            point2.x += src_clip_v.x;
            
            scui_color_wt_t sample_color = 0;
            uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, idx_line, idx_item);
            if (scui_draw_ctx_sample(src_surface, &src_clip_v, &point2, &sample_color)) {
                
                if (src_color.filter) {
                    scui_color_wt_t color = 0;
                    scui_pixel_by_cf(src_surface->format, &color, &sample_color);
                    if (color == filter)
                        continue;
                }
                
                scui_pixel_mix_with(dst_surface->format, dst_ofs,
                    src_surface->format, &sample_color, src_surface->alpha);
                
                continue;
            }
            
            /* 这里使用点对点上色 */
            scui_point_t point = {
                .y = (scui_coord_t)point2.y,
                .x = (scui_coord_t)point2.x,
            };
            /* 逆变换的结果落在的源区域, 取样上色 */
            if (scui_area_point(&src_clip_v, &point)) {
                uint8_t *src_ofs = scui_surface_pixel_ofs(src_surface, point.y, point.x);
                
                if (src_color.filter) {
                    scui_color_wt_t color = 0;
                    scui_pixel_by_cf(src_surface->format, &color, src_ofs);
                    if (color == filter)
                        continue;
                }
                
                scui_pixel_mix_with(dst_surface->format, dst_ofs,
                    src_surface->format, src_ofs, src_surface->alpha);
                continue;
            }
        }
        return;
    }
    SCUI_LOG_ERROR("unsupported rotate blend:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}

/*@brief 图形变换迁移(可以使用VGLITE-blit加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_3d_blend(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_3d_blend.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_3d_blend.dst_clip;
    scui_surface_t *src_surface =  draw_dsc->area_3d_blend.src_surface;
    scui_area_t    *src_clip    = &draw_dsc->area_3d_blend.src_clip;
    scui_color_t    src_color   =  draw_dsc->area_3d_blend.src_color;
    scui_matrix_t  *inv_matrix  = &draw_dsc->area_3d_blend.inv_matrix;
    scui_matrix_t  *src_matrix  = &draw_dsc->area_3d_blend.src_matrix;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_surface != NULL && src_surface->pixel != NULL && src_clip != NULL);
    SCUI_ASSERT(inv_matrix  != NULL);
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   /* v:vaild */
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    /* 利用原图进行一次源初变换,以修饰限制目标区域 */
    if (src_matrix != NULL && !scui_matrix_invalid(src_matrix)) {
        
        scui_face2_t face2 = {0};
        scui_face3_t face3 = {0};
        scui_area2_by_area(&face2, &src_area);
        
        dst_area.x1 = scui_coord_max;
        dst_area.y1 = scui_coord_max;
        dst_area.x2 = scui_coord_min;
        dst_area.y2 = scui_coord_min;
        /* 对四个图像的角进行一次正变换 */
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_point3_by_point2(&face3.point3[idx], &face2.point2[idx]);
            scui_point3_transform_by_matrix(&face3.point3[idx], src_matrix);
            scui_point3_to_point2(&face3.point3[idx], &face2.point2[idx]);
            
            dst_area.x1 = scui_min(dst_area.x1, face2.point2[idx].x - 1.5);
            dst_area.y1 = scui_min(dst_area.y1, face2.point2[idx].y - 1.5);
            dst_area.x2 = scui_max(dst_area.x2, face2.point2[idx].x + 1.5);
            dst_area.y2 = scui_max(dst_area.y2, face2.point2[idx].y + 1.5);
        }
        scui_area_m_by_s(&dst_area, &dst_area);
        
        if (!scui_area_inter2(&dst_clip_v, &dst_area))
             return;
    }
    
    /* 在src_surface.clip中的src_clip_v中每个像素点混合到dst_surface.clip中的dst_clip_v中 */
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    uint8_t *src_addr = scui_surface_pixel_ofs(src_surface, src_clip_v.y, src_clip_v.x);
    
    if (scui_pixel_type_bmp(dst_surface->format) && scui_pixel_type_bmp(src_surface->format)) {
        
        scui_color_wt_t filter = 0;
        scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
        
        /* 注意区域对齐坐标 */
        for (scui_multi_t idx_line = dst_clip_v.y; idx_line < dst_clip_v.y + dst_clip_v.h; idx_line++)
        for (scui_multi_t idx_item = dst_clip_v.x; idx_item < dst_clip_v.x + dst_clip_v.w; idx_item++) {
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
            
            scui_color_wt_t sample_color = 0;
            uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, idx_line, idx_item);
            if (scui_draw_ctx_sample(src_surface, &src_clip_v, &point2, &sample_color)) {
                
                if (src_color.filter) {
                    scui_color_wt_t color = 0;
                    scui_pixel_by_cf(src_surface->format, &color, &sample_color);
                    if (color == filter)
                        continue;
                }
                
                scui_pixel_mix_with(dst_surface->format, dst_ofs,
                    src_surface->format, &sample_color, src_surface->alpha);
                
                continue;
            }
            
            /* 这里使用点对点上色 */
            scui_point_t point = {
                .y = (scui_coord_t)point2.y,
                .x = (scui_coord_t)point2.x,
            };
            /* 逆变换的结果落在的源区域, 取样上色 */
            if (scui_area_point(&src_clip_v, &point)) {
                uint8_t *src_ofs = scui_surface_pixel_ofs(src_surface, point.y, point.x);
                
                if (src_color.filter) {
                    scui_color_wt_t color = 0;
                    scui_pixel_by_cf(src_surface->format, &color, src_ofs);
                    if (color == filter)
                        continue;
                }
                
                scui_pixel_mix_with(dst_surface->format, dst_ofs,
                    src_surface->format, src_ofs, src_surface->alpha);
                continue;
            }
        }
        return;
    }
    
    SCUI_LOG_ERROR("unsupported matrix blend:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_LOG_ERROR("src_surface format:%x", src_surface->format);
    SCUI_ASSERT(false);
}

/*@brief 图形变换填色(可以使用VGLITE-blit加速优化)
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_area_3d_fill(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->area_3d_fill.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->area_3d_fill.dst_clip;
    scui_area_t    *src_clip    = &draw_dsc->area_3d_fill.src_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->area_3d_fill.src_alpha;
    scui_color_t    src_color   =  draw_dsc->area_3d_fill.src_color;
    scui_matrix_t  *inv_matrix  = &draw_dsc->area_3d_fill.inv_matrix;
    scui_matrix_t  *src_matrix  = &draw_dsc->area_3d_fill.src_matrix;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_clip != NULL);
    SCUI_ASSERT(inv_matrix != NULL);
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   /* v:vaild */
    scui_area_t src_area = *src_clip;
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         return;
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    /* 如果逆矩阵是无效矩阵, 则不做使用 */
    if (scui_matrix_invalid(inv_matrix))
        return;
    
    /* 利用原图进行一次源初变换,以修饰限制目标区域 */
    if (src_matrix != NULL && !scui_matrix_invalid(src_matrix)) {
        
        scui_face2_t face2 = {0};
        scui_face3_t face3 = {0};
        scui_area2_by_area(&face2, &src_area);
        
        dst_area.x1 = scui_coord_max;
        dst_area.y1 = scui_coord_max;
        dst_area.x2 = scui_coord_min;
        dst_area.y2 = scui_coord_min;
        /* 对四个图像的角进行一次正变换 */
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_point3_by_point2(&face3.point3[idx], &face2.point2[idx]);
            scui_point3_transform_by_matrix(&face3.point3[idx], src_matrix);
            scui_point3_to_point2(&face3.point3[idx], &face2.point2[idx]);
            
            dst_area.x1 = scui_min(dst_area.x1, face2.point2[idx].x - 1.5);
            dst_area.y1 = scui_min(dst_area.y1, face2.point2[idx].y - 1.5);
            dst_area.x2 = scui_max(dst_area.x2, face2.point2[idx].x + 1.5);
            dst_area.y2 = scui_max(dst_area.y2, face2.point2[idx].y + 1.5);
        }
        scui_area_m_by_s(&dst_area, &dst_area);
        
        if (!scui_area_inter2(&dst_clip_v, &dst_area))
             return;
    }
    
    scui_color_wt_t src_pixel = 0;
    scui_pixel_by_color(dst_surface->format, &src_pixel, src_color.color);
    /* 在src_surface.clip中的src_clip_v中每个像素点混合到dst_surface.clip中的dst_clip_v中 */
    uint8_t *dst_addr = scui_surface_pixel_ofs(dst_surface, dst_clip_v.y, dst_clip_v.x);
    
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
            uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, idx_line, idx_item);
            scui_pixel_mix_with(dst_surface->format, dst_ofs,
                dst_surface->format, &src_pixel, src_alpha);
            continue;
        }
    }
    return;
    
    SCUI_LOG_ERROR("unsupported matrix fill:");
    SCUI_LOG_ERROR("dst_surface format:%x", dst_surface->format);
    SCUI_ASSERT(false);
}
