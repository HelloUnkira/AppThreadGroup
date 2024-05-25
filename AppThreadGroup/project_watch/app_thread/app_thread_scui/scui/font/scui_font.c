/*实现目标:
 *    字体
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
static inline uint8_t scui_font_bpp_1_palette(uint8_t bitmap, uint8_t bpp_x)
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
static inline uint8_t scui_font_bpp_2_palette(uint8_t bitmap, uint8_t bpp_x)
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
static inline uint8_t scui_font_bpp_4_palette(uint8_t bitmap, uint8_t bpp_x)
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
static inline uint8_t scui_font_bpp_8_palette(uint8_t bitmap, uint8_t bpp_x)
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
uint8_t scui_font_bpp_palette(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x)
{
    /* 只支持1,2,4,8的bpp, 在字库加载时检查 */
    SCUI_ASSERT(bpp > bpp_x);
    
    switch (bpp) {
    /* 高位在前,地位在后 */
    case 1: return scui_font_bpp_1_palette(bitmap, bpp_x);
    case 2: return scui_font_bpp_2_palette(bitmap, bpp_x);
    case 4: return scui_font_bpp_4_palette(bitmap, bpp_x);
    case 8: return scui_font_bpp_8_palette(bitmap, bpp_x);
    default: SCUI_ASSERT(false); return 0;
    }
}
