#ifndef SCUI_GENERAL_H
#define SCUI_GENERAL_H

/* RGB565: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:5;
        uint8_t g:6;
        uint8_t r:5;
    } ch;
    uint8_t  byte[2];
    uint16_t full;
} scui_color565_t;
#pragma pack()

/* RGB888: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:8;
        uint8_t g:8;
        uint8_t r:8;
    } ch;
    uint8_t  byte[3];
    uint32_t full;
} scui_color888_t;
#pragma pack()

/* ARGB6666: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:6;
        uint8_t g:6;
        uint8_t r:6;
        uint8_t a:6;
    } ch;
    uint8_t  byte[3];
    uint32_t full;
} scui_color6666_t;
#pragma pack()

/* ARGB8888: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:8;
        uint8_t g:8;
        uint8_t r:8;
        uint8_t a:8;
    } ch;
    uint8_t  byte[4];
    uint32_t full;
} scui_color8888_t;
#pragma pack()

/* 几个不同像素格式转化(EX指字节逆序) */
#define SCUI_RGB888_TO_565(x)        ((((x) >> 19 & 0x1F) << 11) | (((x) >> 10 & 0x3F) << 5) | (((x) >> 3 & 0x1F) <<  0))
#define SCUI_RGB888_TO_565_EX(x)     ((((x) >> 19 & 0x1F) <<  0) | (((x) >> 10 & 0x38) << 5) | (((x) >> 3 & 0x1F) << 11) | (((x) >> 10 & 0x07) << 8))
#define SCUI_RGB666_TO_565(x)        ((((x) >> 13 & 0x1F) << 11) | (((x) >>  6 & 0x3F) << 5) | (((x) >> 1 & 0x1F) <<  0))
#define SCUI_RGB666_TO_565_EX(x)     ((((x) >> 13 & 0x1F) <<  0) | (((x) >>  6 & 0x38) << 5) | (((x) >> 1 & 0x1F) << 11) | (((x) >>  6 & 0x07) << 8))

/* alpha: */
typedef uint8_t scui_alpha_t;

// 绘制实体抽象:画布
// 画布是一整块区域或不存在(共用画布)
typedef struct {
    uint8_t     *pixel;     // 画布像素流地址
    scui_area_t  clip;      // 画布空间剪切域
    scui_alpha_t alpha;     // 画布区域透明度
} scui_surface_t;

/*@brief: 设备像素点格式:
 *        设备像素点格式是直达显示器的帧缓冲或画布
 *        无需额外携带透明度是因为在渲染的过程中
 *        透明度通道会让颜色值加以吸收
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

/*@brief 像素点作用透明度
 *@param pixel 像素点
 *@param alpha 透明度
 *@retval 像素点
 */
SCUI_PIXEL_TYPE scui_pixel_with_alpha(SCUI_PIXEL_TYPE *pixel, scui_alpha_t alpha);

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
