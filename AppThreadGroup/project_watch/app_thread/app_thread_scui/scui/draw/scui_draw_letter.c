/*实现目标:
 *    基础字符绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_letter(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t    *dst_surface = draw_dsc->letter.dst_surface;
    scui_area_t       *dst_clip    = draw_dsc->letter.dst_clip;
    scui_font_glyph_t *src_glyph   = draw_dsc->letter.src_glyph;
    scui_area_t       *src_clip    = draw_dsc->letter.src_clip;
    scui_alpha_t       src_alpha   = draw_dsc->letter.src_alpha;
    scui_color_t       src_color   = draw_dsc->letter.src_color;
    /* draw dsc args<e> */
    //
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
    
    scui_multi_t dst_pixel_ofs = dst_clip_v.y * dst_surface->hor_res + dst_clip_v.x;
    scui_multi_t src_pixel_ofs = src_clip_v.y * src_surface->hor_res + src_clip_v.x;
    
    scui_coord_t dst_byte = scui_pixel_bits(dst_surface->format) / 8;
    uint8_t *dst_addr = dst_surface->pixel + dst_pixel_ofs * dst_byte;
    uint8_t *src_addr = src_surface->pixel;
    /* 调色板数组(为空时计算,有时直接取): */
    scui_multi_t grey_len = 1 << src_glyph->bpp;
    scui_color_wt_t *grey_table = SCUI_MEM_ALLOC(scui_mem_type_mix, grey_len * sizeof(scui_color_wt_t));
    scui_color_wt_t  filter = 0;
    memset(grey_table, 0, grey_len * sizeof(scui_color_wt_t));
    /* 起始色调和结束色调固定 */
    scui_pixel_by_color(src_surface->format, &grey_table[0], src_color.color_e);
    scui_pixel_by_color(src_surface->format, &grey_table[grey_len - 1], src_color.color_s);
    scui_pixel_by_color(src_surface->format, &filter, src_color.color_f);
    bool pixel_no_grad = grey_table[0] == grey_table[grey_len - 1];
    
    for (scui_multi_t idx_line = 0; idx_line < draw_area.h; idx_line++)
    for (scui_multi_t idx_item = 0; idx_item < draw_area.w; idx_item++) {
        uint8_t *dst_ofs = dst_addr + (idx_line * dst_surface->hor_res + idx_item) * dst_byte;
        uint32_t idx_ofs = src_pixel_ofs + idx_line * src_surface->hor_res + idx_item;
        uint8_t *src_ofs = src_addr + idx_ofs / (8 / src_glyph->bpp);
        uint8_t  grey = scui_grey_bpp_x(*src_ofs, src_glyph->bpp, idx_ofs % (8 / src_glyph->bpp));
        uint8_t  grey_idx = pixel_no_grad ? 0 : (uint16_t)grey * (grey_len - 1) / 0xFF;
        
        if (grey_idx != 0 && grey_idx != grey_len - 1)
        if (grey_table[grey_idx] == 0x00) {
            uint32_t *pixel_1 = &grey_table[0];
            uint32_t *pixel_2 = &grey_table[grey_len - 1];
            scui_alpha_t alpha_1 = grey;
            scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
            
            grey_table[grey_idx] = grey_table[0];
            scui_pixel_mix_with(src_surface->format, &grey_table[grey_idx], alpha_1,
                                src_surface->format, &grey_table[grey_len - 1], alpha_2);
        }
        
        scui_color_wt_t grey_color = grey_table[grey_idx];
        scui_pixel_mix_alpha(dst_surface->format, &grey_color, grey);
        /* 过滤色调,去色 */
        if (src_color.filter && grey_color == filter)
            continue;
        
        scui_alpha_t alpha = (uint32_t)src_alpha * grey / scui_alpha_cover;
        scui_pixel_mix_with(dst_surface->format, dst_ofs, scui_alpha_cover - alpha,
                            src_surface->format, &grey_table[grey_idx], alpha);
    }
    
    SCUI_MEM_FREE(grey_table);
}
