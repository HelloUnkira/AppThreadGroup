/*实现目标:
 *    字符串布局资源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符偏移计算
 *@param args   字符串绘制参数
 *@param glyph  字符
 *@param offset 偏移量
 */
void scui_string_args_offset(scui_string_args_t *args, scui_font_glyph_t *glyph, scui_point_t *offset)
{
    if (glyph->bitmap != 0) {
        if (args->line_way)
            offset->y += glyph->box_h + args->gap_item;
        else
            #if 0
            offset->x += glyph->box_w + args->gap_item;
            #else
            offset->x += glyph->ofs_x + glyph->box_w + args->gap_item;
            #endif
    } else {
        if (args->line_way)
            offset->y += args->gap_none + args->gap_item;
        else
            offset->x += args->gap_none + args->gap_item;
    }
}

/*@brief 字符串处理(变形,连字符等等)
 *@param args 字符串绘制参数
 */
void scui_string_args_process(scui_string_args_t *args)
{
    SCUI_ASSERT(args != NULL);
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
    
    if (!args->update)
         return;
    
    args->update = false;
    
    /* 清空旧有布局 */
    if (args->layout != NULL) {
        SCUI_MEM_FREE(args->layout);
        args->layout = NULL;
    }
    
    /* 清空旧有文本 */
    if (args->unicode != NULL) {
        SCUI_MEM_FREE(args->unicode);
        args->unicode = NULL;
    }
    
    args->width  = 0;
    args->height = 0;
    args->offset = 0;
    
    if (args->utf8 == NULL || args->name == SCUI_HANDLE_INVALID)
        return;
    
    /* 统计字符数量 */
    args->number  = scui_utf8_str_num(args->utf8);
    args->unicode = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (args->number + 1));
    memset(args->unicode, 0, 4 * (args->number + 1));
    /* 将utf8转为unicode */
    scui_utf8_str_to_unicode(args->utf8, args->number, args->unicode);
    
    /* 从字库中提取一些信息 */
    scui_font_unit_t font_unit = {.name = args->name,};
    scui_font_cache_load(&font_unit);
    scui_font_cache_unload(&font_unit);
    scui_coord_t base_line   = scui_font_base_line(font_unit.font);
    scui_coord_t line_height = scui_font_line_height(font_unit.font);
    
    /* 特定字体进行文字变形 */
    /* ... */
    
    /* 文字进行排版布局(单行模式不需要排版布局) */
    /* ... */
    
    if (args->layout != NULL) {
        /* ... */
        SCUI_ASSERT(false);
    }
    
    /* 统计宽度和高度 */
    args->width  = 0;
    args->height = 0;
    
    for (uint32_t idx = 0; idx < args->number; idx++) {
        
        scui_font_glyph_unit_t glyph_unit = {0};
        glyph_unit.name = args->name;
        glyph_unit.glyph.space_width = args->gap_none;
        glyph_unit.glyph.unicode_letter = args->unicode[idx];
        
        if (glyph_unit.glyph.unicode_letter < 0x80)
            SCUI_LOG_INFO("letter:%c", glyph_unit.glyph.unicode_letter);
        else
            SCUI_LOG_INFO("letter:%x", glyph_unit.glyph.unicode_letter);
        
        scui_font_glyph_cache_load(&glyph_unit);
        scui_font_glyph_cache_unload(&glyph_unit);
        
        if (args->line_way)
            args->width  = scui_max(glyph_unit.glyph.box_w, args->width);
        else
            args->height = scui_max(glyph_unit.glyph.box_h, args->height);
    }
    
    scui_point_t offset = {0};
    scui_area_t  src_clip_v  = args->clip;
    
    for (uint32_t idx = 0; idx < args->number; idx++) {
        
        scui_font_glyph_unit_t glyph_unit = {0};
        glyph_unit.name = args->name;
        glyph_unit.glyph.space_width = args->gap_none;
        glyph_unit.glyph.unicode_letter = args->unicode[idx];
        
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
        
        if (args->line_multi) {
            if (args->line_way) {
                if (offset.y + glyph_clip.h + args->gap_item > src_clip_v.h) {
                    offset.y  = 0;
                    offset.x += args->width  + args->gap_line;
                }
            } else {
                if (offset.x + glyph_clip.w + args->gap_item > src_clip_v.w) {
                    offset.x  = 0;
                    #if 0
                    offset.y += args->height + args->gap_line;
                    #else
                    offset.y += line_height + args->gap_line;
                    #endif
                }
            }
        }
        
        if (args->line_way)
            args->height += glyph_unit.glyph.box_h + args->gap_item;
        else
            args->width  += glyph_unit.glyph.ofs_x + glyph_unit.glyph.box_w + args->gap_item;
        
        scui_string_args_offset(args, &glyph_unit.glyph, &offset);
    }
    
    if (args->line_multi) {
        if (args->line_way) {
            offset.x   += args->width;
            args->limit = offset.x - src_clip_v.w;
        } else {
            #if 0
            offset.y   += args->height;
            #else
            offset.y   += line_height;
            #endif
            args->limit = offset.y - src_clip_v.h;
        }
    } else {
        if (args->line_way) {
            args->height -= args->gap_item;
            args->limit   = args->height - src_clip_v.h;
        } else {
            args->width  -= args->gap_item;
            args->limit   = args->width  - src_clip_v.w;
        }
    }
}
