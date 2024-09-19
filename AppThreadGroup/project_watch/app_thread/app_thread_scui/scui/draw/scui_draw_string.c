/*实现目标:
 *    字符串绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符偏移计算
 *@param args   字符串绘制参数
 *@param glyph  字符
 *@param offset 偏移量
 */
static void scui_draw_string_offset(scui_string_args_t *args, scui_font_glyph_t *glyph, scui_point_t *offset)
{
    if (glyph->bitmap != 0)
        offset->x += glyph->ofs_x + glyph->box_w + args->gap_item;
    else
        offset->x += args->gap_none + args->gap_item;
}

/*@brief 绘制字符串
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_args    字符串绘制参数
 *@param src_clip    画布绘制区域
 *@param src_alpha   字符透明度
 */
void scui_draw_string(scui_surface_t     *dst_surface, scui_area_t *dst_clip,
                      scui_string_args_t *src_args,    scui_area_t *src_clip,
                      scui_alpha_t        src_alpha)
{
    /* 从字库中提取一些信息 */
    scui_font_unit_t font_unit = {.name = src_args->name,};
    scui_font_cache_load(&font_unit);
    scui_font_cache_unload(&font_unit);
    scui_coord_t base_line   = scui_font_base_line(font_unit.font);
    scui_coord_t line_height = scui_font_line_height(font_unit.font);
    
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
                offset.x += line_h;
            if (src_args->align_ver == 2)
                offset.x += line_h / 2;
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
                 offset_line.x += src_clip_v.w - line_w;
             if (src_args->align_hor == 2)
                 offset_line.x += (src_clip_v.w - line_w) / 2;
             
             SCUI_LOG_INFO("line:<%d, %d> <%d>", line_s, line_e, line_w);
             
             // 行绘制
             for (uint32_t idx = line_s; idx <= line_e; idx++) {
                  SCUI_ASSERT(idx >= 0 && idx < src_args->number);
                  
                  if (src_args->unicode[idx] < 0x80)
                      SCUI_LOG_DEBUG("letter:%c", src_args->unicode[idx]);
                  else
                      SCUI_LOG_DEBUG("letter:%x", src_args->unicode[idx]);
                  
                  scui_font_glyph_unit_t glyph_unit = {
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
                       scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset_line);
                       continue;
                  }
                  
                  scui_point_t glyph_offset = {0};
                  glyph_offset.x = letter_offset.x < 0 ? -letter_offset.x : 0;
                  glyph_offset.y = letter_offset.y < 0 ? -letter_offset.y : 0;
                  if (!scui_area_limit_offset(&glyph_clip, &glyph_offset)) {
                       scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset_line);
                       continue;
                  }
                  
                  scui_area_t clip_inter = {0};
                  if (!scui_area_inter(&clip_inter, &letter_clip, &dst_clip_v)) {
                       scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset_line);
                       continue;
                  }
                  
                  if (glyph_unit.glyph.bitmap != NULL) {
                      letter_clip = clip_inter;
                      scui_draw_letter(dst_surface, &letter_clip, &glyph_unit.glyph, &glyph_clip, src_alpha, src_args->color);
                  }
                  
                  scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset_line);
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
        
        for (uint32_t idx = 0; idx < src_args->number; idx++) {
            
            if (src_args->unicode[idx] < 0x80)
                SCUI_LOG_DEBUG("letter:%c", src_args->unicode[idx]);
            else
                SCUI_LOG_DEBUG("letter:%x", src_args->unicode[idx]);
            
            scui_font_glyph_unit_t glyph_unit = {
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
                 scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset);
                 continue;
            }
            
            scui_point_t glyph_offset = {0};
            glyph_offset.x = letter_offset.x < 0 ? -letter_offset.x : 0;
            glyph_offset.y = letter_offset.y < 0 ? -letter_offset.y : 0;
            if (!scui_area_limit_offset(&glyph_clip, &glyph_offset)) {
                 scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset);
                 continue;
            }
            
            scui_area_t clip_inter = {0};
            if (!scui_area_inter(&clip_inter, &letter_clip, &dst_clip_v)) {
                 scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset);
                 continue;
            }
            
            if (glyph_unit.glyph.bitmap != NULL) {
                letter_clip = clip_inter;
                scui_draw_letter(dst_surface, &letter_clip, &glyph_unit.glyph, &glyph_clip, src_alpha, src_args->color);
            }
            
            scui_draw_string_offset(src_args, &glyph_unit.glyph, &offset);
            
            #if 0   // test
            // 通过生成的lvgl_font.c的数据流做比较确认数据获取的准确性
            for (uint32_t idx_g = 0; idx_g < glyph_unit.glyph.bitmap_size; idx_g++) {
              if (idx_g % 8 == 0)
                  SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
                  SCUI_LOG_INFO_RAW("0x%02x ", glyph_unit.glyph.bitmap[idx_g]);
            }
            SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
            #endif
        }
    }
}
