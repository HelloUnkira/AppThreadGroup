#ifndef SCUI_CFG_H
#define SCUI_CFG_H

/* display:屏幕尺寸,宽与高 */
/* display:多缓冲区(插个眼:现在多缓冲区有渲染问题,显示错误) */
#define SCUI_DRV_HOR_RES                            (466)
#define SCUI_DRV_VER_RES                            (466)
#define SCUI_DRV_PIXEL_DEPTH                        (16)
#define SCUI_DRV_DBUFFER                            (1)

/* 内存使用占用(0: 4M PSRAM; 1: 8M PSRAM) */
#define SCUI_MEM_SIZE_TYPE                          (1)

#if 0
#elif SCUI_MEM_SIZE_TYPE == 0   // 4M PSRAM (空闲1M)
#define SCUI_MEM_TYPE_SIZE_MIX                      (1024 * 128)
#define SCUI_MEM_TYPE_SIZE_FONT                     (1024 * 128)
#define SCUI_MEM_TYPE_SIZE_GRAPH                    (1024 * 1024 * 3 - 1024 * (128 + 128))
#elif SCUI_MEM_SIZE_TYPE == 1   // 8M PSRAM (空闲1M)
#define SCUI_MEM_TYPE_SIZE_MIX                      (1024 * 256)
#define SCUI_MEM_TYPE_SIZE_FONT                     (1024 * 512)
#define SCUI_MEM_TYPE_SIZE_GRAPH                    (1024 * 1024 * 7 - 1024 * (256 + 512))
#else
#error "unknown type"
#endif

/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_CHECK                       (0)
#define SCUI_MEM_RECORD_ANALYSIS                    (0)
#define SCUI_MEM_RECORD_STATISTIC                   (1)
/* 内存使用记录监控(子类型): */
#define SCUI_MEM_RECORD_CHECK_MIX                   (1)
#define SCUI_MEM_RECORD_CHECK_FONT                  (0)
#define SCUI_MEM_RECORD_CHECK_GRAPH                 (0)
/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_ITEM_MIX                    (512)
#define SCUI_MEM_RECORD_ITEM_FONT                   (256)
#define SCUI_MEM_RECORD_ITEM_GRAPH                  (64)

#if 0
#elif SCUI_MEM_SIZE_TYPE == 0
/* 内存图片散列值,内存图片缓存最大限制 */
#define SCUI_CACHE_IMAGE_HASH                       (17)
#define SCUI_CACHE_IMAGE_TOTAL                      (1024 * 1024 * 1)
/* 内存文字散列值,内存文字缓存最大限制 */
#define SCUI_CACHE_FONT_HASH                        (3)
#define SCUI_CACHE_FONT_TOTAL                       (1024 * 64)
/* 内存文字散列值,内存文字缓存最大限制 */
#define SCUI_CACHE_FONT_GLYPH_HASH                  (23)
#define SCUI_CACHE_FONT_GLYPH_TOTAL                 (1024 * 64)
#elif SCUI_MEM_SIZE_TYPE == 1
/* 内存图片散列值,内存图片缓存最大限制 */
#define SCUI_CACHE_IMAGE_HASH                       (17)
#define SCUI_CACHE_IMAGE_TOTAL                      (1024 * 1024 * 4)
/* 内存文字散列值,内存文字缓存最大限制 */
#define SCUI_CACHE_FONT_HASH                        (3)
#define SCUI_CACHE_FONT_TOTAL                       (1024 * 384)
/* 内存文字散列值,内存文字缓存最大限制 */
#define SCUI_CACHE_FONT_GLYPH_HASH                  (23)
#define SCUI_CACHE_FONT_GLYPH_TOTAL                 (1024 * 384)
#else
#endif

/* 放大系数以及偏移值 */
#define SCUI_SCALE_COF                              (1024)
#define SCUI_SCALE_OFS                              (10)

/* 设备像素格式(默认) */
#define SCUI_PIXEL_CF_DEF                           (scui_pixel_cf_bmp565)
#define SCUI_PIXEL_CF_DEF_A                         (scui_pixel_cf_bmp8565)

/* 动画更新频率,动画最大数量 */
#define SCUI_ANIMA_TICK                             (1000 / 60)
#define SCUI_ANIMA_LIMIT                            (10)

/* 输入设备:ptr */
#define SCUI_INDEV_PTR_CLICK                        (100)
#define SCUI_INDEV_PTR_CLICK_SPAN                   (300)   /* 连续点击最大间隔 */
#define SCUI_INDEV_PTR_FLING_RATE                   (448)   /* fling检测: distance * 放大系数 / time == 速度 * 放大系数 */
#define SCUI_INDEV_PTR_FLING_CNT                    (6)     /* fling检测: 敏感度调整(允许掠过部分move) */

/* 输入设备:key */
#define SCUI_INDEV_KEY_LIMIT                        (10)
#define SCUI_INDEV_KEY_CLICK                        (200)
#define SCUI_INDEV_KEY_CLICK_SPAN                   (300)   /* 连续点击最大间隔 */

/* 检查事件调度耗时:以ms计 */
#define SCUI_EVENT_MGR_TICK_CHECK                   (0)
#define SCUI_EVENT_MGR_TICK_FILTER                  (100)

/* 句柄表数量,共享句柄数量--最大限制 */
#define SCUI_HANDLE_TABLE_LIMIT                     ((scui_handle_t)5)
#define SCUI_HANDLE_SHARE_LIMIT                     ((scui_handle_t)512)
#define SCUI_HANDLE_SHARE_OFFSET                    ((scui_handle_t)0x9FFF)

/* 帧缓冲区数量(1或2) */
#define SCUI_SURFACE_FB_LIMIT                       (2)

/* 移动时停止帧动画(假渲染) */
#define SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL           (1)

/* 窗口管理数量限制 */
#define SCUI_WINDOW_MGR_LIMIT                       (5)
#define SCUI_WINDOW_MGR_SWITCH_MODE                 (1)
/* 窗口交互方向 */
#define SCUI_WINDOW_JUMP_DIR                        (scui_opt_dir_to_l)
/* 窗口管理栈数量限制 */
#define SCUI_WINDOW_STACK_NEST                      (10)

/* 内存文字宽度,文字边距: */
#define SCUI_STRING_LIMIT_LINE                      (64)
#define SCUI_STRING_SPACE_WIDTH                     (8)

/* 检查事件调度耗时:以ms计 */
#define SCUI_WIDGET_SURFACE_DRAW_TICK_CHECK         (0)
#define SCUI_WIDGET_SURFACE_DRAW_TICK_FILTER        (1000)

/* 文本滚动时间与距离 */
#define SCUI_WIDGET_STRING_UNIT_MS                  (SCUI_ANIMA_TICK)
#define SCUI_WIDGET_STRING_UNIT_DX                  (3)
/* 按钮1缩放限制百分比 */
#define SCUI_WIDGET_BUTTON_BTN1_PCT                 (90)

/* 基础图元使用矩阵变换 */
#define SCUI_DRAW_MISC_USE_SOFTWARE                 (0)
#define SCUI_DRAW_MISC_USE_MATRIX                   (1)

#endif
