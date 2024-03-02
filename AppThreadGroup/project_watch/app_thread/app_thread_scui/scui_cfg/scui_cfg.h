#ifndef SCUI_CONFIG_H
#define SCUI_CONFIG_H

/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_CHECK       0
#define SCUI_MEM_RECORD_ITEM_DEF    128
#define SCUI_MEM_RECORD_ITEM_TEXT   256
#define SCUI_MEM_RECORD_ITEM_GRAPH  64

/* 缩放系数(Coefficient),为2的指数倍 */
#define SCUI_ZOOM_COF               (1024)
#define SCUI_ZOOM_SHIFT             (10)

/* 设备基础像素格式: */
#define SCUI_PIXEL_FORMAT           scui_pixel_format_rgb565

/* 帧缓冲区数量(1或2) */
#define SCUI_SURFACE_FB_LIMIT       (2)

/* 句柄表数量,共享句柄数量--最大限制 */
#define SCUI_HANDLE_TABLE_LIMIT     ((scui_handle_t)5)
#define SCUI_HANDLE_SHARE_LIMIT     ((scui_handle_t)512)
#define SCUI_HANDLE_SHARE_OFFSET    ((scui_handle_t)0x9FFF)

// #define SCUI_HANDLE_OFFSET_WIDGET   (0x1000)    /* 句柄表偏移:控件 */
// #define SCUI_HANDLE_OFFSET_IMAGE    (0x2000)    /* 句柄表偏移:图片描述 */
// #define SCUI_HANDLE_OFFSET_FONT     (0x3000)    /* 句柄表偏移:字库描述 */
// #define SCUI_HANDLE_OFFSET_LANG     (0x4000)    /* 句柄表偏移:多语言表 */

/* 动画更新频率,动画最大数量 */
#define SCUI_ANIMA_TICK             (16)
#define SCUI_ANIMA_LIMIT            (24)

/* 内存图片散列值,内存图片缓存最大限制 */
#define SCUI_IMAGE_LIMIT_HASH       (100)
#define SCUI_IMAGE_LIMIT_TOTAL      (1024 * 1024 * 1)

/* 输入设备:ptr */
#define SCUI_INDEV_PTR_CLICK        (100)
#define SCUI_INDEV_PTR_CLICK_SPAN   (300)   /* 连续点击最大间隔 */
#define SCUI_INDEV_PTR_FLING_SPAN   (2)     /* fling检测(数字越小越敏感):time / distance < span */
#define SCUI_INDEV_PTR_FLING_CNT    (8)     /* 开始可能超出fling检测 */

/* 输入设备:key */
#define SCUI_INDEV_KEY_LIMIT        (10)
#define SCUI_INDEV_KEY_CLICK        (200)
#define SCUI_INDEV_KEY_CLICK_SPAN   (300)   /* 连续点击最大间隔 */

/* 场景管理数量限制 */
#define SCUI_WINDOW_MGR_LIMIT       (10)

#endif
