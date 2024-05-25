/*实现目标:
 *    基础字符绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_glyph   字符信息
 *@param src_color   字符色调
 *@param src_alpha   字符透明度
 *@param src_filter  字符滤色
 *@param src_trans   字符滤色使能
 */
void scui_draw_letter(scui_surface_t    *dst_surface, scui_area_t     *dst_clip,
                      scui_font_glyph_t *src_glyph,   scui_color_mix_t src_color,
                      scui_alpha_t       src_alpha,   scui_color_mix_t src_filter,
                      bool               src_trans)
{
    SCUI_ASSERT(dst_surface != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_glyph != NULL);
    
    scui_surface_t glyph_surface = {
        .pixel   = src_glyph->bitmap,
        .hor_res = src_glyph->box_w,
        .ver_res = src_glyph->box_h,
    };
    
    scui_surface_t *src_surface = &glyph_surface;
    
    scui_area_t dst_clip_v = {0};   // v:vaild
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t src_clip_v = {0};   // v:vaild
    scui_area_t src_clip = scui_surface_area(src_surface);
    scui_area_t src_area = scui_surface_area(src_surface);
    if (!scui_area_inter(&src_clip_v, &src_area, &src_clip))
         return;
    
    SCUI_PIXEL_TYPE pixel = {0};
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    SCUI_ASSERT(dst_clip->x + draw_area.w <= dst_surface->hor_res);
    SCUI_ASSERT(dst_clip->y + draw_area.h <= dst_surface->ver_res);
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_alpha_t alpha = 0;
    scui_multi_t dst_pixel_ofs = dst_clip_v.y * dst_surface->hor_res + dst_clip_v.x;
    scui_multi_t src_pixel_ofs = src_clip_v.y * src_surface->hor_res + src_clip_v.x;
    
    uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * SCUI_PIXEL_SIZE;
    uint8_t *src_addr = src_surface->pixel;
    /* 调色板数组(为空时计算,有时直接取): */
    scui_multi_t palette_len = 1 << src_glyph->bpp;
    SCUI_PIXEL_TYPE *palette_table = SCUI_MEM_ALLOC(scui_mem_type_mix, palette_len * SCUI_PIXEL_SIZE);
    memset(palette_table, 0, palette_len * SCUI_PIXEL_SIZE);
    /* 起始色调和结束色调固定 */
    palette_table[0] = scui_pixel_by_color(src_color.color_s);
    palette_table[palette_len - 1] = scui_pixel_by_color(src_color.color_e);
    SCUI_PIXEL_TYPE filter = scui_pixel_by_color(src_filter.color);
    scui_multi_t idx_ofs = 0;
    
    /* 无渐变时: */
    if (palette_table[0].full == palette_table[palette_len - 1].full) {
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * SCUI_PIXEL_SIZE;
            uint8_t *src_ofs = src_addr + (idx_line * src_surface->hor_res + idx_item + src_pixel_ofs) / (8 / src_glyph->bpp);
            SCUI_PIXEL_TYPE *dst_addr_ofs = (void *)dst_ofs;
            uint8_t *src_addr_ofs = (void *)src_ofs;
            uint8_t  palette = scui_font_bpp_palette(*src_addr_ofs, src_glyph->bpp, idx_ofs % (8 / src_glyph->bpp));
            idx_ofs++;
            
            /* 过滤色调,去色 */
            if (src_trans && scui_pixel_with_alpha(&palette_table[0], palette).full == filter.full)
                continue;
            
            alpha = (uint32_t)dst_surface->alpha * palette / scui_alpha_cover;
            pixel = scui_pixel_mix_with_alpha(&palette_table[0], alpha, dst_addr_ofs, scui_alpha_cover - alpha);
            // pixel = scui_pixel_blend_with_alpha(&palette_table[0], alpha, dst_addr_ofs, dst_surface->alpha);
            scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
        }
    } else {
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * SCUI_PIXEL_SIZE;
            uint8_t *src_ofs = src_addr + (idx_line * src_surface->hor_res + idx_item + src_pixel_ofs) / (8 / src_glyph->bpp);
            SCUI_PIXEL_TYPE  *dst_addr_ofs = (void *)dst_ofs;
            uint8_t *src_addr_ofs = (void *)src_ofs;
            uint8_t  palette = scui_font_bpp_palette(*src_addr_ofs, src_glyph->bpp, idx_ofs % (8 / src_glyph->bpp));
            idx_ofs++;
            
            if (palette != 0 && palette != palette_len - 1)
            if (palette_table[palette].full == 0x00) {
                SCUI_PIXEL_TYPE *pixel_1 = &palette_table[0];
                SCUI_PIXEL_TYPE *pixel_2 = &palette_table[palette_len - 1];
                scui_alpha_t alpha_1 = palette * (scui_alpha_cover / ((1 << src_glyph->bpp) - 1));
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                palette_table[palette] = scui_pixel_mix_with_alpha(pixel_1, alpha_1, pixel_2, alpha_2);
            }
            
            /* 过滤色调,去色 */
            if (src_trans && scui_pixel_with_alpha(&palette_table[palette], palette).full == filter.full)
                continue;
            
            alpha = (uint32_t)dst_surface->alpha * palette / scui_alpha_cover;
            pixel = scui_pixel_mix_with_alpha(&palette_table[palette], alpha, dst_addr_ofs, scui_alpha_cover - alpha);
            // pixel = scui_pixel_blend_with_alpha(&palette_table[palette], alpha, dst_addr_ofs, dst_surface->alpha);
            scui_mem_w(dst_addr_ofs, pixel, SCUI_PIXEL_TYPE);
        }
    }
    
    SCUI_MEM_FREE(palette_table);
}

