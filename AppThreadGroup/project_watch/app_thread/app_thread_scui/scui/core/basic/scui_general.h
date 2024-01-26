#ifndef SCUI_GENERAL_H
#define SCUI_GENERAL_H

/* RGB565: */
#pragma pack(1)
typedef union {
    struct {
        uint16_t b:5;
        uint16_t g:6;
        uint16_t r:5;
    } ch;
    uint8_t  byte[2];
    uint16_t full;
} scui_color565_t;
#pragma pack()

/* RGB888: */
#pragma pack(1)
typedef union {
    struct {
        uint32_t b:8;
        uint32_t g:8;
        uint32_t r:8;
    } ch;
    uint8_t  byte[3];
    uint32_t full;
} scui_color888_t;
#pragma pack()

/* ARGB8565: */
#pragma pack(1)
typedef union {
    struct {
        uint32_t b:5;
        uint32_t g:6;
        uint32_t r:5;
        uint32_t a:8;
    } ch;
    uint8_t  byte[3];
    uint32_t full;
} scui_color8565_t;
#pragma pack()

/* ARGB8888: */
#pragma pack(1)
typedef union {
    struct {
        uint32_t b:8;
        uint32_t g:8;
        uint32_t r:8;
        uint32_t a:8;
    } ch;
    uint8_t  byte[4];
    uint32_t full;
} scui_color8888_t;
#pragma pack()

/* color format */
typedef enum {
    scui_color_format_rgb565,
    scui_color_format_rgb888,
    scui_color_format_argb8565,
    scui_color_format_argb8888,
}  scui_color_format_t;

/*@brief: 过度色, 固定此格式主要上层通用性
 */
typedef struct {
    union {
        scui_color8888_t color;         // 主色调
        scui_color8888_t color_s;       // 起始色调
    };
    union {
        scui_color8888_t color_render;  // 过度色调
        scui_color8888_t color_e;       // 结束色调
    };
} scui_color_gradient_t;

/*@brief: 透明度枚举值
 */
typedef enum {
    scui_alpha_pct0   = 0,
    scui_alpha_pct10  = 25,
    scui_alpha_pct20  = 51,
    scui_alpha_pct30  = 76,
    scui_alpha_pct40  = 102,
    scui_alpha_pct50  = 127,
    scui_alpha_pct60  = 153,
    scui_alpha_pct70  = 178,
    scui_alpha_pct80  = 204,
    scui_alpha_pct90  = 229,
    scui_alpha_pct100 = 255,
    scui_alpha_trans  = scui_alpha_pct0,
    scui_alpha_cover  = scui_alpha_pct100,
} scui_alpha_val_t;

/*@brief: 设备透明度格式:
 *        可选值scui_alpha_val_t
 *        固定到[0x00, 0xFF]
 */
typedef uint8_t scui_alpha_t;

/*@brief: 设备像素点格式:
 *        设备像素点格式是直达显示器的帧缓冲或画布
 *        无需额外携带透明度是因为在渲染的过程中
 *        透明度通道会让颜色值加以吸收
 */
typedef enum {
    scui_pixel_format_rgb565,
    scui_pixel_format_rgb888,
    scui_pixel_format_argb8565,
    scui_pixel_format_argb8888,
} scui_pixel_format_t;

/*@brief: 字节配置,元素提取,像素点逆序(慎用)
 *        最上层使用的color统一为ARGB8888类型
 *        而设备大多时候使用RGB565格式
 *        所以到设备需要有一个通用转换
 */
#if 0
#elif SCUI_PIXEL_FORMAT == scui_pixel_format_rgb565
#define SCUI_PIXEL_TYPE         scui_color565_t
#define SCUI_PIXEL_SIZE         sizeof(SCUI_PIXEL_TYPE)
#define SCUI_PIXEL_COND         SCUI_PIXEL_SIZE == 2
#define SCUI_PIXEL_R(color)     (((color) & 0xF800) >> 8)   // max-255
#define SCUI_PIXEL_G(color)     (((color) >> 3) & 0x00FC)   // max-255
#define SCUI_PIXEL_B(color)     (((color) << 3) & 0x00F8)   // max-255
#define SCUI_PIXEL_REV(pixel)   ((((pixel) & 0xFF) << 8) & (((pixel) & 0xFF00) >> 8))
#elif SCUI_PIXEL_FORMAT == scui_pixel_format_rgb888
#define SCUI_PIXEL_TYPE         scui_color888_t
#define SCUI_PIXEL_SIZE         sizeof(SCUI_PIXEL_TYPE)
#define SCUI_PIXEL_COND         SCUI_PIXEL_SIZE == 3
#define SCUI_PIXEL_R(color)     (((color) >> 16) & 0xFF)    // max-255
#define SCUI_PIXEL_G(color)     (((color) >>  8) & 0xFF)    // max-255
#define SCUI_PIXEL_B(color)     (((color) >>  0) & 0xFF)    // max-255
#define SCUI_PIXEL_REV(pixel)   ((((pixel) & 0xFF) << 16) & (((pixel) & 0x00FF00)) & (((pixel) & 0xFF0000) >> 16))
#else
#error "pixel format unknown"
#endif

/*@brief 颜色值格式转换
 *@param color888 颜色值
 *@retval 颜色值
 */
scui_color565_t scui_color_rgb888_to_rgb565(scui_color888_t color888);

/*@brief 颜色值格式转换(字节逆序)
 *@param color888 颜色值
 *@retval 颜色值
 */
scui_color565_t scui_color_rgb888_to_rgb565_rev(scui_color888_t color888);

/*@brief 颜色值格式转换
 *@param color565 颜色值
 *@retval 颜色值
 */
scui_color888_t scui_color_rgb565_to_rgb888(scui_color565_t color565);

/*@brief 颜色值格式转换(字节逆序)
 *@param color565 颜色值
 *@retval 颜色值
 */
scui_color888_t scui_color_rgb565_to_rgb888_rev(scui_color565_t color565);

/*@brief 计算透明度通过百分比值
 *@param percent 透明度百分比值[0, 100]
 *@retval 透明度
 */
scui_alpha_t scui_alpha_by_percent(uint8_t percent);

/*@brief 上层颜色值转为设备颜色值
 *       只用于scui_color_gradient_t类型的内部元素转换使用
 *@param color 颜色值
 *@retval 颜色值
 */
SCUI_PIXEL_TYPE scui_pixel_by_color(scui_color8888_t color);

/*@brief 像素点作用透明度
 *@param pixel 像素点
 *@param alpha 透明度
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_with_alpha(SCUI_PIXEL_TYPE *pixel, scui_alpha_t alpha);

/*@brief 像素点融合(同步作用透明度)
 *@param pixel_fg 像素点(fg)
 *@param alpha_fg 透明度(fg)
 *@param pixel_bg 像素点(bg)
 *@param alpha_bg 透明度(bg)
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_mix_with_alpha(SCUI_PIXEL_TYPE *pixel_1, scui_alpha_t alpha_1,
                                          SCUI_PIXEL_TYPE *pixel_2, scui_alpha_t alpha_2);

/*@brief 像素点混合(同步作用透明度)
 *@param pixel_fg 像素点(fg)
 *@param alpha_fg 透明度(fg)
 *@param pixel_bg 像素点(bg)
 *@param alpha_bg 透明度(bg)
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_blend_with_alpha(SCUI_PIXEL_TYPE *pixel_fg, scui_alpha_t alpha_fg,
                                            SCUI_PIXEL_TYPE *pixel_bg, scui_alpha_t alpha_bg);

#endif
