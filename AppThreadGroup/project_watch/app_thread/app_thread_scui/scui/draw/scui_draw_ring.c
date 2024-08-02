/*实现目标:
 *    调色板圆环绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域图像绘制(内部接口)
 *       scui_draw_ring是唯一调用者
 *@param 参数列表与调用者唯一一致
 */
static void scui_draw_ring_edge(scui_surface_t *dst_surface, scui_area_t  *dst_clip,
                                scui_point_t   *dst_center,  scui_image_t *src_image_e,
                                scui_image_t   *src_image,   scui_area_t  *src_clip,
                                scui_coord_t    src_angle_s, scui_alpha_t  src_alpha,
                                scui_coord_t    src_angle_e, scui_color_t  src_color)
{
    /* 绘制俩个端点 */
    scui_area_t edge_clip = {
        .w = src_image_e->pixel.width,
        .h = src_image_e->pixel.height,
    };
    scui_multi_t radius = (src_image->pixel.width - edge_clip.w) / 2;
    scui_multi_t center_e_sx = radius * scui_cos4096((int32_t)src_angle_s) >> 12;
    scui_multi_t center_e_sy = radius * scui_sin4096((int32_t)src_angle_s) >> 12;
    scui_multi_t center_e_ex = radius * scui_cos4096((int32_t)src_angle_e) >> 12;
    scui_multi_t center_e_ey = radius * scui_sin4096((int32_t)src_angle_e) >> 12;
    scui_area_t edge_clip_s = edge_clip;
    scui_area_t edge_clip_e = edge_clip;
    edge_clip_s.x = dst_center->x + center_e_sx - edge_clip.w / 2;
    edge_clip_s.y = dst_center->y + center_e_sy - edge_clip.h / 2;
    edge_clip_e.x = dst_center->x + center_e_ex - edge_clip.w / 2;
    edge_clip_e.y = dst_center->y + center_e_ey - edge_clip.h / 2;
    scui_area_t dst_edge_clip_s = {0};
    scui_area_t dst_edge_clip_e = {0};
    if (scui_area_inter(&dst_edge_clip_s, dst_clip, &edge_clip_s))
        scui_draw_image(dst_surface, &dst_edge_clip_s, src_image_e, &edge_clip, src_alpha, src_color);
    if (scui_area_inter(&dst_edge_clip_e, dst_clip, &edge_clip_e))
        scui_draw_image(dst_surface, &dst_edge_clip_e, src_image_e, &edge_clip, src_alpha, src_color);
}

/*@brief 区域图像绘制(内部接口)
 *       scui_draw_ring是唯一调用者
 *@param 参数列表与调用者唯一一致
 */
static void scui_draw_ring_quadrant(scui_surface_t *dst_surface, scui_area_t  *dst_clip,
                                    scui_point_t   *dst_center,  scui_image_t *src_image_e,
                                    scui_image_t   *src_image,   scui_area_t  *src_clip,
                                    scui_coord_t    src_angle_s, scui_alpha_t  src_alpha,
                                    scui_coord_t    src_angle_e, scui_color_t  src_color)
{
    /* 检查四个象限,如果有完整部分,先行绘制该区块 */
    /* 绕过的重合点不计算在内,应被过滤掉(因为是一个整环) */
    bool quadrant_flag[4] = {
        ((src_angle_s <=   +0 && src_angle_e >=  +90) || (src_angle_s <= -360 && src_angle_e >= -270)),
        ((src_angle_s <=  +90 && src_angle_e >= +180) || (src_angle_s <= -270 && src_angle_e >= -180)),
        ((src_angle_s <= +180 && src_angle_e >= +270) || (src_angle_s <= -180 && src_angle_e >=  -90)),
        ((src_angle_s <= +270 && src_angle_e >= +360) || (src_angle_s <=  -90 && src_angle_e >=   -0)),
    };
    scui_point_t quadrant_offset[4] = {
        {.x = +src_image->pixel.width / 2,.y = +src_image->pixel.height / 2,},
        {.x = -0,                         .y = +src_image->pixel.height / 2,},
        {.x = -0,                         .y = -0,},
        {.x = +src_image->pixel.width / 2,.y = -0,},
    };
    
    for (uint8_t idx = 0; idx < 4; idx++) {
        if (!quadrant_flag[idx])
             continue;
        scui_point_t dst_clip_offset = {
            .x = quadrant_offset[idx].x,
            .y = quadrant_offset[idx].y,
        };
        scui_area_t quadrant_clip = {
            .x = quadrant_offset[idx].x,
            .y = quadrant_offset[idx].y,
            .w = src_image->pixel.width  / 2,
            .h = src_image->pixel.height / 2,
        };
        
        scui_area_t src_ring_clip = *src_clip;
        scui_area_t dst_ring_area = {
            .x = dst_center->x - src_image->pixel.width  / 2 + quadrant_offset[idx].x,
            .y = dst_center->y - src_image->pixel.height / 2 + quadrant_offset[idx].y,
            .w = src_image->pixel.width  / 2,
            .h = src_image->pixel.height / 2,
        };
        scui_area_t dst_ring_clip = *dst_clip;
        if (scui_area_inter(&src_ring_clip, src_clip, &quadrant_clip))
        if (scui_area_inter(&dst_ring_clip, dst_clip, &dst_ring_area))
            scui_draw_image(dst_surface, &dst_ring_clip, src_image, &src_ring_clip, src_alpha, src_color);
    }
}

/*@brief 区域图像绘制(内部接口)
 *       scui_draw_ring_quadrant_1是唯一调用者
 */
static void scui_draw_ring_quadrant_1_angle_tan(scui_multi_t *angle_tan0_4096,
                                                scui_coord_t  src_angle_s,
                                                scui_multi_t *angle_tan1_4096,
                                                scui_coord_t  src_angle_e)
{
    uint8_t idx = -1;
    /* 取那个一定不落在边界的角度 */
    if (src_angle_s % 90 != 0)
        idx = src_angle_s / 90;
    if (src_angle_e % 90 != 0)
        idx = src_angle_e / 90;
    SCUI_ASSERT(idx >= 0 && idx <= 3);
    
    /* 无所谓角度顺序与方向,扫描线始终按行从左至右 */
    /* 如果有一个值为0,则扫描线一端落在边界点处,但不可以俩个都为0 */
    *angle_tan0_4096 = -1;
    *angle_tan1_4096 = -1;
    /* 注意:这里取得反目标, 因为tan(angle) = h / w === tan(90 - angle) = w / h */
    
    if (src_angle_s % 90 != 0 && src_angle_e % 90 != 0) {
        
        if (idx == 0 || idx == 2) {
            *angle_tan0_4096 = scui_tan4096(90 - (src_angle_s % 90));
            *angle_tan1_4096 = scui_tan4096(90 - (src_angle_e % 90));
        } else {
            *angle_tan0_4096 = scui_tan4096(src_angle_s % 90);
            *angle_tan1_4096 = scui_tan4096(src_angle_e % 90);
        }
    } else if (src_angle_s % 90 != 0) {
        
        if (idx == 0 || idx == 2)
            *angle_tan0_4096 = scui_tan4096(90 - (src_angle_s % 90));
        else
            *angle_tan0_4096 = scui_tan4096(src_angle_s % 90);
        
    } else if (src_angle_e % 90 != 0) {
        
        if (idx == 0 || idx == 2)
            *angle_tan1_4096 = scui_tan4096(90 - (src_angle_e % 90));
        else
            *angle_tan1_4096 = scui_tan4096(src_angle_e % 90);
        
    }
    
    SCUI_ASSERT(*angle_tan0_4096 >= 0 || *angle_tan0_4096 == -1);
    SCUI_ASSERT(*angle_tan1_4096 >= 0 || *angle_tan1_4096 == -1);
}

/*@brief 区域图像绘制(内部接口)
 *       scui_draw_ring_quadrant_1是唯一调用者
*/
static scui_point_t scui_draw_ring_quadrant_1_draw_area(scui_area_t *draw_area,
                                                        scui_multi_t idx_line, uint8_t idx,
                                                        scui_multi_t angle_tan0_4096,
                                                        scui_multi_t angle_tan1_4096)
{
    scui_multi_t draw_area_x0 = 0;
    scui_multi_t draw_area_x1 = 0;
    
    if (angle_tan0_4096 != -1 && angle_tan1_4096 != -1) {
        
        if (idx == 0 || idx == 1) {
            draw_area_x0 = angle_tan0_4096 * (idx_line) >> 12;
            draw_area_x1 = angle_tan1_4096 * (idx_line) >> 12;
        } else {
            draw_area_x0 = angle_tan0_4096 * (draw_area->h - idx_line) >> 12;
            draw_area_x1 = angle_tan1_4096 * (draw_area->h - idx_line) >> 12;
        }
        if (idx == 1 || idx == 2) {
            draw_area_x0 = draw_area_x0 > draw_area->w ? 0 : draw_area->w - draw_area_x0;
            draw_area_x1 = draw_area_x1 > draw_area->w ? 0 : draw_area->w - draw_area_x1;
        }
    } else if (angle_tan0_4096 != -1) {
        
        if (idx == 0 || idx == 1)
            draw_area_x0 = angle_tan0_4096 * (idx_line) >> 12;
        else
            draw_area_x0 = angle_tan0_4096 * (draw_area->h - idx_line) >> 12;
        
        if (idx == 1 || idx == 2)
            draw_area_x0 = draw_area_x0 > draw_area->w ? 0 : draw_area->w - draw_area_x0;
        
        if (idx == 0 || idx == 1)
            draw_area_x1 = 0;
        else
            draw_area_x1 = draw_area->w;
        
    } else if (angle_tan1_4096 != -1) {
        
        if (idx == 0 || idx == 1)
            draw_area_x1 = angle_tan1_4096 * (idx_line) >> 12;
        else
            draw_area_x1 = angle_tan1_4096 * (draw_area->h - idx_line) >> 12;
        
        if (idx == 1 || idx == 2)
            draw_area_x1 = draw_area_x1 > draw_area->w ? 0 : draw_area->w - draw_area_x1;
        
        if (idx == 0 || idx == 1)
            draw_area_x0 = draw_area->w;
        else
            draw_area_x0 = 0;
        
    }

    scui_multi_t draw_area_xl = scui_min(draw_area_x0, draw_area_x1);
    scui_multi_t draw_area_xr = scui_max(draw_area_x0, draw_area_x1);
    draw_area_xl = scui_max(draw_area_xl, 0);
    draw_area_xr = scui_min(draw_area_xr, draw_area->w);
    
    return (scui_point_t) {
        .x = draw_area_xl,
        .y = draw_area_xr,
    };
}


/*@brief 区域图像绘制(内部接口)
 *       scui_draw_ring是唯一调用者
 *@param 参数列表与调用者唯一一致
 */
static void scui_draw_ring_quadrant_1(scui_surface_t *dst_surface, scui_area_t  *dst_clip,
                                      scui_point_t   *dst_center,  scui_image_t *src_image_e,
                                      scui_image_t   *src_image,   scui_area_t  *src_clip,
                                      scui_coord_t    src_angle_s, scui_alpha_t  src_alpha,
                                      scui_coord_t    src_angle_e, scui_color_t  src_color)
{
    #if 1
    /* 这里需要断言 */
    SCUI_ASSERT(src_angle_e % 90 != 0 || src_angle_s % 90 != 0);
    
    uint8_t idx = -1;
    /* 取那个一定不落在边界的角度 */
    if (src_angle_s % 90 != 0)
        idx = src_angle_s / 90;
    if (src_angle_e % 90 != 0)
        idx = src_angle_e / 90;
    SCUI_ASSERT(idx >= 0 && idx <= 3);
    
    scui_point_t quadrant_offset[4] = {
        {.x = +src_image->pixel.width / 2,.y = +src_image->pixel.height / 2,},
        {.x = -0,                         .y = +src_image->pixel.height / 2,},
        {.x = -0,                         .y = -0,},
        {.x = +src_image->pixel.width / 2,.y = -0,},
    };
    
    scui_area_t quadrant_clip = {
        .x = quadrant_offset[idx].x,
        .y = quadrant_offset[idx].y,
        .w = src_image->pixel.width  / 2,
        .h = src_image->pixel.height / 2,
    };
    #endif
    
    scui_image_unit_t image = {.image = src_image,};
    scui_image_cache_load(&image);
    SCUI_ASSERT(image.data != NULL);
    
    scui_surface_t image_surface = {
        .pixel   = image.data,
        .hor_res = src_image->pixel.width,
        .ver_res = src_image->pixel.height,
        .alpha   = src_alpha,
    };
    
    scui_image_cf_to_pixel_cf(&image.image->format, &image_surface.format);
    scui_surface_t *src_surface = &image_surface;
    
    scui_point_t dst_offset = {
        .x = dst_center->x - src_image->pixel.width  / 2,
        .y = dst_center->y - src_image->pixel.height / 2,
    };
    
    /* 按俩个画布的透明度进行像素点混合 */
    scui_area_t dst_area_v = {0};   // v:vaild
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t dst_area = scui_surface_area(dst_surface);
    /* 在此处将quadrant_offset融合进去(dst同步src的偏移) */
    scui_area_t dst_area_s = quadrant_clip;
    dst_area_s.x += dst_offset.x;
    dst_area_s.y += dst_offset.y;
    if (!scui_area_inter(&dst_area_v, &dst_area, &dst_area_s))
         goto over;
    if (!scui_area_inter(&dst_clip_v,  dst_clip, &dst_area_v))
         goto over;
    
    scui_area_t src_clip_v = {0};   // v:vaild
    /* 在此处将quadrant_clip融合进去(它一定是src_surface的子剪切域) */
    scui_area_t src_area = quadrant_clip;
    if (!scui_area_inter(&src_clip_v, &src_area, src_clip))
         goto over;
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    SCUI_ASSERT(dst_clip->x + draw_area.w <= dst_surface->hor_res);
    SCUI_ASSERT(dst_clip->y + draw_area.h <= dst_surface->ver_res);
    
    if (scui_area_empty(&draw_area))
        goto over;
    
    #if 1
    scui_multi_t angle_tan0_4096 = -1;
    scui_multi_t angle_tan1_4096 = -1;
    scui_draw_ring_quadrant_1_angle_tan(&angle_tan0_4096, src_angle_s,
                                        &angle_tan1_4096, src_angle_e);
    #endif
    
    scui_alpha_t alpha = 0;
    scui_coord_t dst_bits = scui_pixel_bits(dst_surface->format);
    scui_coord_t src_bits = scui_pixel_bits(src_surface->format);
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    scui_coord_t src_byte = scui_pixel_bits(src_surface->format) / 8;
    scui_multi_t dst_pixel_ofs = dst_offset.y * dst_surface->hor_res + dst_offset.x;
    scui_multi_t src_pixel_ofs = 0;
    uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * dst_byte;
    uint8_t *src_addr = src_surface->pixel + src_pixel_ofs * src_bits / 8;
    
    
    if (image.image->format == scui_image_cf_palette4) {
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
                goto over;
                
            #if 1
            for (scui_multi_t idx_line = 0; idx_line < src_area.h; idx_line++) {
                /* 扫描区不在src_clip_v中,跳过它 */
                if (src_area.y + idx_line < src_clip_v.y ||
                    src_area.y + idx_line > src_clip_v.y + draw_area.h)
                    continue;
                /* 原扫描行[0, draw_area.w],现在重新更新到新的限制扫描行 */
                scui_point_t draw_area_x  = scui_draw_ring_quadrant_1_draw_area(&src_area,
                                            idx_line, idx, angle_tan0_4096, angle_tan1_4096);
                /* 扫描区不在src_clip_v中,跳过它 */
                scui_multi_t draw_area_xl = scui_max(draw_area_x.x, src_clip_v.x - src_area.x);
                scui_multi_t draw_area_xr = scui_min(draw_area_x.y, src_clip_v.x - src_area.x+ draw_area.w);
            /* 更新原扫描行到新的限制扫描行即可 */
            for (scui_multi_t idx_item = draw_area_xl; idx_item < draw_area_xr; idx_item++) {
            #endif
                
                uint8_t *dst_ofs = dst_addr + ((src_area.y + idx_line) * dst_surface->hor_res + (src_area.x + idx_item)) * dst_byte;
                uint8_t *src_ofs = src_addr + ((src_area.y + idx_line) * src_surface->hor_res + (src_area.x + idx_item)) / 2;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = (*src_addr_ofs >> (idx_item % 2 == 0 ? 0 : 4)) & 0xF;
                alpha = (uint32_t)src_surface->alpha * palette * 0x11 / scui_alpha_cover;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                    SCUI_PIXEL_CF_DEF, &palette_table[0], alpha);
            }
            }
        } else {
                
            #if 1
            for (scui_multi_t idx_line = 0; idx_line < src_area.h; idx_line++) {
                /* 扫描区不在src_clip_v中,跳过它 */
                if (src_area.y + idx_line < src_clip_v.y ||
                    src_area.y + idx_line > src_clip_v.y + draw_area.h)
                    continue;
                /* 原扫描行[0, draw_area.w],现在重新更新到新的限制扫描行 */
                scui_point_t draw_area_x  = scui_draw_ring_quadrant_1_draw_area(&src_area,
                                            idx_line, idx, angle_tan0_4096, angle_tan1_4096);
                /* 扫描区不在src_clip_v中,跳过它 */
                scui_multi_t draw_area_xl = scui_max(draw_area_x.x, src_clip_v.x - src_area.x);
                scui_multi_t draw_area_xr = scui_min(draw_area_x.y, src_clip_v.x - src_area.x+ draw_area.w);
                /* 更新原扫描行到新的限制扫描行即可 */
            for (scui_multi_t idx_item = draw_area_xl; idx_item < draw_area_xr; idx_item++) {
            #endif
                
                uint8_t *dst_ofs = dst_addr + ((src_area.y + idx_line) * dst_surface->hor_res + (src_area.x + idx_item)) * dst_byte;
                uint8_t *src_ofs = src_addr + ((src_area.y + idx_line) * src_surface->hor_res + (src_area.x + idx_item)) / 2;
                
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
        }
        goto over;
    }
    if (image.image->format == scui_image_cf_palette8) {
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
                goto over;
                
            #if 1
            for (scui_multi_t idx_line = 0; idx_line < src_area.h; idx_line++) {
                /* 扫描区不在src_clip_v中,跳过它 */
                if (src_area.y + idx_line < src_clip_v.y ||
                    src_area.y + idx_line > src_clip_v.y + draw_area.h)
                    continue;
                /* 原扫描行[0, draw_area.w],现在重新更新到新的限制扫描行 */
                scui_point_t draw_area_x  = scui_draw_ring_quadrant_1_draw_area(&src_area,
                                            idx_line, idx, angle_tan0_4096, angle_tan1_4096);
                /* 扫描区不在src_clip_v中,跳过它 */
                scui_multi_t draw_area_xl = scui_max(draw_area_x.x, src_clip_v.x - src_area.x);
                scui_multi_t draw_area_xr = scui_min(draw_area_x.y, src_clip_v.x - src_area.x+ draw_area.w);
                /* 更新原扫描行到新的限制扫描行即可 */
            for (scui_multi_t idx_item = draw_area_xl; idx_item < draw_area_xr; idx_item++) {
            #endif
                
                uint8_t *dst_ofs = dst_addr + ((src_area.y + idx_line) * dst_surface->hor_res + (src_area.x + idx_item)) * dst_byte;
                uint8_t *src_ofs = src_addr + ((src_area.y + idx_line) * src_surface->hor_res + (src_area.x + idx_item)) * 1;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = *src_addr_ofs;
                alpha = (uint32_t)src_surface->alpha * palette / scui_alpha_cover;
                scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                    SCUI_PIXEL_CF_DEF, &palette_table[0], alpha);
            }
            }
        } else {
            
            #if 1
            for (scui_multi_t idx_line = 0; idx_line < src_area.h; idx_line++) {
                /* 扫描区不在src_clip_v中,跳过它 */
                if (src_area.y + idx_line < src_clip_v.y ||
                    src_area.y + idx_line > src_clip_v.y + draw_area.h)
                    continue;
                /* 原扫描行[0, draw_area.w],现在重新更新到新的限制扫描行 */
                scui_point_t draw_area_x  = scui_draw_ring_quadrant_1_draw_area(&src_area,
                                            idx_line, idx, angle_tan0_4096, angle_tan1_4096);
                /* 扫描区不在src_clip_v中,跳过它 */
                scui_multi_t draw_area_xl = scui_max(draw_area_x.x, src_clip_v.x - src_area.x);
                scui_multi_t draw_area_xr = scui_min(draw_area_x.y, src_clip_v.x - src_area.x+ draw_area.w);
                /* 更新原扫描行到新的限制扫描行即可 */
            for (scui_multi_t idx_item = draw_area_xl; idx_item < draw_area_xr; idx_item++) {
            #endif
                
                uint8_t *dst_ofs = dst_addr + ((src_area.y + idx_line) * dst_surface->hor_res + (src_area.x + idx_item)) * dst_byte;
                uint8_t *src_ofs = src_addr + ((src_area.y + idx_line) * src_surface->hor_res + (src_area.x + idx_item)) * 1;
                
                uint8_t *src_addr_ofs = (void *)src_ofs;
                uint8_t palette = *src_addr_ofs;
                if (palette != 0 && palette != 0xFF)
                if (palette_table[palette] == 0x00) {
                    
                    uint32_t *pixel_1 = &palette_table[0];
                    uint32_t *pixel_2 = &palette_table[palette_len - 1];
                    scui_alpha_t alpha_1 = palette;
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
        }
        goto over;
    }
    
    over:
    scui_image_cache_unload(&image);
}

/*@brief 区域图像绘制
 *@param dst_surface 画布目标实例
 *@param dst_clip    画布绘制区域
 *@param dst_center  图像旋转点
 *@param src_image_e 图像源
 *@param src_image   图像源
 *@param src_clip    图像源绘制区域
 *@param src_angle_s 起始角度
 *@param src_alpha   图像透明度(非图像自带透明度)
 *@param src_angle_e 结束角度
 *@param src_color   图像源色调(调色板使用)
 */
void scui_draw_ring(scui_surface_t *dst_surface, scui_area_t  *dst_clip,
                    scui_point_t   *dst_center,  scui_image_t *src_image_e,
                    scui_image_t   *src_image,   scui_area_t  *src_clip,
                    scui_coord_t    src_angle_s, scui_alpha_t  src_alpha,
                    scui_coord_t    src_angle_e, scui_color_t  src_color)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(dst_center != NULL && src_image_e != NULL);
    SCUI_ASSERT(src_image != NULL && src_clip != NULL);
    
    /* 限制要求,只支持调色板位图 */
    SCUI_ASSERT(src_image->format == scui_image_cf_palette4 ||
                src_image->format == scui_image_cf_palette8);
    SCUI_ASSERT(src_image_e->format == scui_image_cf_palette4 ||
                src_image_e->format == scui_image_cf_palette8);
    
    /* 限制要求,调色板位图为正方形图像 */
    SCUI_ASSERT(src_image->pixel.width == src_image->pixel.height);
    SCUI_ASSERT(src_image_e->pixel.width == src_image_e->pixel.height);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    /* 整环绘制: */
    if ((src_angle_e - src_angle_s) % 360 == 0) {
        scui_draw_image(dst_surface, dst_clip, src_image, src_clip, src_alpha, src_color);
        return;
    }
    
    /* 角度交换(保证s < e) */
    /* 这将限制角度以顺时针旋转绘制为目标 */
    if (src_angle_s > src_angle_e) {
        /*  */
        src_angle_s = src_angle_s ^ src_angle_e;
        src_angle_e = src_angle_s ^ src_angle_e;
        src_angle_s = src_angle_s ^ src_angle_e;
    }
    /* 角度限制(-360, +360): */
    while (src_angle_e > 360) {
        src_angle_s -= 360;
        src_angle_e -= 360;
    }
    
    while (src_angle_e <= 0) {
        src_angle_s += 360;
        src_angle_e += 360;
    }
    
    /* 1.绘制俩个端点 */
    scui_draw_ring_edge(dst_surface, dst_clip,
                        dst_center,  src_image_e,
                        src_image,   src_clip,
                        src_angle_s, src_alpha,
                        src_angle_e, src_color);
    
    /* 2.绘制完整象限(绕过的重合点不计算,在上面被过滤掉) */
    scui_draw_ring_quadrant(dst_surface, dst_clip,
                            dst_center,  src_image_e,
                            src_image,   src_clip,
                            src_angle_s, src_alpha,
                            src_angle_e, src_color);
    
    /* 3.1绘制在同一象限 */
    if (src_angle_s / 90 == src_angle_e / 90 && src_angle_s % 90 != 0 && src_angle_s % 90 != 0) {
        /* 如果s和e在一个象限内,按照上面e>0的限制,s一定大于0 */
        if (src_angle_s > 0 && src_angle_e > 0) {
            scui_draw_ring_quadrant_1(dst_surface, dst_clip,
                                      dst_center,  src_image_e,
                                      src_image,   src_clip,
                                      src_angle_s, src_alpha,
                                      src_angle_e, src_color);
            return;
        }
    }
    
    if (src_angle_s % 90 != 0 && src_angle_e % 90 != 0) {
        
        scui_coord_t rcd_angle_s = src_angle_s;
        scui_coord_t rcd_angle_e = src_angle_e;
        /* s有可能小于0,此时求个模 */
        while (rcd_angle_s < 0)
               rcd_angle_s += 360;
        /* 3.2绘制在最多俩个不完整象限 */
        src_angle_s = (rcd_angle_s);
        src_angle_e = (rcd_angle_s / 90 + 1) * 90;
        if (src_angle_s != src_angle_e)
            scui_draw_ring_quadrant_1(dst_surface, dst_clip,
                                      dst_center,  src_image_e,
                                      src_image,   src_clip,
                                      src_angle_s, src_alpha,
                                      src_angle_e, src_color);
        src_angle_s = (rcd_angle_e / 90) * 90;
        src_angle_e = (rcd_angle_e);
        if (src_angle_s != src_angle_e)
            scui_draw_ring_quadrant_1(dst_surface, dst_clip,
                                      dst_center,  src_image_e,
                                      src_image,   src_clip,
                                      src_angle_s, src_alpha,
                                      src_angle_e, src_color);
        return;
    }
    
    if (src_angle_s % 90 != 0 || src_angle_e % 90 != 0) {
        scui_draw_ring_quadrant_1(dst_surface, dst_clip,
                                  dst_center,  src_image_e,
                                  src_image,   src_clip,
                                  src_angle_s, src_alpha,
                                  src_angle_e, src_color);
        return;
    }
}
