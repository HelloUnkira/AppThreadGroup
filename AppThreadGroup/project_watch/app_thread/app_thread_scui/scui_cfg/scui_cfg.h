#ifndef SCUI_CONFIG_H
#define SCUI_CONFIG_H

/* 设备基础像素格式: */
#define SCUI_PIXEL_FORMAT           scui_pixel_format_rgb565

/* 帧缓冲区数量(1或2) */
#define SCUI_SURFACE_FB_LIMIT       (2)

/* 内存图片散列值,内存图片缓存最大限制 */
#define SCUI_IMAGE_LIMIT_HASH       (100)
#define SCUI_IMAGE_LIMIT_TOTAL      (1024 * 1024 * 1)

/* 输入设备:ptr */
#define SCUI_INDEV_PTR_CLICK        (100)
#define SCUI_INDEV_PTR_CLICK_SPAN   (300)   /* 连续点击最大间隔 */
#define SCUI_INDEV_PTR_FLING_COF    (1024)  /* fling检测: 放大系数 */
#define SCUI_INDEV_PTR_FLING_RATE   (512)   /* fling检测: distance * 放大系数 / time == 速度 * 放大系数 */
#define SCUI_INDEV_PTR_FLING_CNT    (18)    /* fling检测: 敏感度调整(允许掠过部分move) */

/* 输入设备:key */
#define SCUI_INDEV_KEY_LIMIT        (10)
#define SCUI_INDEV_KEY_CLICK        (200)
#define SCUI_INDEV_KEY_CLICK_SPAN   (300)   /* 连续点击最大间隔 */

/* 基础图元使用矩阵变换 */
#define SCUI_DRAW_MISC_USE_SOFTWARE     0
#define SCUI_DRAW_MISC_USE_MATRIX       1

#endif
