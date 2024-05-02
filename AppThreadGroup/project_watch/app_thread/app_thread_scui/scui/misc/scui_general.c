/*实现目标:
 *    基础图元描述
 *    颜色,透明度,像素,画布
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 画布有效区域
 *@retval 区域
 */
scui_area_t scui_surface_area(scui_surface_t *surface)
{
    return (scui_area_t){
        .w = surface->hor_res,
        .h = surface->ver_res,
    };
}

/*@brief 颜色值格式转换
 *@param color888 颜色值
 *@retval 颜色值
 */
scui_color565_t scui_color_rgb888_to_rgb565(scui_color888_t color888)
{
    scui_color565_t color565 = {
        .ch.r = (uint8_t)color888.ch.r >> 3,
        .ch.g = (uint8_t)color888.ch.g >> 2,
        .ch.b = (uint8_t)color888.ch.b >> 3,
    };
    return color565;
}

/*@brief 颜色值格式转换(字节逆序)
 *@param color888 颜色值
 *@retval 颜色值
 */
scui_color565_t scui_color_rgb888_to_rgb565_rev(scui_color888_t color888)
{
    scui_color565_t color565 = {
        .ch.r = (uint8_t)color888.ch.r >> 3,
        .ch.g = (uint8_t)color888.ch.g >> 2,
        .ch.b = (uint8_t)color888.ch.b >> 3,
    };
    uint8_t byte_0 = color565.byte[0];
    uint8_t byte_1 = color565.byte[1];
    color565.byte[0] = byte_1;
    color565.byte[1] = byte_0;
    return color565;
}

/*@brief 颜色值格式转换
 *@param color565 颜色值
 *@retval 颜色值
 */
scui_color888_t scui_color_rgb565_to_rgb888(scui_color565_t color565)
{
    scui_color888_t color888 = {
        .ch.r = (uint8_t)color565.ch.r << 3,
        .ch.g = (uint8_t)color565.ch.g << 2,
        .ch.b = (uint8_t)color565.ch.b << 3,
    };
    return color888;
}

/*@brief 颜色值格式转换(字节逆序)
 *@param color565 颜色值
 *@retval 颜色值
 */
scui_color888_t scui_color_rgb565_to_rgb888_rev(scui_color565_t color565)
{
    scui_color888_t color888 = {
        .ch.r = (uint8_t)color565.ch.r << 3,
        .ch.g = (uint8_t)color565.ch.g << 2,
        .ch.b = (uint8_t)color565.ch.b << 3,
    };
    uint8_t byte_0 = color888.byte[0];
    uint8_t byte_2 = color888.byte[2];
    color888.byte[0] = byte_2;
    color888.byte[2] = byte_0;
    return color888;
}

/*@brief 计算透明度通过百分比值
 *@param percent 透明度百分比值[0, 100]
 *@retval 透明度
 */
scui_alpha_t scui_alpha_by_percent(uint8_t percent)
{
    if (percent >= 100)
        return 0xFF;
    
    return (scui_alpha_t)scui_map((uint16_t)percent, 0, 100, 0x00, 0xFF);
}

/*@brief 上层颜色值转为设备颜色值
 *       只用于scui_color_gradient_t类型的内部元素转换使用
 *@param color 颜色值
 *@retval 颜色值
 */
SCUI_PIXEL_TYPE scui_pixel_by_color(scui_color8888_t color)
{
    #if 0
    #elif SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565
    return (SCUI_PIXEL_TYPE){
        .ch.r = (uint8_t)color.ch.r >> 3,
        .ch.g = (uint8_t)color.ch.g >> 2,
        .ch.b = (uint8_t)color.ch.b >> 3,
    };
    #elif SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888
    return (SCUI_PIXEL_TYPE){
        .ch.r = (uint8_t)color.ch.r,
        .ch.g = (uint8_t)color.ch.g,
        .ch.b = (uint8_t)color.ch.b,
    };
    #else
    #endif
}

/*@brief 像素点作用透明度
 *@param pixel 像素点
 *@param alpha 透明度
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_with_alpha(SCUI_PIXEL_TYPE *pixel, scui_alpha_t alpha)
{
    return (SCUI_PIXEL_TYPE){
        .ch.r = (uint8_t)((uint16_t)pixel->ch.r * alpha / 0xFF),
        .ch.g = (uint8_t)((uint16_t)pixel->ch.g * alpha / 0xFF),
        .ch.b = (uint8_t)((uint16_t)pixel->ch.b * alpha / 0xFF),
    };
}

/*@brief 像素点融合(同步作用透明度)
 *@param pixel_1 像素点
 *@param alpha_1 透明度
 *@param pixel_2 像素点
 *@param alpha_2 透明度
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_mix_with_alpha(SCUI_PIXEL_TYPE *pixel_1, scui_alpha_t alpha_1,
                                          SCUI_PIXEL_TYPE *pixel_2, scui_alpha_t alpha_2)
{
    /* 像素点融合主要处理俩个像素点在一个新像素点中的浓度 */
    /* 所以说此时的透明度可以等价理解为该原始像素点的浓度 */
    /* 这俩个像素点为融合后的像素点稳定的贡献全部的颜色 */
    SCUI_ASSERT(alpha_1 + alpha_2 == scui_alpha_cover);
    
    #if SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply
    uint8_t a1 = alpha_1 >> 3;
    uint8_t a2 = alpha_2 >> 3;
    uint32_t p1 = pixel_1->full;
    uint32_t p2 = pixel_2->full;
    p1 = ((p1 | (p1 << 16)) & 0x07e0f81f);
    p2 = ((p2 | (p2 << 16)) & 0x07e0f81f);
    uint32_t ret = ((p1 * a1 + p2 * a2) >> 5) & 0x07e0f81f;
    return (SCUI_PIXEL_TYPE){
        .full = (uint16_t)((ret >> 16) | ret),
    };
    #endif
    
    return (SCUI_PIXEL_TYPE){
        .ch.r = ((uint16_t)pixel_1->ch.r * alpha_1 + (uint16_t)pixel_2->ch.r * alpha_2) / 0xFF,
        .ch.g = ((uint16_t)pixel_1->ch.g * alpha_1 + (uint16_t)pixel_2->ch.g * alpha_2) / 0xFF,
        .ch.b = ((uint16_t)pixel_1->ch.b * alpha_1 + (uint16_t)pixel_2->ch.b * alpha_2) / 0xFF,
    };
}

/*@brief 像素点混合(同步作用透明度)
 *@param pixel_fg 像素点(fg)
 *@param alpha_fg 透明度(fg)
 *@param pixel_bg 像素点(bg)
 *@param alpha_bg 透明度(bg)
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_blend_with_alpha(SCUI_PIXEL_TYPE *pixel_fg, scui_alpha_t alpha_fg,
                                            SCUI_PIXEL_TYPE *pixel_bg, scui_alpha_t alpha_bg)
{
    /* 像素点混合主要处理俩个像素点在一个新像素点中的权重 */
    /* 所以说此时的透明度可以等价理解为该原始像素点的权重 */
    /* 前景色像素点所占权重越多背景色像素点占用则越少 */
    /* 同样的透明度权重总和越小混合出的色调越暗淡 */
    /* 它一般用于中间过程的像素点使用而不是最后 */
    
    //R=(R1 x A1+R2 x A2 x(1-A1))/(A1+A2 x(1-A1))
    //G=(G1 x A1+G2 x A2 x(1-A1))/(A1+A2 x(1-Al))
    //B=(B1 x A1+B2 x A2 x(1-A1))/(A1+A2 x(1-Al))
    uint16_t blend_sum = 0xFF - (0xFF - alpha_fg) * (0xFF - alpha_bg);
    uint16_t blend_bg = (uint16_t)alpha_bg * (0xFF - alpha_fg);
    uint16_t blend_fg = (uint16_t)alpha_fg;
    
    #if SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply
    uint8_t fg_a = blend_fg >> 3;
    uint8_t bg_a = blend_bg >> 3;
    uint32_t fg = pixel_fg->full;
    uint32_t bg = pixel_bg->full;
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    uint32_t ret = ((fg * fg_a + bg * bg_a) >> 5) & 0x07e0f81f;
    return (SCUI_PIXEL_TYPE){
        .full = (uint16_t)((ret >> 16) | ret),
    };
    #endif
    
    return (SCUI_PIXEL_TYPE){
        .ch.r = (uint8_t)((pixel_fg->ch.r * blend_fg + pixel_bg->ch.r * blend_bg) / blend_sum),
        .ch.g = (uint8_t)((pixel_fg->ch.g * blend_fg + pixel_bg->ch.g * blend_bg) / blend_sum),
        .ch.b = (uint8_t)((pixel_fg->ch.b * blend_fg + pixel_bg->ch.b * blend_bg) / blend_sum),
    };
}
