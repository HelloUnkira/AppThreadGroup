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
 *@param src_clip    字符源绘制区域
 *@param src_alpha   字符透明度
 *@param src_color   字符色调
 */
void scui_draw_letter(scui_surface_t    *dst_surface, scui_area_t *dst_clip,
                      scui_font_glyph_t *src_glyph,   scui_area_t *src_clip,
                      scui_alpha_t       src_alpha,   scui_color_t src_color)
{
    SCUI_ASSERT(dst_surface != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_glyph != NULL && src_clip != NULL);
    
    scui_surface_t glyph_surface = {
        .pixel   = src_glyph->bitmap,
        .format  = SCUI_PIXEL_CF_DEF,
        .hor_res = src_glyph->box_w,
        .ver_res = src_glyph->box_h,
    };
    
    scui_surface_t *src_surface = &glyph_surface;
    
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
    
    scui_alpha_t alpha = 0;
    scui_multi_t dst_pixel_ofs = dst_clip_v.y * dst_surface->hor_res + dst_clip_v.x;
    scui_multi_t src_pixel_ofs = src_clip_v.y * src_surface->hor_res + src_clip_v.x;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * dst_byte;
    uint8_t *src_addr = src_surface->pixel;
    /* 调色板数组(为空时计算,有时直接取): */
    scui_multi_t palette_len = 1 << src_glyph->bpp;
    scui_color_wt_t *palette_table = SCUI_MEM_ALLOC(scui_mem_type_mix, palette_len * sizeof(scui_color_wt_t));
    scui_color_wt_t  filter = 0;
    memset(palette_table, 0, palette_len * sizeof(scui_color_wt_t));
    /* 起始色调和结束色调固定 */
    scui_pixel_by_color(src_surface->format, &palette_table[0], src_color.color_e);
    scui_pixel_by_color(src_surface->format, &palette_table[palette_len - 1], src_color.color_s);
    scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
    
    /* 无渐变时: */
    if (palette_table[0] == palette_table[palette_len - 1]) {
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
            uint32_t idx_ofs = src_pixel_ofs + idx_line * src_surface->hor_res + idx_item;
            uint8_t *src_ofs = src_addr + idx_ofs / (8 / src_glyph->bpp);
            uint8_t  palette = scui_letter_bpp_palette(*(uint8_t *)src_ofs, src_glyph->bpp, idx_ofs % (8 / src_glyph->bpp));
            
            uint32_t palette_color = palette_table[0];
            scui_pixel_mix_alpha(dst_surface->format, &palette_color, palette);
            /* 过滤色调,去色 */
            if (src_color.filter && palette_color == filter)
                continue;
            
            alpha = (uint32_t)src_alpha * palette / scui_alpha_cover;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                src_surface->format, &palette_table[0], alpha);
        }
    } else {
        for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
        for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
            uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
            uint32_t idx_ofs = src_pixel_ofs + idx_line * src_surface->hor_res + idx_item;
            uint8_t *src_ofs = src_addr + idx_ofs / (8 / src_glyph->bpp);
            uint8_t  palette = scui_letter_bpp_palette(*(uint8_t *)src_ofs, src_glyph->bpp, idx_ofs % (8 / src_glyph->bpp));
            uint8_t  palette_idx = (uint16_t)palette * (palette_len - 1) / 0xFF;
            
            if (palette_idx != 0 && palette_idx != palette_len - 1)
            if (palette_table[palette_idx] == 0x00) {
                uint32_t *pixel_1 = &palette_table[0];
                uint32_t *pixel_2 = &palette_table[palette_len - 1];
                scui_alpha_t alpha_1 = palette;
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                
                palette_table[palette_idx] = palette_table[0];
                scui_pixel_mix_with(src_surface->format, &palette_table[palette_idx], alpha_1,
                                    src_surface->format, &palette_table[palette_len - 1], alpha_2);
            }
            
            uint32_t palette_color = palette_table[palette_idx];
            scui_pixel_mix_alpha(dst_surface->format, &palette_color, palette);
            /* 过滤色调,去色 */
            if (src_color.filter && palette_color == filter)
                continue;
            
            alpha = (uint32_t)src_alpha * palette / scui_alpha_cover;
            scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                                src_surface->format, &palette_table[palette_idx], alpha);
        }
    }
    
    SCUI_MEM_FREE(palette_table);
}
