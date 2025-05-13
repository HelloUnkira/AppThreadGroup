/*实现目标:
 *    字符串布局资源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制字符查找一个单词
 *@param args    字符串绘制参数
 *@param match_s 起始点
 *@param match_e 结束点
 *@param width   单词总宽度
 *@param width   单词总宽度限制
 */
static void scui_string_args_find_word(scui_string_args_t *args, uint32_t *idx_s, uint32_t *idx_e, uint32_t *width, uint32_t *width_max)
{
    uint32_t idx = 0;
    *idx_e = *idx_s;
    *width = 0;
    
    for (idx = *idx_s; idx < args->number; idx++) {
        /* 文字信息缓存节点实例映射 */
        scui_font_glyph_unit_t glyph_unit = {
            .size                   = args->size,
            .name                   = args->name,
            .glyph.space_width      = args->gap_none,
            .glyph.unicode_letter   = args->unicode[idx],
        };
        scui_font_glyph_cache_load(&glyph_unit);
        
        uint32_t glyph_width = 0;
        glyph_width += glyph_unit.glyph.ofs_x;
        glyph_width += glyph_unit.glyph.box_w;
        glyph_width += args->gap_item;
        
        scui_font_glyph_cache_unload(&glyph_unit);
        
        if (width_max != NULL && *width + glyph_width > *width_max) {
           *idx_e = idx - 1;
            break;
        }
        *width += glyph_width;
        
        // 找可以明确分割单词的字符
        bool break_char = false;
        
        if (args->unicode[idx] == '\n' ||
            args->unicode[idx] == '\r')
            break_char = true;
        
        // 中文字符都可单独为一个单词
        if (args->unicode[idx] >= 0x4E00 &&
            args->unicode[idx] <= 0x9FA5)   // 0x4E00
            break_char = true;
        
        // 被标记的字符都可以分割单词
        for (uint8_t idx_k = 0; !break_char && SCUI_STRING_BREAK_CHAR[idx_k] != '\0'; idx_k++)
            if (args->unicode[idx] == SCUI_STRING_BREAK_CHAR[idx_k])
                break_char = true;
        
        if (break_char) {
           *idx_e = idx;
            break;
        }
    }
    
    if (idx ==  args->number) {
       *idx_e = args->number - 1;
    }
}

/*@brief 字符串处理
 *@param args 字符串绘制参数
 */
static void scui_string_args_prepare(scui_string_args_t *args)
{
    /* 预处理:将utf8->转为unicode */
    
    /* 清空旧有文本 */
    if (args->unicode != NULL) {
        SCUI_MEM_FREE(args->unicode);
        args->unicode = NULL;
    }
    
    /* 提前退出,用于回收时使用 */
    if (args->utf8 == NULL)
        return;
    
    /* 统计字符数量 */
    args->number  = scui_utf8_str_num(args->utf8);
    args->unicode = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (args->number + 1));
    memset(args->unicode, 0, 4 * (args->number + 1));
    /* 将utf8转为unicode */
    scui_utf8_str_to_unicode(args->utf8, args->number, args->unicode);
}

/*@brief 字符串处理
 *@param args 字符串绘制参数
 */
static void scui_string_args_transform(scui_string_args_t *args)
{
    if (args->utf8 == NULL || args->number == 0 ||
        args->name == SCUI_HANDLE_INVALID)
        return;
    
    /* 文字变形:将特殊的连序unicode替换成新的映射unicode */
}

/*@brief 字符串处理
 *@param args 字符串绘制参数
 */
static void scui_string_args_typography(scui_string_args_t *args)
{
    /* 清空已有排版 */
    if (args->typo != NULL) {
        if (args->typo->line_ofs != NULL)
            SCUI_MEM_FREE(args->typo->line_ofs);
        if (args->typo->line_width != NULL)
            SCUI_MEM_FREE(args->typo->line_width);
        SCUI_MEM_FREE(args->typo);
        args->typo = NULL;
    }
    
    if (args->utf8 == NULL || args->number == 0 ||
        args->name == SCUI_HANDLE_INVALID)
        return;
    
    args->width  = 0;
    args->height = 0;
    // 最大行宽为绘制区域的宽度
    scui_area_t  src_clip_v = args->clip;
    scui_point_t src_offset = {0};
    /* 从字库中提取一些信息 */
    scui_font_unit_t font_unit = {0};
    font_unit.name = args->name;
    font_unit.size = args->size;
    scui_font_cache_load(&font_unit);
    scui_coord_t base_line   = scui_font_base_line(font_unit.font);
    scui_coord_t line_height = scui_font_line_height(font_unit.font);
    scui_font_cache_unload(&font_unit);
    
    if (args->line_multi) {
        /* 文字排版是在虚拟区域内进行的 */
        /* 也即当已知最大绘制宽度的情况下, 进行多行的排布 */
        
        // 先做统计,将信息填入到临时开辟的空间之中
        uint32_t line_w_list[SCUI_STRING_LIMIT_LINE] = {0};
        uint32_t line_o_list[SCUI_STRING_LIMIT_LINE] = {0};
        uint32_t line_n = 1;
        
        for (uint32_t idx = 0; idx < args->number; idx++) {
            
            if (args->unicode[idx] < 0x80)
                SCUI_LOG_DEBUG("letter:%c", args->unicode[idx]);
            else
                SCUI_LOG_DEBUG("letter:%x", args->unicode[idx]);
            
            // 先拿一个单词,尝试填满
            uint32_t word_s = idx;
            uint32_t word_e = 0;
            uint32_t word_w = 0;
            scui_string_args_find_word(args, &word_s, &word_e, &word_w, NULL);
            
            // 本行还能接受一个单词,接受它
            if (line_w_list[line_n - 1] +  word_w < src_clip_v.w) {
                line_w_list[line_n - 1] += word_w;
                line_o_list[line_n - 1]  = word_e;
                idx = word_e;
                continue;
            }
            
            // 一个单词填不满,且不是单字符,限制填充
            if (line_w_list[line_n - 1] == 0 && word_e - word_s > 0) {
                uint32_t word_s = idx;
                uint32_t word_e = 0;
                uint32_t word_w = 0;
                uint32_t word_m = src_clip_v.w - line_w_list[line_n];
                scui_string_args_find_word(args, &word_s, &word_e, &word_w, &word_m);
                if (word_w != 0) {
                    line_w_list[line_n - 1] += word_w;
                    line_o_list[line_n - 1]  = word_e;
                    idx = word_e;
                    continue;
                }
            }
            
            // 换行,退格重填
            SCUI_ASSERT(line_w_list[line_n - 1] <= src_clip_v.w);
            line_n++;
            idx--;
        }
        
        // 是否到达最后一个字符
        SCUI_ASSERT(line_o_list[line_n - 1] == args->number - 1);
        // 生成排版
        args->typo = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(scui_string_typo_t));
        args->typo->line_mum = line_n;
        args->typo->line_ofs = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(uint32_t) * line_n);
        args->typo->line_width = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(uint32_t) * line_n);
        
        SCUI_LOG_INFO("typo num:%d", line_n);
        for (uint8_t idx = 0; idx < line_n; idx++) {
            args->typo->line_ofs[idx]   = line_o_list[idx];
            args->typo->line_width[idx] = line_w_list[idx];
            SCUI_LOG_INFO("line end:%d",   args->typo->line_ofs[idx]);
            SCUI_LOG_INFO("line width:%d", args->typo->line_width[idx]);
            SCUI_ASSERT(line_w_list[idx] <= src_clip_v.w);
        }
        
        args->height = line_n * (line_height + args->gap_line) - args->gap_line;
        args->limit  = args->height - src_clip_v.h;
    } else {
        /* 单行模式下只需统计最大行宽即可 */
        for (uint32_t idx = 0; idx < args->number; idx++) {
            
            if (args->unicode[idx] < 0x80)
                SCUI_LOG_DEBUG("letter:%c", args->unicode[idx]);
            else
                SCUI_LOG_DEBUG("letter:%x", args->unicode[idx]);
            
            /* 文字信息缓存节点实例映射 */
            scui_font_glyph_unit_t glyph_unit = {
                .size                   = args->size,
                .name                   = args->name,
                .glyph.space_width      = args->gap_none,
                .glyph.unicode_letter   = args->unicode[idx],
            };
            scui_font_glyph_cache_load(&glyph_unit);
            
            args->width += glyph_unit.glyph.ofs_x;
            args->width += glyph_unit.glyph.box_w;
            args->width += args->gap_item;
            
            scui_font_glyph_cache_unload(&glyph_unit);
        }
        args->width -= args->gap_item;
        args->limit  = args->width - src_clip_v.w;
    }
}

/*@brief 字符串处理
 *@param args 字符串绘制参数
 */
void scui_string_args_process(scui_string_args_t *args)
{
    SCUI_ASSERT(args != NULL);
    // 无需更新时不处理文字
    if (!args->update)
         return;
    // 更新参数初始状态
    args->update = false;
    args->width  = 0;
    args->height = 0;
    args->limit  = 0;
    args->offset = 0;
    
    /*字符串绘制一般分为以下三步曲调:
     *步调1: 文字预处理(例:阿拉伯语等)
     *       它的目标主要是将原unicode单字符索引
     *       根据前后字符关系映射到新的连字符索引
     *步调2: 文字排版(多行文字绘制时使用)
     *       它的目标主要是将对齐效果作用到多行文字上去
     *       使得多字符在整体的显示效果上具有美观效果
     *步调3: 文字字符绘制
     *       它的目标主要是将字符按排版和索引
     *       一个个绘制到目标上面去
     */
    
    /* 文字预处理,信息准备 */
    scui_string_args_prepare(args);
    /* 特定文字进行文字变形 */
    scui_string_args_transform(args);
    /* 文字进行排版布局(单行模式不需要排版布局) */
    scui_string_args_typography(args);
}

/*****************************************************************************/
/* utf-8 unicode tools<s>:************************************************** */
/*****************************************************************************/

/*@brief utf8字符转unicode字符
 *@param utf8    utf8字符
 *@param unicode unicode字符
 *@retval utf8字符字节数
 */
uint32_t scui_utf8_to_unicode(uint8_t *utf8, uint32_t *unicode)
{
    if (utf8[0] < 0x80) {
        
       *unicode = utf8[0];
        return 1;
    }
    if (utf8[0] < 0xE0) {
        
       *unicode = ((utf8[0] & 0x1F) << 6) |
                   (utf8[1] & 0x3F);
        return 2;
    }
    if (utf8[0] < 0xF0) {
        
       *unicode = ((utf8[0] & 0x0F) << 12) |
                  ((utf8[1] & 0x3F) << 6) |
                   (utf8[2] & 0x3F);
        return 3;
    }
    if (0xF0 == (utf8[0] & 0xF0)) {
        
       *unicode = ((utf8[0] & 0x07) << 18) |
                  ((utf8[1] & 0x3F) << 12) |
                  ((utf8[2] & 0x3F) << 6) |
                   (utf8[3] & 0x3F);
        return 4;
    }
    
    /* 新标准只支持utf8映射到unicode的字符集(1~4字节) */
    SCUI_ASSERT(false);
    return 0;
}

/*@brief utf8字符字节数
 *@param utf8 字符(首字符)
 *@retval 字符长度
 */
uint32_t scui_utf8_bytes(uint8_t utf8)
{
    if (utf8 <= 0x7F)                   // ASCII 占用1个字节
        return 1;
    if (utf8 >= 0xC0 && utf8 <= 0xDF)   // UTF-8 占用2个字节
        return 2;
    if (utf8 >= 0xE0 && utf8 <= 0xEF)   // UTF-8 占用3个字节
        return 3;
    if (utf8 >= 0xF0 && utf8 <= 0xF7)   // UTF-8 占用4个字节
        return 4;
    if (utf8 >= 0xF8 && utf8 <= 0xFB)   // UTF-8 占用5个字节
        return 5;
    if (utf8 >= 0xFC && utf8 <= 0xFD)   // UTF-8 占用6个字节
        return 6;
    
    #if 1
    // 我不知道为什么代码会走到此处
    SCUI_LOG_DEBUG("here ?");
    uint8_t offset = 1;
    
    if(utf8 & 128)
        if(utf8 & 32)
            if(utf8 & 16)
                if(utf8 & 8)
                    if(utf8 & 4) offset = 6;
                    else offset = 5;
                else offset = 4;
            else offset = 3;
        else offset = 2;
    return offset;
    #endif
    
    /* 新标准只支持utf8映射到unicode的字符集(1~4字节) */
    // UTF-8 非首字节
    SCUI_ASSERT(false);
    return 0;
}

/*@brief utf8字符数量
 *@param utf8 字符串
 *@retval 字符数量
 */
uint32_t scui_utf8_str_num(uint8_t *utf8)
{
    /* 统计utf8字符数量 */
    uint32_t str_num = 0;
    while (*utf8 != '\0') {
        uint8_t utf8_len = scui_utf8_bytes(*utf8);
        utf8 += utf8_len;
        str_num++;
    }
    
    return str_num;
}

/*@brief utf8字节数量
 *@param utf8 字符串
 *@retval 字节数量
 */
uint32_t scui_utf8_str_bytes(uint8_t *utf8)
{
    /* 统计utf8字节数量(不包括结尾符) */
    uint32_t str_bytes = 0;
    while (*utf8 != '\0') {
        uint8_t utf8_bytes = scui_utf8_bytes(*utf8);
        utf8 += utf8_bytes;
        str_bytes += utf8_bytes;
    }
    
    return str_bytes;
}

/*@brief utf8字符串转为unicode字符串
 *@param unicode  unicode字符串
 *@param utf8     utf8字符串
 *@param utf8_num utf8字符数
 */
void scui_utf8_str_to_unicode(uint8_t *utf8, uint32_t utf8_num, uint32_t *unicode)
{
    uint8_t *utf8_ptr = utf8;
    for (uint32_t idx = 0; idx < utf8_num; idx++) {
         uint8_t utf8_len = scui_utf8_to_unicode(utf8_ptr, &unicode[idx]);
         SCUI_ASSERT(utf8_len != 0);
         utf8_ptr += utf8_len;
    }
}

/*@brief utf8字符串后缀匹配
 *@param str    utf8字符串
 *@param suffix utf8字符串
 */
bool scui_utf8_str_match_suffix(uint8_t *str, uint8_t *suffix)
{
    uint32_t bytes_str    = scui_utf8_str_bytes(str);
    uint32_t bytes_suffix = scui_utf8_str_bytes(suffix);
    uint32_t bytes_offset = bytes_str - bytes_suffix;
    
    if (bytes_str < bytes_suffix)
        return false;
    
    for (uint32_t idx = 0; idx < bytes_suffix; idx++)
        if (str[bytes_offset + idx] != suffix[idx])
            return false;
    
    return true;
}

/*****************************************************************************/
/* utf-8 unicode tools<e>:************************************************** */
/*****************************************************************************/

