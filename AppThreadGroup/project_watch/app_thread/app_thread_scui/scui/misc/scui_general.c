/*实现目标:
 *    基础图元描述
 *    颜色,透明度,像素,画布
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 调色板生成颜色值
 *@param color   颜色值
 *@param palette 调色板
 *@param type    调色板类型(0:主调色板;1:亮调色板;2:暗调色板)
 *@param level   亮度等级(0~5)
 */
void scui_color_by_palette(scui_color_t *color, scui_palette_t palette, uint8_t type, uint8_t level)
{
    #define SCUI_COLOR32_PALETTE(r8, g8, b8)   {.ch.a = 0xFF, .ch.r = r8, .ch.g = g8, .ch.b = b8,}
    
    switch (type) {
    default:
    case 0: {
        static const scui_color32_t color32_map[] = {
            SCUI_COLOR32_PALETTE(0xF4, 0x43, 0x36), SCUI_COLOR32_PALETTE(0xE9, 0x1E, 0x63), SCUI_COLOR32_PALETTE(0x9C, 0x27, 0xB0), SCUI_COLOR32_PALETTE(0x67, 0x3A, 0xB7),
            SCUI_COLOR32_PALETTE(0x3F, 0x51, 0xB5), SCUI_COLOR32_PALETTE(0x21, 0x96, 0xF3), SCUI_COLOR32_PALETTE(0x03, 0xA9, 0xF4), SCUI_COLOR32_PALETTE(0x00, 0xBC, 0xD4),
            SCUI_COLOR32_PALETTE(0x00, 0x96, 0x88), SCUI_COLOR32_PALETTE(0x4C, 0xAF, 0x50), SCUI_COLOR32_PALETTE(0x8B, 0xC3, 0x4A), SCUI_COLOR32_PALETTE(0xCD, 0xDC, 0x39),
            SCUI_COLOR32_PALETTE(0xFF, 0xEB, 0x3B), SCUI_COLOR32_PALETTE(0xFF, 0xC1, 0x07), SCUI_COLOR32_PALETTE(0xFF, 0x98, 0x00), SCUI_COLOR32_PALETTE(0xFF, 0x57, 0x22),
            SCUI_COLOR32_PALETTE(0x79, 0x55, 0x48), SCUI_COLOR32_PALETTE(0x60, 0x7D, 0x8B), SCUI_COLOR32_PALETTE(0x9E, 0x9E, 0x9E)
        };
        color->color = (palette > SCUI_PALETTE_LAST) ? SCUI_COLOR32_BLACK : color32_map[palette];
        break;
    }
    case 1: {
        static const scui_color32_t color32_map[][5] = {
            {SCUI_COLOR32_PALETTE(0xEF, 0x53, 0x50), SCUI_COLOR32_PALETTE(0xE5, 0x73, 0x73), SCUI_COLOR32_PALETTE(0xEF, 0x9A, 0x9A), SCUI_COLOR32_PALETTE(0xFF, 0xCD, 0xD2), SCUI_COLOR32_PALETTE(0xFF, 0xEB, 0xEE)},
            {SCUI_COLOR32_PALETTE(0xEC, 0x40, 0x7A), SCUI_COLOR32_PALETTE(0xF0, 0x62, 0x92), SCUI_COLOR32_PALETTE(0xF4, 0x8F, 0xB1), SCUI_COLOR32_PALETTE(0xF8, 0xBB, 0xD0), SCUI_COLOR32_PALETTE(0xFC, 0xE4, 0xEC)},
            {SCUI_COLOR32_PALETTE(0xAB, 0x47, 0xBC), SCUI_COLOR32_PALETTE(0xBA, 0x68, 0xC8), SCUI_COLOR32_PALETTE(0xCE, 0x93, 0xD8), SCUI_COLOR32_PALETTE(0xE1, 0xBE, 0xE7), SCUI_COLOR32_PALETTE(0xF3, 0xE5, 0xF5)},
            {SCUI_COLOR32_PALETTE(0x7E, 0x57, 0xC2), SCUI_COLOR32_PALETTE(0x95, 0x75, 0xCD), SCUI_COLOR32_PALETTE(0xB3, 0x9D, 0xDB), SCUI_COLOR32_PALETTE(0xD1, 0xC4, 0xE9), SCUI_COLOR32_PALETTE(0xED, 0xE7, 0xF6)},
            {SCUI_COLOR32_PALETTE(0x5C, 0x6B, 0xC0), SCUI_COLOR32_PALETTE(0x79, 0x86, 0xCB), SCUI_COLOR32_PALETTE(0x9F, 0xA8, 0xDA), SCUI_COLOR32_PALETTE(0xC5, 0xCA, 0xE9), SCUI_COLOR32_PALETTE(0xE8, 0xEA, 0xF6)},
            {SCUI_COLOR32_PALETTE(0x42, 0xA5, 0xF5), SCUI_COLOR32_PALETTE(0x64, 0xB5, 0xF6), SCUI_COLOR32_PALETTE(0x90, 0xCA, 0xF9), SCUI_COLOR32_PALETTE(0xBB, 0xDE, 0xFB), SCUI_COLOR32_PALETTE(0xE3, 0xF2, 0xFD)},
            {SCUI_COLOR32_PALETTE(0x29, 0xB6, 0xF6), SCUI_COLOR32_PALETTE(0x4F, 0xC3, 0xF7), SCUI_COLOR32_PALETTE(0x81, 0xD4, 0xFA), SCUI_COLOR32_PALETTE(0xB3, 0xE5, 0xFC), SCUI_COLOR32_PALETTE(0xE1, 0xF5, 0xFE)},
            {SCUI_COLOR32_PALETTE(0x26, 0xC6, 0xDA), SCUI_COLOR32_PALETTE(0x4D, 0xD0, 0xE1), SCUI_COLOR32_PALETTE(0x80, 0xDE, 0xEA), SCUI_COLOR32_PALETTE(0xB2, 0xEB, 0xF2), SCUI_COLOR32_PALETTE(0xE0, 0xF7, 0xFA)},
            {SCUI_COLOR32_PALETTE(0x26, 0xA6, 0x9A), SCUI_COLOR32_PALETTE(0x4D, 0xB6, 0xAC), SCUI_COLOR32_PALETTE(0x80, 0xCB, 0xC4), SCUI_COLOR32_PALETTE(0xB2, 0xDF, 0xDB), SCUI_COLOR32_PALETTE(0xE0, 0xF2, 0xF1)},
            {SCUI_COLOR32_PALETTE(0x66, 0xBB, 0x6A), SCUI_COLOR32_PALETTE(0x81, 0xC7, 0x84), SCUI_COLOR32_PALETTE(0xA5, 0xD6, 0xA7), SCUI_COLOR32_PALETTE(0xC8, 0xE6, 0xC9), SCUI_COLOR32_PALETTE(0xE8, 0xF5, 0xE9)},
            {SCUI_COLOR32_PALETTE(0x9C, 0xCC, 0x65), SCUI_COLOR32_PALETTE(0xAE, 0xD5, 0x81), SCUI_COLOR32_PALETTE(0xC5, 0xE1, 0xA5), SCUI_COLOR32_PALETTE(0xDC, 0xED, 0xC8), SCUI_COLOR32_PALETTE(0xF1, 0xF8, 0xE9)},
            {SCUI_COLOR32_PALETTE(0xD4, 0xE1, 0x57), SCUI_COLOR32_PALETTE(0xDC, 0xE7, 0x75), SCUI_COLOR32_PALETTE(0xE6, 0xEE, 0x9C), SCUI_COLOR32_PALETTE(0xF0, 0xF4, 0xC3), SCUI_COLOR32_PALETTE(0xF9, 0xFB, 0xE7)},
            {SCUI_COLOR32_PALETTE(0xFF, 0xEE, 0x58), SCUI_COLOR32_PALETTE(0xFF, 0xF1, 0x76), SCUI_COLOR32_PALETTE(0xFF, 0xF5, 0x9D), SCUI_COLOR32_PALETTE(0xFF, 0xF9, 0xC4), SCUI_COLOR32_PALETTE(0xFF, 0xFD, 0xE7)},
            {SCUI_COLOR32_PALETTE(0xFF, 0xCA, 0x28), SCUI_COLOR32_PALETTE(0xFF, 0xD5, 0x4F), SCUI_COLOR32_PALETTE(0xFF, 0xE0, 0x82), SCUI_COLOR32_PALETTE(0xFF, 0xEC, 0xB3), SCUI_COLOR32_PALETTE(0xFF, 0xF8, 0xE1)},
            {SCUI_COLOR32_PALETTE(0xFF, 0xA7, 0x26), SCUI_COLOR32_PALETTE(0xFF, 0xB7, 0x4D), SCUI_COLOR32_PALETTE(0xFF, 0xCC, 0x80), SCUI_COLOR32_PALETTE(0xFF, 0xE0, 0xB2), SCUI_COLOR32_PALETTE(0xFF, 0xF3, 0xE0)},
            {SCUI_COLOR32_PALETTE(0xFF, 0x70, 0x43), SCUI_COLOR32_PALETTE(0xFF, 0x8A, 0x65), SCUI_COLOR32_PALETTE(0xFF, 0xAB, 0x91), SCUI_COLOR32_PALETTE(0xFF, 0xCC, 0xBC), SCUI_COLOR32_PALETTE(0xFB, 0xE9, 0xE7)},
            {SCUI_COLOR32_PALETTE(0x8D, 0x6E, 0x63), SCUI_COLOR32_PALETTE(0xA1, 0x88, 0x7F), SCUI_COLOR32_PALETTE(0xBC, 0xAA, 0xA4), SCUI_COLOR32_PALETTE(0xD7, 0xCC, 0xC8), SCUI_COLOR32_PALETTE(0xEF, 0xEB, 0xE9)},
            {SCUI_COLOR32_PALETTE(0x78, 0x90, 0x9C), SCUI_COLOR32_PALETTE(0x90, 0xA4, 0xAE), SCUI_COLOR32_PALETTE(0xB0, 0xBE, 0xC5), SCUI_COLOR32_PALETTE(0xCF, 0xD8, 0xDC), SCUI_COLOR32_PALETTE(0xEC, 0xEF, 0xF1)},
            {SCUI_COLOR32_PALETTE(0xBD, 0xBD, 0xBD), SCUI_COLOR32_PALETTE(0xE0, 0xE0, 0xE0), SCUI_COLOR32_PALETTE(0xEE, 0xEE, 0xEE), SCUI_COLOR32_PALETTE(0xF5, 0xF5, 0xF5), SCUI_COLOR32_PALETTE(0xFA, 0xFA, 0xFA)},
        };
        color->color_s = (palette > SCUI_PALETTE_LAST || level > 5) ? SCUI_COLOR32_BLACK : color32_map[palette][level];
        break;
    }
    case 2: {
        static const scui_color32_t color32_map[][4] = {
            {SCUI_COLOR32_PALETTE(0xE5, 0x39, 0x35), SCUI_COLOR32_PALETTE(0xD3, 0x2F, 0x2F), SCUI_COLOR32_PALETTE(0xC6, 0x28, 0x28), SCUI_COLOR32_PALETTE(0xB7, 0x1C, 0x1C)},
            {SCUI_COLOR32_PALETTE(0xD8, 0x1B, 0x60), SCUI_COLOR32_PALETTE(0xC2, 0x18, 0x5B), SCUI_COLOR32_PALETTE(0xAD, 0x14, 0x57), SCUI_COLOR32_PALETTE(0x88, 0x0E, 0x4F)},
            {SCUI_COLOR32_PALETTE(0x8E, 0x24, 0xAA), SCUI_COLOR32_PALETTE(0x7B, 0x1F, 0xA2), SCUI_COLOR32_PALETTE(0x6A, 0x1B, 0x9A), SCUI_COLOR32_PALETTE(0x4A, 0x14, 0x8C)},
            {SCUI_COLOR32_PALETTE(0x5E, 0x35, 0xB1), SCUI_COLOR32_PALETTE(0x51, 0x2D, 0xA8), SCUI_COLOR32_PALETTE(0x45, 0x27, 0xA0), SCUI_COLOR32_PALETTE(0x31, 0x1B, 0x92)},
            {SCUI_COLOR32_PALETTE(0x39, 0x49, 0xAB), SCUI_COLOR32_PALETTE(0x30, 0x3F, 0x9F), SCUI_COLOR32_PALETTE(0x28, 0x35, 0x93), SCUI_COLOR32_PALETTE(0x1A, 0x23, 0x7E)},
            {SCUI_COLOR32_PALETTE(0x1E, 0x88, 0xE5), SCUI_COLOR32_PALETTE(0x19, 0x76, 0xD2), SCUI_COLOR32_PALETTE(0x15, 0x65, 0xC0), SCUI_COLOR32_PALETTE(0x0D, 0x47, 0xA1)},
            {SCUI_COLOR32_PALETTE(0x03, 0x9B, 0xE5), SCUI_COLOR32_PALETTE(0x02, 0x88, 0xD1), SCUI_COLOR32_PALETTE(0x02, 0x77, 0xBD), SCUI_COLOR32_PALETTE(0x01, 0x57, 0x9B)},
            {SCUI_COLOR32_PALETTE(0x00, 0xAC, 0xC1), SCUI_COLOR32_PALETTE(0x00, 0x97, 0xA7), SCUI_COLOR32_PALETTE(0x00, 0x83, 0x8F), SCUI_COLOR32_PALETTE(0x00, 0x60, 0x64)},
            {SCUI_COLOR32_PALETTE(0x00, 0x89, 0x7B), SCUI_COLOR32_PALETTE(0x00, 0x79, 0x6B), SCUI_COLOR32_PALETTE(0x00, 0x69, 0x5C), SCUI_COLOR32_PALETTE(0x00, 0x4D, 0x40)},
            {SCUI_COLOR32_PALETTE(0x43, 0xA0, 0x47), SCUI_COLOR32_PALETTE(0x38, 0x8E, 0x3C), SCUI_COLOR32_PALETTE(0x2E, 0x7D, 0x32), SCUI_COLOR32_PALETTE(0x1B, 0x5E, 0x20)},
            {SCUI_COLOR32_PALETTE(0x7C, 0xB3, 0x42), SCUI_COLOR32_PALETTE(0x68, 0x9F, 0x38), SCUI_COLOR32_PALETTE(0x55, 0x8B, 0x2F), SCUI_COLOR32_PALETTE(0x33, 0x69, 0x1E)},
            {SCUI_COLOR32_PALETTE(0xC0, 0xCA, 0x33), SCUI_COLOR32_PALETTE(0xAF, 0xB4, 0x2B), SCUI_COLOR32_PALETTE(0x9E, 0x9D, 0x24), SCUI_COLOR32_PALETTE(0x82, 0x77, 0x17)},
            {SCUI_COLOR32_PALETTE(0xFD, 0xD8, 0x35), SCUI_COLOR32_PALETTE(0xFB, 0xC0, 0x2D), SCUI_COLOR32_PALETTE(0xF9, 0xA8, 0x25), SCUI_COLOR32_PALETTE(0xF5, 0x7F, 0x17)},
            {SCUI_COLOR32_PALETTE(0xFF, 0xB3, 0x00), SCUI_COLOR32_PALETTE(0xFF, 0xA0, 0x00), SCUI_COLOR32_PALETTE(0xFF, 0x8F, 0x00), SCUI_COLOR32_PALETTE(0xFF, 0x6F, 0x00)},
            {SCUI_COLOR32_PALETTE(0xFB, 0x8C, 0x00), SCUI_COLOR32_PALETTE(0xF5, 0x7C, 0x00), SCUI_COLOR32_PALETTE(0xEF, 0x6C, 0x00), SCUI_COLOR32_PALETTE(0xE6, 0x51, 0x00)},
            {SCUI_COLOR32_PALETTE(0xF4, 0x51, 0x1E), SCUI_COLOR32_PALETTE(0xE6, 0x4A, 0x19), SCUI_COLOR32_PALETTE(0xD8, 0x43, 0x15), SCUI_COLOR32_PALETTE(0xBF, 0x36, 0x0C)},
            {SCUI_COLOR32_PALETTE(0x6D, 0x4C, 0x41), SCUI_COLOR32_PALETTE(0x5D, 0x40, 0x37), SCUI_COLOR32_PALETTE(0x4E, 0x34, 0x2E), SCUI_COLOR32_PALETTE(0x3E, 0x27, 0x23)},
            {SCUI_COLOR32_PALETTE(0x54, 0x6E, 0x7A), SCUI_COLOR32_PALETTE(0x45, 0x5A, 0x64), SCUI_COLOR32_PALETTE(0x37, 0x47, 0x4F), SCUI_COLOR32_PALETTE(0x26, 0x32, 0x38)},
            {SCUI_COLOR32_PALETTE(0x75, 0x75, 0x75), SCUI_COLOR32_PALETTE(0x61, 0x61, 0x61), SCUI_COLOR32_PALETTE(0x42, 0x42, 0x42), SCUI_COLOR32_PALETTE(0x21, 0x21, 0x21)},
        };
        color->color_e = (palette > SCUI_PALETTE_LAST || level > 4) ? SCUI_COLOR32_BLACK : color32_map[palette][level];
        break;
    }
    }
}

/*@brief 像素点配置
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param tar   像素点(目标值)
 */
void scui_pixel_by_cf(scui_pixel_cf_t cf, void *pixel, void *tar)
{
    switch (cf) {
    case scui_pixel_cf_bmp565: {
        scui_color565_t *c0 = pixel;
        scui_color565_t *c1 = tar;
        c0->full = c1->full;
        break;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *c0 = pixel;
        scui_color8565_t *c1 = tar;
        for (uint8_t idx = 0; idx < 3; idx++)
        c0->byte[idx] = c1->byte[idx];
        break;
    }
    case scui_pixel_cf_bmp888: {
        scui_color888_t *c0 = pixel;
        scui_color888_t *c1 = tar;
        for (uint8_t idx = 0; idx < 3; idx++)
        c0->byte[idx] = c1->byte[idx];
        break;
    }
    case scui_pixel_cf_bmp8888: {
        scui_color8888_t *c0 = pixel;
        scui_color8888_t *c1 = tar;
        c0->full = c1->full;
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
}

/*@brief 颜色值转换(仅scui_color_t类型的内部元素转换)
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param color 颜色值
 */
void scui_pixel_by_color(scui_pixel_cf_t cf, void *pixel, scui_color8888_t color)
{
    switch (cf) {
    case scui_pixel_cf_bmp565: {
        scui_color565_t *color565 = pixel;
        color565->ch.r = (uint8_t)color.ch.r >> 3;
        color565->ch.g = (uint8_t)color.ch.g >> 2;
        color565->ch.b = (uint8_t)color.ch.b >> 3;
        break;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *color8565 = pixel;
        color8565->ch.a = (uint8_t)color.ch.a,
        color8565->ch.r = (uint8_t)color.ch.r >> 3;
        color8565->ch.g = (uint8_t)color.ch.g >> 2;
        color8565->ch.b = (uint8_t)color.ch.b >> 3;
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
}

/*@brief 像素点作用透明度
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param alpha 透明度
 */
void scui_pixel_mix_alpha(scui_pixel_cf_t cf, void *pixel, scui_alpha_t alpha)
{
    switch (cf) {
    case scui_pixel_cf_bmp565: {
        scui_color565_t *pc = pixel;
        pc->ch.r = SCUI_DIV_0xFF((uint16_t)pc->ch.r * alpha);
        pc->ch.g = SCUI_DIV_0xFF((uint16_t)pc->ch.g * alpha);
        pc->ch.b = SCUI_DIV_0xFF((uint16_t)pc->ch.b * alpha);
        break;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *pc = pixel;
        pc->ch.a = SCUI_DIV_0xFF((uint16_t)pc->ch.a * alpha);
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
}

/*@brief 像素点融合(同步作用透明度)
 *@param cf_1    像素点格式
 *@param pixel_1 像素点
 *@param alpha_1 透明度
 *@param cf_2    像素点格式
 *@param pixel_2 像素点
 *@param alpha_2 透明度
 */
void scui_pixel_mix_with(scui_pixel_cf_t cf_1, void *pixel_1, scui_alpha_t alpha_1,
                         scui_pixel_cf_t cf_2, void *pixel_2, scui_alpha_t alpha_2)
{
    /* 像素点融合主要处理俩个像素点在一个新像素点中的浓度 */
    /* 所以说此时的透明度可以等价理解为该原始像素点的浓度 */
    /* 这俩个像素点为融合后的像素点稳定的贡献全部的颜色 */
    SCUI_ASSERT(alpha_1 + alpha_2 == 0xFF);
    
    if (cf_1 == scui_pixel_cf_bmp565 &&
        cf_2 == scui_pixel_cf_bmp565) {
        scui_color565_t *c_1 = pixel_1;
        scui_color565_t *c_2 = pixel_2;
        
        if (alpha_1 == 0xFF)
            return;
        if (alpha_2 == 0xFF) {
            c_1->full = c_2->full;
            return;
        }
        
        #if 1
        // 这是一种加速像素点混合的方式
        // Alpha converted from [0..255] to [0..31]
        // Converts  0000000000000000rrrrrggggggbbbbb
        //     into  00000gggggg00000rrrrr000000bbbbb
        // with mask 00000111111000001111100000011111
        // This is useful because it makes space for a parallel fixed-point multiply
        uint8_t a1 = alpha_1 >> 3; uint32_t p1 = c_1->full;
        uint8_t a2 = alpha_2 >> 3; uint32_t p2 = c_2->full;
        p1 = ((p1 | (p1 << 16)) & 0x07e0f81f);
        p2 = ((p2 | (p2 << 16)) & 0x07e0f81f);
        uint32_t ret = ((p1 * a1 + p2 * a2) >> 5) & 0x07e0f81f;
        c_1->full = ((ret >> 16) | ret);
        #else
        scui_color_wt_t a2 = alpha_2;
        scui_color_wt_t a1 = alpha_1;
        c_1->ch.r = SCUI_DIV_0xFF(c_1->ch.r * a1 + c_2->ch.r * a2);
        c_1->ch.g = SCUI_DIV_0xFF(c_1->ch.g * a1 + c_2->ch.g * a2);
        c_1->ch.b = SCUI_DIV_0xFF(c_1->ch.b * a1 + c_2->ch.b * a2);
        #endif
        
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp565 &&
        cf_2 == scui_pixel_cf_bmp8565) {
        scui_color565_t  *c_1 = pixel_1;
        scui_color8565_t *c_2 = pixel_2;
        
        if (alpha_1 == 0xFF)
            return;
        
        scui_color_wt_t a2 = scui_alpha_mix(c_2->ch.a, alpha_2);
        scui_color_wt_t a1 = 0xFF - a2;
        c_1->ch.r = SCUI_DIV_0xFF(c_1->ch.r * a1 + c_2->ch.r * a2);
        c_1->ch.g = SCUI_DIV_0xFF(c_1->ch.g * a1 + c_2->ch.g * a2);
        c_1->ch.b = SCUI_DIV_0xFF(c_1->ch.b * a1 + c_2->ch.b * a2);
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp8565 &&
        cf_2 == scui_pixel_cf_bmp565) {
        scui_color8565_t *c_1 = pixel_1;
        scui_color565_t  *c_2 = pixel_2;
        
        scui_color_wt_t a2 = alpha_2;
        scui_color_wt_t a1 = 0xFF - a2;
        
        if (alpha_1 == 0xFF || a2 == 0x00)
            return;
        
        c_1->ch.a = a2 + a1;
        c_1->ch.r = SCUI_DIV_0xFF(c_1->ch.r * a1 + c_2->ch.r * a2);
        c_1->ch.g = SCUI_DIV_0xFF(c_1->ch.g * a1 + c_2->ch.g * a2);
        c_1->ch.b = SCUI_DIV_0xFF(c_1->ch.b * a1 + c_2->ch.b * a2);
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp8565 &&
        cf_2 == scui_pixel_cf_bmp8565) {
        scui_color8565_t *c_1 = pixel_1;
        scui_color8565_t *c_2 = pixel_2;
        
        scui_color_wt_t a2 = scui_alpha_mix(c_2->ch.a, alpha_2);
        scui_color_wt_t a1 = scui_alpha_mix(c_1->ch.a, alpha_1);
        scui_color_wt_t as = scui_alpha_mix(a1, (0xFF - a2));
        
        if (alpha_1 == 0xFF || a2 == 0x00)
            return;
        
        c_1->ch.a = 0xFF - scui_alpha_mix(0xFF - a2, 0xFF - a1);
        c_1->ch.r = SCUI_DIV_0xFF(c_1->ch.r * as + c_2->ch.r * a2);
        c_1->ch.g = SCUI_DIV_0xFF(c_1->ch.g * as + c_2->ch.g * a2);
        c_1->ch.b = SCUI_DIV_0xFF(c_1->ch.b * as + c_2->ch.b * a2);
        return;
    }
    
    SCUI_ASSERT(false);
}

/*@brief 像素点生成灰度值
 *@param cf    像素点格式
 *@param pixel 像素点
 *@retval grey 灰度值
 */
uint8_t scui_grey_by_pixel(scui_pixel_cf_t cf, void *pixel)
{
    switch (cf) {
    case scui_pixel_cf_bmp565: {
        scui_color565_t *pc = pixel;
        
        uint16_t r8 = (pc->ch.r << 3) + 0b111;
        uint16_t g8 = (pc->ch.g << 2) + 0b11;
        uint16_t b8 = (pc->ch.b << 3) + 0b111;
        
        // Gray * 1024 = R * 306 + G * 601 + B * 117
        uint8_t grey = (r8 * 306 + g8 * 601 + b8 * 117) >> 10;
        return grey;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *pc = pixel;
        scui_color565_t c565 = {0};
        c565.ch.r = pc->ch.r;
        c565.ch.g = pc->ch.g;
        c565.ch.b = pc->ch.b;
        scui_pixel_mix_alpha(scui_pixel_cf_bmp565, &c565, pc->ch.a);
        
        uint16_t r8 = (c565.ch.r << 3) + 0b111;
        uint16_t g8 = (c565.ch.g << 2) + 0b11;
        uint16_t b8 = (c565.ch.b << 3) + 0b111;
        
        // Gray * 1024 = R * 306 + G * 601 + B * 117
        uint8_t grey = (r8 * 306 + g8 * 601 + b8 * 117) >> 10;
        return grey;
    }
    default:
        SCUI_ASSERT(false);
        return 0;
    }
}

/*@brief 计算灰度值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 灰度值
 */
uint8_t scui_grey_bpp_x(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x)
{
    /* 只支持1,2,4,8的bpp, 在字库加载时检查 */
    SCUI_ASSERT(bpp * bpp_x < 8);
    
    uint8_t ofs = 0;
    const uint16_t mask_1 = 0x0001;
    const uint16_t mask_2 = 0x0003;
    const uint16_t mask_4 = 0x000F;
    const uint16_t mask_8 = 0x00FF;
    
    switch (bpp) {
    /* 高位在前,地位在后 */
    case 1: ofs = 7 - bpp_x * 1;
        return ((bitmap & (mask_1 << ofs)) >> ofs) * 0xFF / mask_1;
    case 2: ofs = 6 - bpp_x * 2;
        return ((bitmap & (mask_2 << ofs)) >> ofs) * 0xFF / mask_2;
    case 4: ofs = 4 - bpp_x * 4;
        return ((bitmap & (mask_4 << ofs)) >> ofs) * 0xFF / mask_4;
    case 8: ofs = 0;    // 0 - bpp_x * 8;   // 这个应该永远都是0
        return ((bitmap & (mask_8 << ofs)) >> ofs) * 0xFF / mask_8;
    default:
        SCUI_ASSERT(false);
        return 0;
    }
}

/*@brief 透明度混合
 *@param alpha1 透明度1
 *@param alpha2 透明度2
 *@retval 混合后的透明度
 */
scui_alpha_t scui_alpha_mix(scui_alpha_t alpha1, scui_alpha_t alpha2)
{
    return SCUI_DIV_0xFF(((uint16_t)alpha1 * (uint16_t)alpha2));
}

/*@brief 透明度混合撤销
 *@param alpha1 透明度1
 *@param alpha2 透明度2
 *@retval 混合撤销后的透明度
 */
scui_alpha_t scui_alpha_undo(scui_alpha_t alpha1, scui_alpha_t alpha2)
{
    const scui_alpha_t alpha_s = scui_alpha_trans;
    const scui_alpha_t alpha_e = scui_alpha_cover;
    
    if (alpha2 == 0) return alpha1;
    return scui_clamp((uint16_t)alpha1 * alpha_e / alpha2, alpha_s, alpha_e);
}

/*@brief 计算透明度通过百分比值
 *@param pct 透明度百分比值[0, 100]
 *@retval 透明度
 */
scui_alpha_t scui_alpha_pct(uint8_t pct)
{
    const scui_alpha_t alpha_s = scui_alpha_trans;
    const scui_alpha_t alpha_e = scui_alpha_cover;
    return scui_map(pct, 0, 100, alpha_s, alpha_e);
}

/*@brief 像素点比特位数
 *@param cf 像素点格式
 *@retval 比特位数
 */
scui_coord_t scui_pixel_bits(scui_pixel_cf_t cf)
{
    return cf & scui_pixel_cf_bits_mask;
}

/*@brief 像素点是否有透明度
 *@param cf 像素点格式
 *@retval 是否有透明度
 */
bool scui_pixel_have_alpha(scui_pixel_cf_t cf)
{
    switch (cf) {
    case scui_pixel_cf_bmp565:
    case scui_pixel_cf_bmp888:
        return false;
    default:
        return true;
    }
}

/*@brief 画布有效区域
 *@param surface 画布实例
 *@retval 区域
 */
scui_area_t scui_surface_area(scui_surface_t *surface)
{
    return (scui_area_t){
        .w = surface->hor_res,
        .h = surface->ver_res,
    };
}
