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
    case scui_pixel_cf_bmp565: {
        scui_color565_t *c0 = pixel;
        scui_color565_t *c1 = tar;
        c0->ch.r = c1->ch.r;
        c0->ch.g = c1->ch.g;
        c0->ch.b = c1->ch.b;
        break;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *c0 = pixel;
        scui_color8565_t *c1 = tar;
        c0->ch.a = c1->ch.a;
        c0->ch.r = c1->ch.r;
        c0->ch.g = c1->ch.g;
        c0->ch.b = c1->ch.b;
        break;
    }
    case scui_pixel_cf_bmp888: {
        scui_color888_t *c0 = pixel;
        scui_color888_t *c1 = tar;
        c0->ch.r = c1->ch.r;
        c0->ch.g = c1->ch.g;
        c0->ch.b = c1->ch.b;
        break;
    }
    case scui_pixel_cf_bmp8888: {
        scui_color8888_t *c0 = pixel;
        scui_color8888_t *c1 = tar;
        c0->ch.a = c1->ch.a;
        c0->ch.r = c1->ch.r;
        c0->ch.g = c1->ch.g;
        c0->ch.b = c1->ch.b;
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
        scui_color565_t *pixel565 = pixel;
        *pixel565 = (scui_color565_t){
            .ch.r = (uint8_t)SCUI_DIV_0xFF((uint16_t)pixel565->ch.r * alpha),
            .ch.g = (uint8_t)SCUI_DIV_0xFF((uint16_t)pixel565->ch.g * alpha),
            .ch.b = (uint8_t)SCUI_DIV_0xFF((uint16_t)pixel565->ch.b * alpha),
        };
        break;
    }
    case scui_pixel_cf_bmp8565: {
        scui_color8565_t *pixel8565 = pixel;
        pixel8565->ch.a = scui_alpha_mix(pixel8565->ch.a, alpha);
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
        #else
        scui_color565_t  *c_1 = pixel_1;
        scui_color565_t  *c_2 = pixel_2;
        scui_color_wt_t a2 = alpha_2;
        scui_color_wt_t a1 = alpha_1;
        scui_color_wt_t r  = SCUI_DIV_0xFF((uint16_t)c_1->ch.r * a1 + (uint16_t)c_2->ch.r * a2);
        scui_color_wt_t g  = SCUI_DIV_0xFF((uint16_t)c_1->ch.g * a1 + (uint16_t)c_2->ch.g * a2);
        scui_color_wt_t b  = SCUI_DIV_0xFF((uint16_t)c_1->ch.b * a1 + (uint16_t)c_2->ch.b * a2);
        
        if (alpha_1 == 0xFF)
            return;
        
        scui_color565_t color565 = {
            .ch.r = r,
            .ch.g = g,
            .ch.b = b,
        };
        *c_1 = color565;
        #endif
        
        return;
    }
    
    if (cf_1 == scui_pixel_cf_bmp565 &&
        cf_2 == scui_pixel_cf_bmp8565) {
        
        scui_color565_t  *c_1 = pixel_1;
        scui_color8565_t *c_2 = pixel_2;
        scui_color_wt_t a2 = scui_alpha_mix(c_2->ch.a, alpha_2);
        scui_color_wt_t a1 = 0xFF - a2;
        scui_color_wt_t r  = SCUI_DIV_0xFF((uint16_t)c_1->ch.r * a1 + (uint16_t)c_2->ch.r * a2);
        scui_color_wt_t g  = SCUI_DIV_0xFF((uint16_t)c_1->ch.g * a1 + (uint16_t)c_2->ch.g * a2);
        scui_color_wt_t b  = SCUI_DIV_0xFF((uint16_t)c_1->ch.b * a1 + (uint16_t)c_2->ch.b * a2);
        
        if (alpha_1 == 0xFF)
            return;
        
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
        scui_color_wt_t a2 = alpha_2;
        scui_color_wt_t a1 = 0xFF - a2;
        scui_color_wt_t as = a2 + a1;
        scui_color_wt_t r  = SCUI_DIV_0xFF((uint16_t)c_1->ch.r * a1 + (uint16_t)c_2->ch.r * a2);
        scui_color_wt_t g  = SCUI_DIV_0xFF((uint16_t)c_1->ch.g * a1 + (uint16_t)c_2->ch.g * a2);
        scui_color_wt_t b  = SCUI_DIV_0xFF((uint16_t)c_1->ch.b * a1 + (uint16_t)c_2->ch.b * a2);
        
        if (alpha_1 == 0xFF || a2 == 0x00)
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
        scui_color_wt_t a2 = scui_alpha_mix(c_2->ch.a, alpha_2);
        scui_color_wt_t a1 = scui_alpha_mix(c_1->ch.a, alpha_1);
        
        scui_color_wt_t as = 0xFF - scui_alpha_mix(0xFF - a2, 0xFF - a1);
        scui_color_wt_t r  = SCUI_DIV_0xFF((uint32_t)c_1->ch.r * a1 * (0xFF - a2) / 0xFF + (uint16_t)c_2->ch.r * a2);
        scui_color_wt_t g  = SCUI_DIV_0xFF((uint32_t)c_1->ch.g * a1 * (0xFF - a2) / 0xFF + (uint16_t)c_2->ch.g * a2);
        scui_color_wt_t b  = SCUI_DIV_0xFF((uint32_t)c_1->ch.b * a1 * (0xFF - a2) / 0xFF + (uint16_t)c_2->ch.b * a2);
        
        //去黑
        if (alpha_1 == 0xFF || a2 == 0x00)
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

/*@brief 像素点生成调色板值
 *@param cf      像素点格式
 *@param pixel   像素点
 *@param palette 调色板值
 */
void scui_palette_by_pixel(scui_pixel_cf_t cf, void *pixel, uint8_t *palette)
{
    if (cf == scui_pixel_cf_bmp565) {
        scui_color565_t *c565 = pixel;
        
        uint16_t r8 = c565->ch.r << 3;
        uint16_t g8 = c565->ch.g << 2;
        uint16_t b8 = c565->ch.b << 3;
        
        // Gray * 1024 = R * 306 + G * 601 + B * 117
        *palette = (r8 * 306 + g8 * 601 + b8 * 117) >> 10;
        return;
    }
    
    if (cf == scui_pixel_cf_bmp8565) {
        scui_color8565_t *c8565 = pixel;
        scui_color565_t c565 = {0};
        c565.ch.r = c8565->ch.r;
        c565.ch.g = c8565->ch.g;
        c565.ch.b = c8565->ch.b;
        scui_pixel_mix_alpha(scui_pixel_cf_bmp565, &c565, c8565->ch.a);
        
        uint16_t r8 = (c565.ch.r << 3) + 0b111;
        uint16_t g8 = (c565.ch.g << 2) + 0b11;
        uint16_t b8 = (c565.ch.b << 3) + 0b111;
        
        // Gray * 1024 = R * 306 + G * 601 + B * 117
        *palette = (r8 * 306 + g8 * 601 + b8 * 117) >> 10;
        return;
    }
    
    SCUI_ASSERT(false);
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
    case 1:
        ofs = 7 - bpp_x * 1;
        return ((bitmap & (mask_1 << ofs)) >> ofs) * 0xFF / mask_1;
    case 2:
        ofs = 6 - bpp_x * 2;
        return ((bitmap & (mask_2 << ofs)) >> ofs) * 0xFF / mask_2;
    case 4:
        ofs = 4 - bpp_x * 4;
        return ((bitmap & (mask_4 << ofs)) >> ofs) * 0xFF / mask_4;
    case 8:
        ofs = 0;    // 0 - bpp_x * 8;   // 这个应该永远都是0
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
    return ((uint16_t)alpha1 * scui_alpha_cover / (uint16_t)alpha2);
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
