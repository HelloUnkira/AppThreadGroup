/*实现目标:
 *    图像绘制
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 区域图像绘制(可以使用DMA2D-blend加速优化)
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param color        图像源色调(调色板使用)
 *@param alpha        图像透明度(非图像自带透明度)
 */
void scui_draw_image(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                     scui_image_unit_t *image_unit,  scui_area_t *src_clip,
                     scui_color_gradient_t color,    scui_alpha_t alpha)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(image_unit != NULL && image_unit->image != NULL && image_unit->data != NULL);
    
    scui_area_t image_clip = {
        .x = 0, .w = image_unit->image->pixel.width,
        .y = 0, .h = image_unit->image->pixel.height,
    };
    
    scui_surface_t image_surface = {
        .pixel = image_unit->data,
        .clip  = image_clip,
        .alpha = alpha,
    };
    
    SCUI_PIXEL_TYPE pixel = {0};
    scui_surface_t *src_surface = &image_surface;
    
    scui_area_t *dst_area = &dst_surface->clip;
    scui_area_t *src_area = &image_clip;
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_inter(&dst_clip_v, dst_area, dst_clip);
    scui_area_inter(&src_clip_v, src_area, src_clip);
    
    scui_area_t draw_area = {
        .w = scui_min(dst_clip_v.w, src_clip_v.w),
        .h = scui_min(dst_clip_v.h, src_clip_v.h),
    };
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_coord_t dst_pixel_ofs = dst_clip_v.y * dst_surface->clip.w + dst_clip_v.x;
    scui_coord_t src_pixel_ofs = src_clip_v.y * src_surface->clip.w + src_clip_v.x;
    
    /* image blend: */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_argb8565) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_argb8888)) {
        if (image_unit->image->format == scui_image_format_argb8565) {
            uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * SCUI_PIXEL_SIZE;
            uint8_t *src_addr = src_surface->pixel + src_pixel_ofs * 3;
            for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->clip.w + idx_item) * SCUI_PIXEL_SIZE;
                uint8_t *src_ofs = src_addr + (idx_line * src_surface->clip.w + idx_item) * 3;
                SCUI_PIXEL_TYPE  *dst_addr_ofs = (void *)dst_ofs;
                scui_color8565_t *src_addr_ofs = (void *)src_ofs;
                alpha = (scui_alpha_t)src_addr_ofs->ch.a * (uint16_t)src_surface->alpha / scui_alpha_cover;
                pixel = *(SCUI_PIXEL_TYPE *)&src_addr_ofs->ch;
                pixel = scui_pixel_mix_with_alpha(&pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                // pixel = scui_pixel_blend_with_alpha(&pixel, alpha, dst_addr_ofs, dst_surface->alpha);
                scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                // 断言检查,正常不使用
                SCUI_ASSERT(dst_ofs >= dst_addr && dst_ofs - dst_addr <= dst_surface->clip.w * dst_surface->clip.h * SCUI_PIXEL_SIZE);
                SCUI_ASSERT(src_ofs >= src_addr && src_ofs - src_addr <= src_surface->clip.w * src_surface->clip.h * 3);
            }
            return;
        }
        if (image_unit->image->format == scui_image_format_argb8888) {
            uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * SCUI_PIXEL_SIZE;
            uint8_t *src_addr = src_surface->pixel + src_pixel_ofs * 4;
            for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
            for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->clip.w + idx_item) * SCUI_PIXEL_SIZE;
                uint8_t *src_ofs = src_addr + (idx_line * src_surface->clip.w + idx_item) * 4;
                SCUI_PIXEL_TYPE  *dst_addr_ofs = (void *)dst_ofs;
                scui_color8888_t *src_addr_ofs = (void *)src_ofs;
                alpha = (scui_alpha_t)src_addr_ofs->ch.a * (uint16_t)src_surface->alpha / scui_alpha_cover;
                pixel = *(SCUI_PIXEL_TYPE *)&src_addr_ofs->ch;
                pixel = scui_pixel_mix_with_alpha(&pixel, alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                // pixel = scui_pixel_blend_with_alpha(&pixel, alpha, dst_addr_ofs, dst_surface->alpha);
                scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                // 断言检查,正常不使用
                SCUI_ASSERT(dst_ofs >= dst_addr && dst_ofs - dst_addr <= dst_surface->clip.w * dst_surface->clip.h * SCUI_PIXEL_SIZE);
                SCUI_ASSERT(src_ofs >= src_addr && src_ofs - src_addr <= src_surface->clip.w * src_surface->clip.h * 4);
            }
            return;
        }
    }
    
    /* image cover:(调色板) */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_p4) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_p8)) {
         if (image_unit->image->format == scui_image_format_p4) {
             uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * SCUI_PIXEL_SIZE;
             uint8_t *src_addr = src_surface->pixel + src_pixel_ofs / 2;
             /* 调色板数组(为空时计算,有时直接取): */
             scui_coord_t palette_len = 1 << (image_unit->image->format >> 8);
             SCUI_PIXEL_TYPE palette_table[1 << (scui_image_format_p4 >> 8)] = {0};
             /* 起始色调和结束色调固定 */
             palette_table[0] = scui_pixel_by_color(color.color_s);
             palette_table[palette_len - 1] = scui_pixel_by_color(color.color_e);
             /* 无渐变时: */
             if (palette_table[0].full == palette_table[palette_len - 1].full) {
                 for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
                 for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                    uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->clip.w + idx_item) * SCUI_PIXEL_SIZE;
                    uint8_t *src_ofs = src_addr + (idx_line * src_surface->clip.w + idx_item) / 2;
                    SCUI_PIXEL_TYPE  *dst_addr_ofs = (void *)dst_ofs;
                    uint8_t *src_addr_ofs = (void *)src_ofs;
                    uint8_t palette = (*src_addr_ofs >> (idx_item % 2 == 0 ? 0 : 4)) & 0xF;
                    alpha = (uint32_t)src_surface->alpha * palette * 0x11 / scui_alpha_cover;
                    pixel = scui_pixel_mix_with_alpha(&palette_table[0], alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                    // pixel = scui_pixel_blend_with_alpha(&palette_table[0], alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                 }
             } else {
                 for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
                 for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                    uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->clip.w + idx_item) * SCUI_PIXEL_SIZE;
                    uint8_t *src_ofs = src_addr + (idx_line * src_surface->clip.w + idx_item) / 2;
                    SCUI_PIXEL_TYPE  *dst_addr_ofs = (void *)dst_ofs;
                    uint8_t *src_addr_ofs = (void *)src_ofs;
                    uint8_t palette = (*src_addr_ofs >> (idx_item % 2 == 0 ? 0 : 4)) & 0xF;
                    if (palette != 0 && palette != 0xF)
                    if (palette_table[palette].full == 0x00) {
                        SCUI_PIXEL_TYPE *pixel_1 = &palette_table[0];
                        SCUI_PIXEL_TYPE *pixel_2 = &palette_table[palette_len - 1];
                        scui_alpha_t alpha_1 = palette * 0x11;
                        scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                        palette_table[palette] = scui_pixel_mix_with_alpha(pixel_1, alpha_1, pixel_2, alpha_2);
                    }
                    pixel = scui_pixel_mix_with_alpha(&palette_table[palette], src_surface->alpha, dst_addr_ofs, scui_alpha_cover - src_surface->alpha);
                    // pixel = scui_pixel_blend_with_alpha(&palette_table[palette], src_surface->alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                 }
             }
         }
         if (image_unit->image->format == scui_image_format_p8) {
             uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * SCUI_PIXEL_SIZE;
             uint8_t *src_addr = src_surface->pixel + src_pixel_ofs * 1;
             /* 调色板数组(为空时计算,有时直接取): */
             scui_coord_t palette_len = 1 << (image_unit->image->format >> 8);
             SCUI_PIXEL_TYPE palette_table[1 << (scui_image_format_p8 >> 8)] = {0};
             /* 起始色调和结束色调固定 */
             palette_table[0] = scui_pixel_by_color(color.color_s);
             palette_table[palette_len - 1] = scui_pixel_by_color(color.color_e);
             /* 无渐变时: */
             if (palette_table[0].full == palette_table[palette_len - 1].full) {
                 for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
                 for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                    uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->clip.w + idx_item) * SCUI_PIXEL_SIZE;
                    uint8_t *src_ofs = src_addr + (idx_line * src_surface->clip.w + idx_item) * 1;
                    SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                    uint8_t *src_addr_ofs = (void *)src_ofs;
                    uint8_t palette = *src_addr_ofs;
                    alpha = (uint32_t)src_surface->alpha * palette / scui_alpha_cover;
                    pixel = scui_pixel_mix_with_alpha(&palette_table[0], alpha, dst_addr_ofs, scui_alpha_cover - alpha);
                    // pixel = scui_pixel_blend_with_alpha(&palette_table[0], alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                 }
             } else {
                 for (scui_coord_t idx_line = 0; idx_line < draw_area.h; idx_line++)
                 for (scui_coord_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
                    uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->clip.w + idx_item) * SCUI_PIXEL_SIZE;
                    uint8_t *src_ofs = src_addr + (idx_line * src_surface->clip.w + idx_item) * 1;
                    SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
                    uint8_t *src_addr_ofs = (void *)src_ofs;
                    uint8_t palette = *src_addr_ofs;
                    if (palette != 0 && palette != 0xFF)
                    if (palette_table[palette].full == 0x00) {
                        SCUI_PIXEL_TYPE *pixel_1 = &palette_table[0];
                        SCUI_PIXEL_TYPE *pixel_2 = &palette_table[palette_len - 1];
                        scui_alpha_t alpha_1 = palette;
                        scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                        palette_table[palette] = scui_pixel_mix_with_alpha(pixel_1, alpha_1, pixel_2, alpha_2);
                    }
                    pixel = scui_pixel_mix_with_alpha(&palette_table[palette], src_surface->alpha, dst_addr_ofs, scui_alpha_cover - src_surface->alpha);
                    // pixel = scui_pixel_blend_with_alpha(&palette_table[palette], src_surface->alpha, dst_addr_ofs, dst_surface->alpha);
                    scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
                 }
             }
         }
    }
    
    /* image blend: */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_rgb565) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_rgb888)) {
        scui_draw_area_blend(dst_surface, dst_clip, &image_surface, src_clip);
        SCUI_LOG_INFO("image pixel blend");
        /* 不应该支持这种条件,有损性能 */
        return;
    }
    
    /* image blend: */
    /* 这个很慢很慢(不应该支持这种条件,有损性能), 除非硬件支持, 建议对齐平台支持 */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_rgb888) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_rgb565))
        SCUI_LOG_WARN("image pixel copy too slow");
    
    /* image blend: */
    /* 这个很慢很慢(不应该支持这种条件,有损性能), 除非硬件支持, 建议对齐平台支持 */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_argb8888) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_argb8565))
        SCUI_LOG_WARN("image pixel copy too slow");
    
    /* image cover:(调色板) */
    /* 这个很慢很慢(不应该支持这种条件,有损性能), 除非硬件支持, 建议对齐平台支持 */
    if ((SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565 && image_unit->image->format == scui_image_format_p8) ||
        (SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888 && image_unit->image->format == scui_image_format_p4))
        SCUI_LOG_WARN("image pixel copy too slow");
    
    SCUI_LOG_ERROR("image can't support");
    SCUI_LOG_ERROR("pixel format:%d", SCUI_PIXEL_FORMAT);
    SCUI_LOG_ERROR("image format:%d", image_unit->image->format);
}

/*@brief 区域图像缩放(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以放大缩小指定比例
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param alpha        图像透明度(非图像自带透明度)
 *@param zoom         图形缩放比例(放大:+,缩小:-)
 */
void scui_draw_image_zoom(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                          scui_image_unit_t *image_unit,  scui_area_t *src_clip,
                          scui_alpha_t       alpha,       scui_coord_t zoom)
{
    /* @等待适配,要用的时候再去实现 */
}

/*@brief 区域图像旋转(可以使用DMA2D加速优化)
 *       以图形源绘制区域中心以指定方向旋转指定角度
 *@param dst_surface  画布目标实例
 *@param dst_clip     画布绘制区域
 *@param image_unit   图像源
 *@param src_clip     图像源绘制区域
 *@param alpha        图像透明度(非图像自带透明度)
 *@param angle        图形旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_draw_image_rotate(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                            scui_image_unit_t *image_unit,  scui_area_t *src_clip,
                            scui_alpha_t       alpha,       scui_coord_t angle)
{
    /* @等待适配,要用的时候再去实现 */
}
