#ifndef SCUI_CONFIG_H
#define SCUI_CONFIG_H

/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_CHECK       1
#define SCUI_MEM_RECORD_ITEM_PART   128
#define SCUI_MEM_RECORD_ITEM_FONT   256
#define SCUI_MEM_RECORD_ITEM_IMAGE  64

/* 缩放系数(Coefficient),为2的指数倍 */
#define SCUI_ZOOM_COF               (1024)
#define SCUI_ZOOM_SHIFT             (10)

/* 设备基础像素格式: */
#define SCUI_PIXEL_FORMAT           scui_pixel_format_rgb565
/* 帧缓冲区数量(1或2) */
#define SCUI_SURFACE_FB_LIMIT       (2)

/* 静态句柄,动态句柄--最大限制 */
#define SCUI_HANDLE_LIMIT_S         (2048)
#define SCUI_HANDLE_LIMIT_D         (1024)

/* 动画更新频率,动画最大数量 */
#define SCUI_ANIMA_TICK             (16)
#define SCUI_ANIMA_LIMIT            (24)

/* 内存图片散列值,内存图片缓存最大限制 */
#define SCUI_IMAGE_LIMIT_HASH       (100)
#define SCUI_IMAGE_LIMIT_TOTAL      (1024 * 1024 * 1)



#endif
