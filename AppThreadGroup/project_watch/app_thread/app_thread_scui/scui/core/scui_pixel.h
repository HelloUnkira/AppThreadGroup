#ifndef SCUI_PIXEL_H
#define SCUI_PIXEL_H

/*@brief: 设备像素点格式:
 *        设备像素点格式是直达显示器的FrameBuffer或Canvas
 *        所以无需额外携带Alpha通道,因为在渲染的过程中
 *        Alpha通道会作用到颜色值上去
 */
typedef enum {
    scui_pixel_format_rgb565,
    scui_pixel_format_rgb888,
} scui_pixel_format_type_t;

/*@brief: 字节配置,元素提取,像素点逆序(慎用)
 *        最上层使用的color统一为ARGB8888类型
 *        而设备大多时候使用RGB565格式
 *        所以到设备需要有一个通用转换
 */
#if 0
#elif SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565
#define SCUI_PIXEL_TYPE         scui_color565_t
#define SCUI_PIXEL_SIZE         sizeof(scui_color565_t)
#define SCUI_PIXEL_R(color)     (((color) & 0xF800) >> 8)   // max-255
#define SCUI_PIXEL_G(color)     (((color) >> 3) & 0x00FC)   // max-255
#define SCUI_PIXEL_B(color)     (((color) << 3) & 0x00F8)   // max-255
#define SCUI_PIXEL_REV(pixel)   ((((pixel) & 0xFF) << 8) & (((pixel) & 0xFF00) >> 8))
#elif SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888
#define SCUI_PIXEL_TYPE         scui_color888_t
#define SCUI_PIXEL_SIZE         sizeof(scui_color888_t)
#define SCUI_PIXEL_R(color)     (((color) >> 16) & 0xFF)    // max-255
#define SCUI_PIXEL_G(color)     (((color) >>  8) & 0xFF)    // max-255
#define SCUI_PIXEL_B(color)     (((color) >>  0) & 0xFF)    // max-255
#define SCUI_PIXEL_REV(pixel)   ((((pixel) & 0xFF) << 16) & (((pixel) & 0x00FF00)) & (((pixel) & 0xFF0000) >> 16))
#else
#error "pixel format unknown"
#endif

/*@brief 像素点作用Alpha通道值(慎用)
 *       影响性能不建议使用
 */
static inline void scui_pixel_with_alpha(SCUI_PIXEL_TYPE *pixel, uint8_t alpha)
{
    pixel->ch.r = (uint8_t)((uint16_t)pixel->ch.r * alpha / 0xFF);
    pixel->ch.g = (uint8_t)((uint16_t)pixel->ch.g * alpha / 0xFF);
    pixel->ch.b = (uint8_t)((uint16_t)pixel->ch.b * alpha / 0xFF);
}

#endif
