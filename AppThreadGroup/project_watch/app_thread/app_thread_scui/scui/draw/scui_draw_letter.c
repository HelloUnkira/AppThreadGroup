/*实现目标:
 *    字符及字符串绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_letter(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t    *dst_surface = draw_dsc->letter.dst_surface;
    scui_area_t       *dst_clip    = draw_dsc->letter.dst_clip;
    scui_area_t       *src_clip    = draw_dsc->letter.src_clip;
    scui_alpha_t       src_alpha   = draw_dsc->letter.src_alpha;
    scui_color_t       src_color   = draw_dsc->letter.src_color;
    scui_font_glyph_t *src_glyph   = draw_dsc->letter.src_glyph;
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

/*@brief 绘制字符偏移计算
 *@param args   字符串绘制参数
 *@param glyph  字符
 *@param offset 偏移量
 */
static void scui_draw_ctx_string_offset(scui_string_args_t *args, scui_font_glyph_t *glyph, scui_point_t *offset)
{
    if (glyph->bitmap != 0)
        offset->x += glyph->ofs_x + glyph->box_w + args->gap_item;
    else
        offset->x += args->gap_none + args->gap_item;
}

/*@brief 绘制字符串
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_string(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t     *dst_surface = draw_dsc->string.dst_surface;
    scui_area_t        *dst_clip    = draw_dsc->string.dst_clip;
    scui_area_t        *src_clip    = draw_dsc->string.src_clip;
    scui_alpha_t        src_alpha   = draw_dsc->string.src_alpha;
    scui_string_args_t *src_args    = draw_dsc->string.src_args;
    /* draw dsc args<e> */
    //
    /* 从字库中提取一些信息 */
    scui_font_unit_t font_unit = {0};
    font_unit.name = src_args->name;
    font_unit.size = src_args->size;
    scui_font_cache_load(&font_unit);
    scui_coord_t base_line   = scui_font_base_line(font_unit.font);
    scui_coord_t line_height = scui_font_line_height(font_unit.font);
    scui_coord_t underline   = scui_font_underline(font_unit.font);
    scui_font_cache_unload(&font_unit);
    
    scui_area_t dst_clip_v = *dst_clip;   // v:vaild
    scui_area_t src_clip_v = *src_clip;   // v:vaild
    
    if (scui_area_empty(&dst_clip_v))
        return;
    if (scui_area_empty(&src_clip_v))
        return;
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    SCUI_ASSERT(dst_clip->x + draw_area.w <= dst_surface->hor_res);
    SCUI_ASSERT(dst_clip->y + draw_area.h <= dst_surface->ver_res);
    
    if (scui_area_empty(&draw_area))
        return;
    
    scui_point_t offset = {.x = src_clip_v.x,.y = src_clip_v.y,};
    
    if (src_args->line_multi) {
        SCUI_ASSERT(src_args->typo != NULL);
        
        if (src_args->limit <= 0) {
            scui_coord_t line_h = -src_args->limit;
            if (src_args->align_ver == 0);
            if (src_args->align_ver == 1)
                offset.y += line_h;
            if (src_args->align_ver == 2)
                offset.y += line_h / 2;
        }
        
        uint32_t line_ofs = 0;
        // 绘制每一个行
        for (uint32_t idx_line = 0; idx_line < src_args->typo->line_mum; idx_line++) {
             uint32_t line_s = line_ofs;
             uint32_t line_e = src_args->typo->line_ofs[idx_line];
             uint32_t line_w = src_args->typo->line_width[idx_line];
             line_ofs = line_e + 1;
             
             scui_point_t offset_line = offset;
             offset_line.y += (line_height + src_args->gap_line) * idx_line;
             
             if (src_args->align_hor == 0);
             if (src_args->align_hor == 1)
                 offset_line.x += (src_clip_v.w - line_w);
             if (src_args->align_hor == 2)
                 offset_line.x += (src_clip_v.w - line_w) / 2;
             
             SCUI_LOG_INFO("line:<%d, %d> <%d>", line_s, line_e, line_w);
             
            /* 下划线和删除线 */
            scui_point_t line_point_s = offset_line;
            scui_point_t line_point_e = offset_line;
            
             // 行绘制
             for (uint32_t idx = line_s; idx <= line_e; idx++) {
                  SCUI_ASSERT(idx >= 0 && idx < src_args->number);
                
                if (src_args->unicode[idx] < 0x80)
                    SCUI_LOG_DEBUG("letter:%c", src_args->unicode[idx]);
                else
                    SCUI_LOG_DEBUG("letter:%x", src_args->unicode[idx]);
                
                scui_font_glyph_unit_t glyph_unit = {
                    .size = src_args->size,
                    .name = src_args->name,
                    .glyph.space_width = src_args->gap_none,
                    .glyph.unicode_letter = src_args->unicode[idx],
                };
                scui_font_glyph_cache_load(&glyph_unit);
                scui_font_glyph_cache_unload(&glyph_unit);
                
                SCUI_LOG_DEBUG("box_w:%d", glyph_unit.glyph.box_w);
                SCUI_LOG_DEBUG("box_h:%d", glyph_unit.glyph.box_h);
                SCUI_LOG_DEBUG("ofs_x:%d", glyph_unit.glyph.ofs_x);
                SCUI_LOG_DEBUG("ofs_y:%d", glyph_unit.glyph.ofs_y);
                
                scui_area_t letter_clip = dst_clip_v;
                scui_area_t glyph_clip = {
                    .w = glyph_unit.glyph.box_w,
                    .h = glyph_unit.glyph.box_h,
                };
                
                scui_point_t letter_offset = offset_line;
                
                // 抄录自:lv_draw_sw_letter
                letter_offset.x += glyph_unit.glyph.ofs_x;
                letter_offset.y += src_args->offset;
                letter_offset.y += (line_height - base_line) - glyph_unit.glyph.box_h - glyph_unit.glyph.ofs_y;
                
                SCUI_LOG_INFO("offset:%d", src_args->offset);
                SCUI_LOG_INFO("offset_line:<%d, %d>", offset_line.x, offset_line.y);
                SCUI_LOG_INFO("letter_offset:<%d, %d>", letter_offset.x, letter_offset.y);
                
                if (!scui_area_limit_offset(&letter_clip, &letter_offset)) {
                     scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset_line);
                     continue;
                }
                
                scui_point_t glyph_offset = {0};
                glyph_offset.x = letter_offset.x < 0 ? -letter_offset.x : 0;
                glyph_offset.y = letter_offset.y < 0 ? -letter_offset.y : 0;
                if (!scui_area_limit_offset(&glyph_clip, &glyph_offset)) {
                     scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset_line);
                     continue;
                }
                
                scui_area_t clip_inter = {0};
                if (!scui_area_inter(&clip_inter, &letter_clip, &dst_clip_v)) {
                     scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset_line);
                     continue;
                }
                
                if (glyph_unit.glyph.bitmap != NULL) {
                    letter_clip = clip_inter;
                    
                    scui_color_t glyph_color = src_args->color;
                    if (src_args->recolor && src_args->colors != NULL)
                    for (uint32_t idx_rec = 0; idx_rec < src_args->colors->color_num; idx_rec++) {
                        if (src_args->colors->index_ls[idx_rec] == -1 ||
                            src_args->colors->index_le[idx_rec] == -1 ||
                            src_args->colors->index_ls[idx_rec] > idx ||
                            src_args->colors->index_le[idx_rec] < idx)
                            continue;
                        
                        glyph_color = src_args->colors->color_ll[idx_rec];
                        break;
                    }
                    
                    scui_draw_letter(dst_surface, &letter_clip,
                        &glyph_unit.glyph, &glyph_clip, src_alpha, glyph_color);
                }
                
                scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset_line);
            }
            
            /* 下划线和删除线 */
            line_point_e = offset_line;
            if (src_args->line_delete || src_args->line_under) {
                line_point_s.x += dst_clip->x;
                line_point_s.y += dst_clip->y;
                line_point_e.x += dst_clip->x;
                line_point_e.y += dst_clip->y;
                scui_draw_graph_dsc_t draw_graph = {
                    .type           = scui_draw_graph_type_line,
                    .dst_surface    = dst_surface,
                    .dst_clip       = dst_clip,
                    .src_alpha      = src_alpha,
                    .src_color      = src_args->color,
                    .line.src_width = src_args->line_width,
                };
                scui_coord_t offset_line_delete = (line_height - src_args->line_width) / 2;
                scui_coord_t offset_line_under  = (line_height - base_line - underline);
                if (src_args->line_delete) {
                    draw_graph.line.src_pos_1 = line_point_s;
                    draw_graph.line.src_pos_2 = line_point_e;
                    draw_graph.line.src_pos_1.y += offset_line_delete;
                    draw_graph.line.src_pos_2.y += offset_line_delete;
                    draw_graph.line.src_pos_1.y += src_args->offset;
                    draw_graph.line.src_pos_2.y += src_args->offset;
                    scui_draw_graph_ctx(&draw_graph);
                }
                if (src_args->line_under) {
                    draw_graph.line.src_pos_1 = line_point_s;
                    draw_graph.line.src_pos_2 = line_point_e;
                    draw_graph.line.src_pos_1.y += offset_line_under;
                    draw_graph.line.src_pos_2.y += offset_line_under;
                    draw_graph.line.src_pos_1.y += src_args->offset;
                    draw_graph.line.src_pos_2.y += src_args->offset;
                    scui_draw_graph_ctx(&draw_graph);
                }
            }
        }
    } else {
        /* 单行模式不需要排版布局,绘制时手动计算偏移 */
        SCUI_ASSERT(src_args->typo == NULL);
        
        if (src_args->limit <= 0) {
            scui_coord_t line_w = -src_args->limit;
            if (src_args->align_hor == 0);
            if (src_args->align_hor == 1)
                offset.x += line_w;
            if (src_args->align_hor == 2)
                offset.x += line_w / 2;
        }
        if (src_args->align_ver == 0);
        if (src_args->align_ver == 1)
            offset.y += (src_clip_v.h - (line_height - base_line));
        if (src_args->align_ver == 2)
            offset.y += (src_clip_v.h - (line_height - base_line)) / 2;
        
        /* 下划线和删除线 */
        scui_point_t line_point_s = offset;
        scui_point_t line_point_e = offset;
        
        for (uint32_t idx = 0; idx < src_args->number; idx++) {
            
            if (src_args->unicode[idx] < 0x80)
                SCUI_LOG_DEBUG("letter:%c", src_args->unicode[idx]);
            else
                SCUI_LOG_DEBUG("letter:%x", src_args->unicode[idx]);
            
            scui_font_glyph_unit_t glyph_unit = {
                .size = src_args->size,
                .name = src_args->name,
                .glyph.space_width = src_args->gap_none,
                .glyph.unicode_letter = src_args->unicode[idx],
            };
            scui_font_glyph_cache_load(&glyph_unit);
            scui_font_glyph_cache_unload(&glyph_unit);
            
            SCUI_LOG_DEBUG("box_w:%d", glyph_unit.glyph.box_w);
            SCUI_LOG_DEBUG("box_h:%d", glyph_unit.glyph.box_h);
            SCUI_LOG_DEBUG("ofs_x:%d", glyph_unit.glyph.ofs_x);
            SCUI_LOG_DEBUG("ofs_y:%d", glyph_unit.glyph.ofs_y);
            
            scui_area_t letter_clip = dst_clip_v;
            scui_area_t glyph_clip = {
                .w = glyph_unit.glyph.box_w,
                .h = glyph_unit.glyph.box_h,
            };
            
            scui_point_t letter_offset = offset;
            
            // 抄录自:lv_draw_sw_letter
            letter_offset.x += src_args->offset + glyph_unit.glyph.ofs_x;
            letter_offset.y += (line_height - base_line) - glyph_unit.glyph.box_h - glyph_unit.glyph.ofs_y;
            
            SCUI_LOG_INFO("offset:%d", src_args->offset);
            SCUI_LOG_INFO("offset:<%d, %d>", offset.x, offset.y);
            SCUI_LOG_INFO("letter_offset:<%d, %d>", letter_offset.x, letter_offset.y);
            
            if (!scui_area_limit_offset(&letter_clip, &letter_offset)) {
                 scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset);
                 continue;
            }
            
            scui_point_t glyph_offset = {0};
            glyph_offset.x = letter_offset.x < 0 ? -letter_offset.x : 0;
            glyph_offset.y = letter_offset.y < 0 ? -letter_offset.y : 0;
            if (!scui_area_limit_offset(&glyph_clip, &glyph_offset)) {
                 scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset);
                 continue;
            }
            
            scui_area_t clip_inter = {0};
            if (!scui_area_inter(&clip_inter, &letter_clip, &dst_clip_v)) {
                 scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset);
                 continue;
            }
            
            if (glyph_unit.glyph.bitmap != NULL) {
                letter_clip = clip_inter;
                
                scui_color_t glyph_color = src_args->color;
                if (src_args->recolor && src_args->colors != NULL)
                for (uint32_t idx_rec = 0; idx_rec < src_args->colors->color_num; idx_rec++) {
                    if (src_args->colors->index_ls[idx_rec] == -1 ||
                        src_args->colors->index_le[idx_rec] == -1 ||
                        src_args->colors->index_ls[idx_rec] > idx ||
                        src_args->colors->index_le[idx_rec] < idx)
                        continue;
                    
                    glyph_color = src_args->colors->color_ll[idx_rec];
                    break;
                }
                
                scui_draw_letter(dst_surface, &letter_clip,
                    &glyph_unit.glyph, &glyph_clip, src_alpha, glyph_color);
            }
            
            scui_draw_ctx_string_offset(src_args, &glyph_unit.glyph, &offset);
        }
        
        /* 下划线和删除线 */
        line_point_e = offset;
        if (src_args->line_delete || src_args->line_under) {
            line_point_s.x += dst_clip->x;
            line_point_s.y += dst_clip->y;
            line_point_e.x += dst_clip->x;
            line_point_e.y += dst_clip->y;
            scui_draw_graph_dsc_t draw_graph = {
                .type           = scui_draw_graph_type_line,
                .dst_surface    = dst_surface,
                .dst_clip       = dst_clip,
                .src_alpha      = src_alpha,
                .src_color      = src_args->color,
                .line.src_width = src_args->line_width,
            };
            scui_coord_t offset_line_delete = (line_height - src_args->line_width) / 2;
            scui_coord_t offset_line_under  = (line_height - base_line - underline);
            if (src_args->line_delete) {
                draw_graph.line.src_pos_1 = line_point_s;
                draw_graph.line.src_pos_2 = line_point_e;
                draw_graph.line.src_pos_1.y += offset_line_delete;
                draw_graph.line.src_pos_2.y += offset_line_delete;
                draw_graph.line.src_pos_1.x += src_args->offset;
                draw_graph.line.src_pos_2.x += src_args->offset;
                scui_draw_graph_ctx(&draw_graph);
            }
            if (src_args->line_under) {
                draw_graph.line.src_pos_1 = line_point_s;
                draw_graph.line.src_pos_2 = line_point_e;
                draw_graph.line.src_pos_1.y += offset_line_under;
                draw_graph.line.src_pos_2.y += offset_line_under;
                draw_graph.line.src_pos_1.x += src_args->offset;
                draw_graph.line.src_pos_2.x += src_args->offset;
                scui_draw_graph_ctx(&draw_graph);
            }
        }
    }
}
