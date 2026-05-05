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
    scui_surface_t    *dst_surface =  draw_dsc->letter.dst_surface;
    scui_area_t       *dst_clip    = &draw_dsc->letter.dst_clip;
    scui_area_t       *src_clip    = &draw_dsc->letter.src_clip;
    scui_alpha_t       src_alpha   =  draw_dsc->letter.src_alpha;
    scui_color_t       src_color   =  draw_dsc->letter.src_color;
    scui_font_glyph_t *src_glyph   =  draw_dsc->letter.src_glyph;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_clip != NULL);
    SCUI_ASSERT(src_glyph != NULL && src_clip != NULL);
    
    /* 使用scui_pixel_cf_alpha格式blend */
    scui_surface_t glyph_surface = {
        .pixel   = src_glyph->bitmap,
        .hor_res = src_glyph->box_w,
        .ver_res = src_glyph->box_h,
        .alpha   = src_alpha,
    };
    switch (src_glyph->bpp) {
    case 1: glyph_surface.format = scui_pixel_cf_alpha1; break;
    case 2: glyph_surface.format = scui_pixel_cf_alpha2; break;
    case 4: glyph_surface.format = scui_pixel_cf_alpha4; break;
    case 8: glyph_surface.format = scui_pixel_cf_alpha8; break;
    default: SCUI_LOG_WARN("unsupport bpp:%d", src_glyph->bpp);
        SCUI_ASSERT(false); return;
    }
    glyph_surface.pbyte   = scui_pixel_byte(glyph_surface.format);
    glyph_surface.stride  = glyph_surface.hor_res;
    glyph_surface.stride *= scui_pixel_bits(glyph_surface.format);
    glyph_surface.stride /= 8;
    
    scui_surface_t *src_surface = &glyph_surface;
    scui_draw_area_blend(true, dst_surface, *dst_clip,
        src_surface, *src_clip, src_color);
}

/*@brief 绘制字符串
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_string(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t     *dst_surface =  draw_dsc->string.dst_surface;
    scui_area_t        *dst_clip    = &draw_dsc->string.dst_clip;
    scui_area_t        *src_clip    = &draw_dsc->string.src_clip;
    scui_alpha_t        src_alpha   =  draw_dsc->string.src_alpha;
    scui_string_args_t *src_args    =  draw_dsc->string.src_args;
    /* draw dsc args<e> */
    
    /* 从字库中提取一些信息 */
    scui_cache_font_unit_t font_unit = {0};
    font_unit.name = src_args->name;
    font_unit.size = src_args->size;
    scui_cache_font_load(&font_unit);
    scui_coord_t base_line   = scui_font_base_line(font_unit.font);
    scui_coord_t line_height = scui_font_line_height(font_unit.font);
    scui_coord_t underline   = scui_font_underline(font_unit.font);
    scui_cache_font_unload(&font_unit);
    
    scui_area_t dst_clip_v = *dst_clip;   /* v:vaild */
    scui_area_t src_clip_v = *src_clip;   /* v:vaild */
    
    scui_area_t draw_area = {0};
    draw_area.w = scui_min(dst_clip_v.w, src_clip_v.w);
    draw_area.h = scui_min(dst_clip_v.h, src_clip_v.h);
    if (scui_area_empty(&draw_area))
        return;
    
    scui_point_t offset = {.x = src_clip_v.x,.y = src_clip_v.y,};
    
    if (src_args->line_multi) {
        SCUI_ASSERT(src_args->typo != NULL);
        
        if (src_args->limit <= 0) {
            if (src_args->align_ver == 0);
            if (src_args->align_ver == 1) offset.y += -src_args->limit;
            if (src_args->align_ver == 2) offset.y += -src_args->limit / 2;
        }
        
        /* 绘制每一个行 */
        for (uint32_t idx_line = 0; idx_line < src_args->typo->line_mum; idx_line++) {
             uint32_t line_s = src_args->typo->line_ofs_s[idx_line];
             uint32_t line_e = src_args->typo->line_ofs_e[idx_line];
             uint32_t line_w = src_args->typo->line_width[idx_line];
             SCUI_LOG_INFO("line:<%d, %d> <%d>", line_s, line_e, line_w);
             
             scui_point_t offset_line = offset;
             if (src_args->align_hor == 0);
             if (src_args->align_hor == 1) offset_line.x += (src_clip_v.w - line_w);
             if (src_args->align_hor == 2) offset_line.x += (src_clip_v.w - line_w) / 2;
             offset_line.y += (line_height + src_args->gap_line) * idx_line;
             
             /* 下划线和删除线 */
             scui_point_t line_point_s = offset_line;
             scui_point_t line_point_e = offset_line;
            
             /* 行绘制 */
             for (uint32_t idx = line_s; idx <= line_e; idx++) {
                  SCUI_ASSERT(idx >= 0 && idx < src_args->number);
                
                if (src_args->unicode[idx] < 0x80)
                    SCUI_LOG_DEBUG("letter:%c", src_args->unicode[idx]);
                else
                    SCUI_LOG_DEBUG("letter:%x", src_args->unicode[idx]);
                
                scui_cache_glyph_unit_t glyph_unit = {
                    .size = src_args->size,
                    .name = src_args->name,
                    .glyph.space_width = src_args->gap_none,
                    .glyph.unicode_letter = src_args->unicode[idx],
                };
                scui_cache_glyph_load(&glyph_unit);
                scui_cache_glyph_unload(&glyph_unit);
                
                SCUI_LOG_DEBUG("box_w:%d", glyph_unit.glyph.box_w);
                SCUI_LOG_DEBUG("box_h:%d", glyph_unit.glyph.box_h);
                SCUI_LOG_DEBUG("ofs_x:%d", glyph_unit.glyph.ofs_x);
                SCUI_LOG_DEBUG("ofs_y:%d", glyph_unit.glyph.ofs_y);
                
                /* 抄录自:lv_draw_sw_letter */
                scui_point_t letter_offset = offset_line;
                letter_offset.x += glyph_unit.glyph.ofs_x;
                letter_offset.y += src_args->offset;
                letter_offset.y += (line_height - base_line) - glyph_unit.glyph.box_h - glyph_unit.glyph.ofs_y;
                
                SCUI_LOG_INFO("offset:%d", src_args->offset);
                SCUI_LOG_INFO("offset_line:<%d, %d>", offset_line.x, offset_line.y);
                SCUI_LOG_INFO("letter_offset:<%d, %d>", letter_offset.x, letter_offset.y);
                
                scui_area_t letter_clip = dst_clip_v;
                if (!scui_area_limit_offset(&letter_clip, &letter_offset)) {
                    offset_line.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                        (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
                     continue;
                }
                
                scui_point_t glyph_offset = {
                    .x = letter_offset.x < 0 ? -letter_offset.x : 0,
                    .y = letter_offset.y < 0 ? -letter_offset.y : 0,
                };
                scui_area_t glyph_clip = {
                    .w = glyph_unit.glyph.box_w,
                    .h = glyph_unit.glyph.box_h,
                };
                if (!scui_area_limit_offset(&glyph_clip, &glyph_offset)) {
                    offset_line.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                        (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
                     continue;
                }
                
                if (!scui_area_inter2(&letter_clip, &dst_clip_v)) {
                    offset_line.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                        (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
                     continue;
                }
                
                if (glyph_unit.glyph.bitmap != NULL) {
                    
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
                    
                    scui_draw_letter(true, dst_surface, letter_clip,
                        &glyph_unit.glyph, glyph_clip, src_alpha, glyph_color);
                }
                
                offset_line.x += (glyph_unit.glyph.bitmap == NULL ? src_args->gap_none :
                    (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
            }
            
            /* 下划线和删除线 */
            line_point_e.x += line_w - src_args->gap_item;
            if (src_args->line_delete || src_args->line_under) {
                line_point_s.x += dst_clip->x;
                line_point_s.y += dst_clip->y;
                line_point_e.x += dst_clip->x;
                line_point_e.y += dst_clip->y;
                scui_coord_t offset_line_delete = (line_height - src_args->line_width) / 2;
                scui_coord_t offset_line_under  = (line_height - base_line - underline);
                if (src_args->line_delete) {
                    scui_draw_dsc_t draw_dsc_line = {0};
                    draw_dsc_line.type = scui_draw_type_pixel_line;
                    draw_dsc_line.graph.src_width = src_args->line_width;
                    
                    draw_dsc_line.graph.src_pos_1 = line_point_s;
                    draw_dsc_line.graph.src_pos_2 = line_point_e;
                    draw_dsc_line.graph.src_pos_1.y += offset_line_delete;
                    draw_dsc_line.graph.src_pos_2.y += offset_line_delete;
                    draw_dsc_line.graph.src_pos_1.y += src_args->offset;
                    draw_dsc_line.graph.src_pos_2.y += src_args->offset;
                    scui_draw_graph(true, dst_surface, *dst_clip,
                        src_alpha, src_args->color, &draw_dsc_line);
                }
                if (src_args->line_under) {
                    scui_draw_dsc_t draw_dsc_line = {0};
                    draw_dsc_line.type = scui_draw_type_pixel_line;
                    draw_dsc_line.graph.src_width = src_args->line_width;
                    
                    draw_dsc_line.graph.src_pos_1 = line_point_s;
                    draw_dsc_line.graph.src_pos_2 = line_point_e;
                    draw_dsc_line.graph.src_pos_1.y += offset_line_under;
                    draw_dsc_line.graph.src_pos_2.y += offset_line_under;
                    draw_dsc_line.graph.src_pos_1.y += src_args->offset;
                    draw_dsc_line.graph.src_pos_2.y += src_args->offset;
                    scui_draw_graph(true, dst_surface, *dst_clip,
                        src_alpha, src_args->color, &draw_dsc_line);
                }
            }
        }
    } else {
        /* 单行模式不需要排版布局,绘制时手动计算偏移 */
        SCUI_ASSERT(src_args->typo == NULL);
        
        if (src_args->limit <= 0) {
            scui_coord_t line_w = -src_args->limit;
            if (src_args->align_hor == 0);
            if (src_args->align_hor == 1) offset.x += line_w;
            if (src_args->align_hor == 2) offset.x += line_w / 2;
        }
        if (src_args->align_ver == 0);
        if (src_args->align_ver == 1) offset.y += (src_clip_v.h - (line_height - base_line));
        if (src_args->align_ver == 2) offset.y += (src_clip_v.h - (line_height - base_line)) / 2;
        
        /* 下划线和删除线 */
        scui_point_t line_point_s = offset;
        scui_point_t line_point_e = offset;
        
        for (uint32_t idx = 0; idx < src_args->number; idx++) {
            
            if (src_args->unicode[idx] < 0x80)
                SCUI_LOG_DEBUG("letter:%c", src_args->unicode[idx]);
            else
                SCUI_LOG_DEBUG("letter:%x", src_args->unicode[idx]);
            
            scui_cache_glyph_unit_t glyph_unit = {
                .size = src_args->size,
                .name = src_args->name,
                .glyph.space_width = src_args->gap_none,
                .glyph.unicode_letter = src_args->unicode[idx],
            };
            scui_cache_glyph_load(&glyph_unit);
            scui_cache_glyph_unload(&glyph_unit);
            
            SCUI_LOG_DEBUG("box_w:%d", glyph_unit.glyph.box_w);
            SCUI_LOG_DEBUG("box_h:%d", glyph_unit.glyph.box_h);
            SCUI_LOG_DEBUG("ofs_x:%d", glyph_unit.glyph.ofs_x);
            SCUI_LOG_DEBUG("ofs_y:%d", glyph_unit.glyph.ofs_y);
            
            /* 抄录自:lv_draw_sw_letter */
            scui_point_t letter_offset = offset;
            letter_offset.x += src_args->offset + glyph_unit.glyph.ofs_x;
            letter_offset.y += (line_height - base_line) - glyph_unit.glyph.box_h - glyph_unit.glyph.ofs_y;
            
            SCUI_LOG_INFO("offset:%d", src_args->offset);
            SCUI_LOG_INFO("offset:<%d, %d>", offset.x, offset.y);
            SCUI_LOG_INFO("letter_offset:<%d, %d>", letter_offset.x, letter_offset.y);
            
            scui_area_t letter_clip = dst_clip_v;
            if (!scui_area_limit_offset(&letter_clip, &letter_offset)) {
                 offset.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                    (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
                 continue;
            }
            
            scui_point_t glyph_offset = {
                .x = letter_offset.x < 0 ? -letter_offset.x : 0,
                .y = letter_offset.y < 0 ? -letter_offset.y : 0,
            };
            scui_area_t glyph_clip = {
                .w = glyph_unit.glyph.box_w,
                .h = glyph_unit.glyph.box_h,
            };
            if (!scui_area_limit_offset(&glyph_clip, &glyph_offset)) {
                 offset.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                    (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
                 continue;
            }
            
            if (!scui_area_inter2(&letter_clip, &dst_clip_v)) {
                 offset.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                    (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
                 continue;
            }
            
            if (glyph_unit.glyph.bitmap != NULL) {
                
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
                
                scui_draw_letter(true, dst_surface, letter_clip,
                    &glyph_unit.glyph, glyph_clip, src_alpha, glyph_color);
            }
            
            offset.x += (glyph_unit.glyph.bitmap == 0 ? src_args->gap_none :
                (glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w)) + src_args->gap_item;
        }
        
        /* 下划线和删除线 */
        line_point_e = offset;
        if (src_args->line_delete || src_args->line_under) {
            line_point_s.x += dst_clip->x;
            line_point_s.y += dst_clip->y;
            line_point_e.x += dst_clip->x;
            line_point_e.y += dst_clip->y;
            if (src_args->line_delete) {
                scui_draw_dsc_t draw_dsc_line = {0};
                draw_dsc_line.type = scui_draw_type_pixel_line;
                draw_dsc_line.graph.src_width = src_args->line_width;
                
                draw_dsc_line.graph.src_pos_1 = line_point_s;
                draw_dsc_line.graph.src_pos_2 = line_point_e;
                draw_dsc_line.graph.src_pos_1.y += (line_height - src_args->line_width) / 2;
                draw_dsc_line.graph.src_pos_2.y += (line_height - src_args->line_width) / 2;
                draw_dsc_line.graph.src_pos_1.x += src_args->offset;
                draw_dsc_line.graph.src_pos_2.x += src_args->offset;
                scui_draw_graph(true, dst_surface, *dst_clip,
                    src_alpha, src_args->color, &draw_dsc_line);
            }
            if (src_args->line_under) {
                scui_draw_dsc_t draw_dsc_line = {0};
                draw_dsc_line.type = scui_draw_type_pixel_line;
                draw_dsc_line.graph.src_width = src_args->line_width;
                
                draw_dsc_line.graph.src_pos_1 = line_point_s;
                draw_dsc_line.graph.src_pos_2 = line_point_e;
                draw_dsc_line.graph.src_pos_1.y += (line_height - base_line - underline);
                draw_dsc_line.graph.src_pos_2.y += (line_height - base_line - underline);
                draw_dsc_line.graph.src_pos_1.x += src_args->offset;
                draw_dsc_line.graph.src_pos_2.x += src_args->offset;
                scui_draw_graph(true, dst_surface, *dst_clip,
                    src_alpha, src_args->color, &draw_dsc_line);
            }
        }
    }
}
