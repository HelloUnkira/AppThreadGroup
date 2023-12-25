/*实现目标:
 *    基础图元描述
 *    颜色,透明度,像素,画布
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"





/*@brief 像素点作用Alpha通道值
 *@param pixel 像素点
 *@param alpha alpha值
 */
inline void scui_pixel_with_alpha(SCUI_PIXEL_TYPE *pixel, scui_alpha_t alpha)
{
    pixel->ch.r = (uint8_t)((uint16_t)pixel->ch.r * alpha / 0xFF);
    pixel->ch.g = (uint8_t)((uint16_t)pixel->ch.g * alpha / 0xFF);
    pixel->ch.b = (uint8_t)((uint16_t)pixel->ch.b * alpha / 0xFF);
}














