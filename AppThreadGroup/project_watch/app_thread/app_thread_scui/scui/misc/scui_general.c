/*实现目标:
 *    基础图元描述
 *    颜色,透明度,像素,画布
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 像素点配置
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param tar   像素点(目标值)
 */
void scui_pixel_by_cf(scui_pixel_cf_t cf, void *pixel, void *tar)
{
    switch (cf) {
    case scui_pixel_cf_bmp565:
        scui_mem_w(pixel, *(scui_color565_t  *)tar, scui_color565_t);
        break;
    case scui_pixel_cf_bmp8565:
        scui_mem_w(pixel, *(scui_color8565_t *)tar, scui_color8565_t);
        break;
    case scui_pixel_cf_bmp888:
        scui_mem_w(pixel, *(scui_color888_t  *)tar, scui_color888_t);
        break;
    case scui_pixel_cf_bmp8888:
        scui_mem_w(pixel, *(scui_color8888_t *)tar, scui_color8888_t);
        break;
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
        scui_color565_t *pixel565 = pixel;
        *pixel565 = (scui_color565_t){
            .ch.r = (uint8_t)((uint16_t)pixel565->ch.r * alpha / 0xFF),
            .ch.g = (uint8_t)((uint16_t)pixel565->ch.g * alpha / 0xFF),
            .ch.b = (uint8_t)((uint16_t)pixel565->ch.b * alpha / 0xFF),
        };
        break;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *pixel8565 = pixel;
        pixel8565->ch.a = (scui_alpha_t)((uint16_t)pixel8565->ch.a * alpha);
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
    SCUI_ASSERT(alpha_1 + alpha_2 == scui_alpha_cover);
    
    if (cf_1 == scui_pixel_cf_bmp565 &&
        cf_2 == scui_pixel_cf_bmp565) {
        
        if (alpha_1 == scui_alpha_cover)
            return;
        if (alpha_2 == scui_alpha_cover) {
            *(scui_color565_t *)pixel_1 =
            *(scui_color565_t *)pixel_2;
            return;
        }
        
        #if 1
        // 这是一种加速像素点混合的方式
        // Alpha converted from [0..255] to [0..31]
        // Converts  0000000000000000rrrrrggggggbbbbb
        //     into  00000gggggg00000rrrrr000000bbbbb
        // with mask 00000111111000001111100000011111
        // This is useful because it makes space for a parallel fixed-point multiply
        uint8_t a1 = alpha_1 >> 3;
        uint8_t a2 = alpha_2 >> 3;
        uint32_t p1 = ((scui_color565_t *)pixel_1)->full;
        uint32_t p2 = ((scui_color565_t *)pixel_2)->full;
        p1 = ((p1 | (p1 << 16)) & 0x07e0f81f);
        p2 = ((p2 | (p2 << 16)) & 0x07e0f81f);
        uint32_t ret = ((p1 * a1 + p2 * a2) >> 5) & 0x07e0f81f;
        ((scui_color565_t *)pixel_1)->full = (uint16_t)((ret >> 16) | ret);
        #endif
        
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp565 &&
        cf_2 == scui_pixel_cf_bmp8565) {
        
        scui_color565_t  *c_1 = pixel_1;
        scui_color8565_t *c_2 = pixel_2;
        scui_color_limit_t a2 = (uint16_t)c_2->ch.a * alpha_2 / 0xFF;
        scui_color_limit_t a1 = 0xFF - a2;
        scui_color_limit_t r  = ((uint16_t)c_1->ch.r * a1 + (uint16_t)c_2->ch.r * a2) / 0xFF;
        scui_color_limit_t g  = ((uint16_t)c_1->ch.g * a1 + (uint16_t)c_2->ch.g * a2) / 0xFF;
        scui_color_limit_t b  = ((uint16_t)c_1->ch.b * a1 + (uint16_t)c_2->ch.b * a2) / 0xFF;
        
        scui_color565_t color565 = {
            .ch.r = r,
            .ch.g = g,
            .ch.b = b,
        };
        *c_1 = color565;
        
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp8565 &&
        cf_2 == scui_pixel_cf_bmp565) {
        
        scui_color8565_t *c_1 = pixel_1;
        scui_color565_t  *c_2 = pixel_2;
        scui_color_limit_t a1 = (uint16_t)c_1->ch.a * alpha_1 / 0xFF;
        scui_color_limit_t a2 = 0xFF - a1;
        scui_color_limit_t as = a1 + a2 * alpha_2 / 0xFF;
        scui_color_limit_t r  = ((uint16_t)c_1->ch.r * a1 + (uint16_t)c_2->ch.r * a2) / 0xFF;
        scui_color_limit_t g  = ((uint16_t)c_1->ch.g * a1 + (uint16_t)c_2->ch.g * a2) / 0xFF;
        scui_color_limit_t b  = ((uint16_t)c_1->ch.b * a1 + (uint16_t)c_2->ch.b * a2) / 0xFF;
        
        if (alpha_1 == 0xFF)
            return;
        
        scui_color8565_t color8565 = {
            .ch.a = as,
            .ch.r = r,
            .ch.g = g,
            .ch.b = b,
        };
        *c_1 = color8565;
        
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp8565 &&
        cf_2 == scui_pixel_cf_bmp8565) {
        
        scui_color8565_t *c_1 = pixel_1;
        scui_color8565_t *c_2 = pixel_2;
        scui_color_limit_t a2 = (uint16_t)c_2->ch.a * alpha_2 / 0xFF;
        scui_color_limit_t a1 = (uint16_t)c_1->ch.a * alpha_1 / 0xFF;
        scui_color_limit_t as = (a1 * alpha_1 + a2 * alpha_2) / 0xFF;
        scui_color_limit_t r  = ((uint16_t)c_1->ch.r * a1 + (uint16_t)c_2->ch.r * a2) / 0xFF;
        scui_color_limit_t g  = ((uint16_t)c_1->ch.g * a1 + (uint16_t)c_2->ch.g * a2) / 0xFF;
        scui_color_limit_t b  = ((uint16_t)c_1->ch.b * a1 + (uint16_t)c_2->ch.b * a2) / 0xFF;
        
        if (alpha_1 == 0xFF)
            return;
        
        scui_color8565_t color8565 = {
            .ch.a = as,
            .ch.r = r,
            .ch.g = g,
            .ch.b = b,
        };
        *c_1 = color8565;
        
        return;
    }
    
    SCUI_ASSERT(false);
}

/*@brief 计算透明度通过百分比值
 *@param pct 透明度百分比值[0, 100]
 *@retval 透明度
 */
scui_alpha_t scui_alpha_pct(uint8_t pct)
{
    const scui_alpha_t alpha_s = scui_alpha_trans;
    const scui_alpha_t alpha_e = scui_alpha_cover;
    return pct >= 100 ? scui_alpha_cover : (scui_alpha_t)
           scui_map((uint16_t)pct, 0, 100, alpha_s, alpha_e);
}

/*@brief 像素点比特位数
 *@param cf 像素点格式
 *@retval 比特位数
 */
scui_coord_t scui_pixel_bits(scui_pixel_cf_t cf)
{
    return cf & scui_pixel_cf_bits_mask;
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
