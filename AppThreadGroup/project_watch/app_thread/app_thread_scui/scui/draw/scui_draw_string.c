/*实现目标:
 *    字符串绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符串
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param src_args    字符串绘制参数
 *@param src_clip    画布绘制区域
 *@param src_name    字库名字句柄
 *@param src_alpha   字符透明度
 *@param src_utf8    字符串(utf8)
 */
void scui_draw_string(scui_surface_t     *dst_surface, scui_area_t *dst_clip,
                      scui_string_args_t *src_args,    scui_area_t *src_clip,
                      scui_handle_t       src_name,    scui_alpha_t src_alpha,
                      uint8_t            *src_utf8)
{
    /*多字符字符串绘制一般分为以下三步曲调:
     *步调1: 文字变形(例:阿拉伯语等)
     *       它的目标主要是将原unicode单字符索引
     *       根据前后字符关系映射到新的连字符索引
     *步调2: 文字排版(一般是多行文字)
     *       它的目标主要是将对齐效果作用到多行文字上去
     *       使得多字符在整体的显示效果上具有美观效果
     *步调3: 文字字符绘制
     *       它的目标主要是将字符按排版和索引
     *       一个个绘制到目标上面去
     */
    
    if (src_args->update) {
        src_args->update = false;
        scui_string_args_layout_clear(src_args->layout);
        src_args->layout = NULL;
    }
    
    scui_string_args_prepare(src_args, src_name, src_utf8);
    SCUI_ASSERT(src_args->unicode != NULL);
    
    /* 从字库中提取一些信息 */
    scui_font_unit_t font_unit = {.name = src_name,};
    scui_font_cache_load(&font_unit);
    scui_font_cache_unload(&font_unit);
    scui_coord_t base_line   = scui_font_base_line(font_unit.font);
    scui_coord_t line_height = scui_font_line_height(font_unit.font);
    
    scui_area_t letter_clip = {0};
    scui_area_t dst_clip_v  = *dst_clip;
    scui_area_t src_clip_v  = *src_clip;
    /* 边距融合 */
    src_clip_v.x += src_args->margin;
    src_clip_v.y += src_args->margin;
    src_clip_v.w -= src_args->margin * 2;
    src_clip_v.h -= src_args->margin * 2;
    
    if (src_args->layout) {
        
    } else {
        scui_point_t offset = {0};
        
        for (uint32_t idx = 0; idx < src_args->number; idx++) {
            
            scui_font_glyph_unit_t glyph_unit = {
                .name = src_name,
                .glyph.space_width = src_args->gap_none,
                .glyph.unicode_letter = src_args->unicode[idx],
            };
            
            if (glyph_unit.glyph.unicode_letter < 0x80)
                SCUI_LOG_INFO("letter:%c", glyph_unit.glyph.unicode_letter);
            else
                SCUI_LOG_INFO("letter:%x", glyph_unit.glyph.unicode_letter);
            
            scui_font_glyph_cache_load(&glyph_unit);
            scui_font_glyph_cache_unload(&glyph_unit);
            
            scui_area_t glyph_clip = {
                .w = glyph_unit.glyph.box_w,
                .h = glyph_unit.glyph.box_h,
            };
            
            letter_clip = src_clip_v;
            letter_clip.x += dst_clip_v.x;
            letter_clip.y += dst_clip_v.y;
            
            if (!scui_area_limit_offset(&letter_clip, &offset))
                 continue;
            
            scui_point_t letter_offset = {
                .x =  glyph_unit.glyph.ofs_x,
                .y = (line_height - glyph_unit.glyph.box_h),
                // .y = (line_height - glyph_unit.glyph.box_h) / 2 - glyph_unit.glyph.ofs_y - base_line,
            };
            
            SCUI_LOG_INFO("box_w:%d", glyph_unit.glyph.box_w);
            SCUI_LOG_INFO("box_h:%d", glyph_unit.glyph.box_h);
            SCUI_LOG_INFO("ofs_x:%d", glyph_unit.glyph.ofs_x);
            SCUI_LOG_INFO("ofs_y:%d", glyph_unit.glyph.ofs_y);
            SCUI_LOG_INFO("l_ofs_x:%d", letter_offset.x);
            SCUI_LOG_INFO("l_ofs_y:%d", letter_offset.y);
            
            if (!scui_area_limit_offset(&letter_clip, &letter_offset))
                 continue;
            
            scui_area_t clip_inter = {0};
            if (!scui_area_inter(&clip_inter, &letter_clip, &dst_clip_v))
                 continue;
            letter_clip = clip_inter;
            
            if (glyph_unit.glyph.bitmap != NULL) {
                
                scui_draw_letter(dst_surface,      &letter_clip,
                                &glyph_unit.glyph, &glyph_clip,
                                 src_alpha,         src_args->color,
                                !src_args->cover,   src_args->filter);
                
                if (src_args->line_way == 0)
                    offset.x += glyph_unit.glyph.box_w + src_args->gap_line;
                if (src_args->line_way == 1)
                    offset.y += glyph_unit.glyph.box_h + src_args->gap_line;
            } else {
                if (src_args->line_way == 0)
                    offset.x += src_args->gap_none + src_args->gap_line;
                if (src_args->line_way == 1)
                    offset.y += src_args->gap_none + src_args->gap_line;
            }
            
            #if 0   // test
            // 通过生成的lvgl_font.c的数据流做比较确认数据获取的准确性
            for (uint32_t idx = 0; idx < glyph_unit.glyph.bitmap_size; idx++) {
              if (idx % 8 == 0)
                  SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
                  SCUI_LOG_INFO_RAW("0x%02x ", glyph_unit.glyph.bitmap[idx]);
            }
            SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
            #endif
        }
    }
}
