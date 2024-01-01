#ifndef SCUI_IMAGE_H
#define SCUI_IMAGE_H

/* 图片像素字节占用偏移 */
#define SCUI_IMAGE_PIXEL_OFFSET     (8)

/* 图片像素格式: */
/* 枚举值为(bits << 8) | index */
typedef enum {
    scui_image_format_p4        = 0x0400,
    scui_image_format_rgb565    = 0x1600,
    scui_image_format_rgb888    = 0x2401,
    scui_image_format_argb8565  = 0x2400,
    scui_image_format_argb8888  = 0x3201,
} scui_image_format_t;

/* 图片状态: */
typedef enum {
    scui_image_status_none = 0,     // 默认图片
    scui_image_status_lz4,          // 压缩图片
    scui_image_status_num,
} scui_image_status_t;

/* 图片像素数据: */
typedef struct {
    uintptr_t data;         // 图片数据源(外存地址)
    uintptr_t width;        // 图片宽
    uintptr_t height;       // 图片高
    uintptr_t size_raw;     // 图片数据源(外存源大小)
    uintptr_t size_mem;     // 图片数据源(在内存中预期源大小)
} scui_image_pixel_t;

/* 图片数据来源: */
typedef enum {
    scui_image_from_mem = 0,    // 图片数据在内存中
    scui_image_from_ext,        // 图片数据在外存(静态资源)
    scui_image_from_cloud,      // 图片数据在外存(云存储资源)
    scui_image_from_num,
} scui_image_from_t;

/* 图片数据: */
typedef struct {
    scui_image_format_t format;     // 图片格式
    scui_image_status_t status;     // 图片状态
    scui_image_pixel_t  pixel;      // 图片色彩数据
    scui_image_from_t   from;       // 图片来源
} scui_image_t;

/*@brief 图片合法性检查
 *@param image 图片信息
 *@retval 合法或不合法
 */
bool scui_image_check(scui_image_t *image);

#endif
