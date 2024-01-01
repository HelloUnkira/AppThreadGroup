/*实现目标:
 *    基础图元描述
 *    颜色,透明度,像素,画布
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

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
    
    return (scui_alpha_t)app_sys_map((uint16_t)percent, 0, 100, 0x00, 0xFF);
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
    //R=(R1 x A1+R2 x A2 x(1-A1))/(A1+A2 x(1-A1))
    //G=(G1 x A1+G2 x A2 x(1-A1))/(A1+A2 x(1-Al))
    //B=(B1 x A1+B2 x A2 x(1-A1))/(A1+A2 x(1-Al))
    uint16_t blend_sum = 0xFF - (0xFF - alpha_fg) * (0xFF - alpha_bg);
    uint16_t blend_bg = (uint16_t)alpha_bg * (0xFF - alpha_fg);
    uint16_t blend_fg = (uint16_t)alpha_fg;
    
    return (SCUI_PIXEL_TYPE){
        .ch.r = (uint8_t)((pixel_fg->ch.r * blend_fg + pixel_bg->ch.r * blend_bg) / blend_sum),
        .ch.g = (uint8_t)((pixel_fg->ch.g * blend_fg + pixel_bg->ch.g * blend_bg) / blend_sum),
        .ch.b = (uint8_t)((pixel_fg->ch.b * blend_fg + pixel_bg->ch.b * blend_bg) / blend_sum),
    };
}
