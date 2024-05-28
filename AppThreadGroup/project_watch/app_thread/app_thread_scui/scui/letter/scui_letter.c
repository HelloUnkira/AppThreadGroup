/*实现目标:
 *    字符
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
static inline uint8_t scui_letter_bpp_1_palette(uint8_t bitmap, uint8_t bpp_x)
{
    uint8_t ofs = 7 - bpp_x * 1;
    const uint16_t mask = 0x0001;
    return ((bitmap & (mask << ofs)) >> ofs) * scui_alpha_cover / mask;
}

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
static inline uint8_t scui_letter_bpp_2_palette(uint8_t bitmap, uint8_t bpp_x)
{
    uint8_t ofs = 6 - bpp_x * 2;
    const uint16_t mask = 0x0003;
    return ((bitmap & (mask << ofs)) >> ofs) * scui_alpha_cover / mask;
}

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
static inline uint8_t scui_letter_bpp_4_palette(uint8_t bitmap, uint8_t bpp_x)
{
    uint8_t ofs = 4 - bpp_x * 4;
    const uint16_t mask = 0x000F;
    return ((bitmap & (mask << ofs)) >> ofs) * scui_alpha_cover / mask;
}

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
static inline uint8_t scui_letter_bpp_8_palette(uint8_t bitmap, uint8_t bpp_x)
{
    uint8_t ofs = 0;    // 0 - bpp_x * 8;   // 这个应该永远都是0
    const uint16_t mask = 0x00FF;
    return ((bitmap & (mask << ofs)) >> ofs) * scui_alpha_cover / mask;
}

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
uint8_t scui_letter_bpp_palette(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x)
{
    /* 只支持1,2,4,8的bpp, 在字库加载时检查 */
    SCUI_ASSERT(bpp * bpp_x < 8);
    
    switch (bpp) {
    /* 高位在前,地位在后 */
    case 1: return scui_letter_bpp_1_palette(bitmap, bpp_x);
    case 2: return scui_letter_bpp_2_palette(bitmap, bpp_x);
    case 4: return scui_letter_bpp_4_palette(bitmap, bpp_x);
    case 8: return scui_letter_bpp_8_palette(bitmap, bpp_x);
    default: SCUI_ASSERT(false); return 0;
    }
}

/*****************************************************************************/
/* utf-8 tools<part>:******************************************************* */
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

/*@brief 后缀匹配
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
/* unicode tools<part>:***************************************************** */
/*****************************************************************************/

/*@brief utf8字符串构造生成unicode字符串
 *@param unicode unicode字符串
 *@param utf8    utf8字符串
 *@retval 字符数量
 */
uint32_t scui_unicode_str_by_utf8(uint32_t **unicode, uint8_t *utf8)
{
    /* 统计字符数量 */
    uint32_t utf8_num = scui_utf8_str_num(utf8);
    *unicode = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (utf8_num + 1));
    memset(*unicode, 0, 4 * (utf8_num + 1));
    
    uint8_t *utf8_ptr = utf8;
    for (uint32_t idx = 0; idx < utf8_num; idx++) {
         uint8_t utf8_len = scui_utf8_to_unicode(utf8_ptr, &(*unicode)[idx]);
         SCUI_ASSERT(utf8_len != 0);
         utf8_ptr += utf8_len;
    }
    
    return utf8_num;
}



/*****************************************************************************/
/* tools end:*************************************************************** */
/*****************************************************************************/
