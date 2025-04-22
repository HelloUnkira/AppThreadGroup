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
/* 内存哨兵监控(用于序列内存泄漏追查) */
#define SCUI_MEM_SENTRY_CHECK                       (1)
/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_CHECK                       (0)
#define SCUI_MEM_RECORD_ANALYSIS                    (1)
#define SCUI_MEM_RECORD_STATISTIC                   (1)
/* 内存使用记录监控(子类型): */
#define SCUI_MEM_RECORD_CHECK_MIX                   (0)
#define SCUI_MEM_RECORD_CHECK_FONT                  (0)
#define SCUI_MEM_RECORD_CHECK_GRAPH                 (0)
#define SCUI_MEM_RECORD_CHECK_USER                  (0)
/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_ITEM_MIX                    (1024)
#define SCUI_MEM_RECORD_ITEM_FONT                   (1024)
#define SCUI_MEM_RECORD_ITEM_GRAPH                  (1024)
#define SCUI_MEM_RECORD_ITEM_USER                   (1024)

#if 0
#elif   SCUI_MEM_SIZE_TYPE == 0     // 4M PSRAM (空闲1M)
#define SCUI_MEM_TYPE_SIZE_MIX                      (1024 * 64)
#define SCUI_MEM_TYPE_SIZE_FONT                     (1024 * 128)
#define SCUI_MEM_TYPE_SIZE_GRAPH                    (1024 * 1024 * 3 - 1024 * (256))
#define SCUI_MEM_TYPE_SIZE_USER                     (1024 * 64)
/* 缓存散列值(建议素数),缓存最大限制 */
#define SCUI_CACHE_FONT_HASH                        (3)
#define SCUI_CACHE_FONT_TOTAL                       (SCUI_MEM_TYPE_SIZE_FONT * 20 / 100)
#define SCUI_CACHE_FONT_GLYPH_HASH                  (23)
#define SCUI_CACHE_FONT_GLYPH_TOTAL                 (SCUI_MEM_TYPE_SIZE_FONT * 50 / 100)
#define SCUI_CACHE_IMAGE_HASH                       (17)
#define SCUI_CACHE_IMAGE_TOTAL                      (SCUI_MEM_TYPE_SIZE_GRAPH * 30 / 100)
#elif   SCUI_MEM_SIZE_TYPE == 1     // 8M PSRAM (空闲1M)
#define SCUI_MEM_TYPE_SIZE_MIX                      (1024 * 384)
#define SCUI_MEM_TYPE_SIZE_FONT                     (1024 * 256)
#define SCUI_MEM_TYPE_SIZE_GRAPH                    (1024 * 1024 * 7 - 1024 * (768))
#define SCUI_MEM_TYPE_SIZE_USER                     (1024 * 128)
/* 缓存散列值(建议素数),缓存最大限制 */
#define SCUI_CACHE_FONT_HASH                        (5)
#define SCUI_CACHE_FONT_TOTAL                       (SCUI_MEM_TYPE_SIZE_FONT * 20 / 100)
#define SCUI_CACHE_FONT_GLYPH_HASH                  (47)
#define SCUI_CACHE_FONT_GLYPH_TOTAL                 (SCUI_MEM_TYPE_SIZE_FONT * 50 / 100)
#define SCUI_CACHE_IMAGE_HASH                       (37)
#define SCUI_CACHE_IMAGE_TOTAL                      (SCUI_MEM_TYPE_SIZE_GRAPH * 60 / 100)
#else
#error "unknown type"
#endif

/* 放大系数以及偏移值 */
#define SCUI_SCALE_COF                              (1024)
#define SCUI_SCALE_OFS                              (10)

/* 设备像素格式(默认) */
#define SCUI_PIXEL_CF_DEF                           (scui_pixel_cf_bmp565)
#define SCUI_PIXEL_CF_DEF_A                         (scui_pixel_cf_bmp8565)

/* 动画更新频率,动画最大数量 */
#define SCUI_ANIMA_TICK                             (1000 / 100)
#define SCUI_ANIMA_LIMIT                            (10)

/* 输入设备:ptr */
#define SCUI_INDEV_PTR_CLICK                        (200)
#define SCUI_INDEV_PTR_CLICK_SPAN                   (300)   /* 连续点击最大间隔 */
#define SCUI_INDEV_PTR_FLING_RATE                   (448)   /* fling检测: distance * 放大系数 / time == 速度 * 放大系数 */
#define SCUI_INDEV_PTR_MOVE_DELTA                   (1)     /* move消抖 */
#define SCUI_INDEV_PTR_MOVE_CNT                     (6)     /* fling检测: 敏感度调整(允许掠过部分move) */

/* 输入设备:key */
#define SCUI_INDEV_KEY_LIMIT                        (10)
#define SCUI_INDEV_KEY_CLICK                        (200)
#define SCUI_INDEV_KEY_CLICK_SPAN                   (300)   /* 连续点击最大间隔 */

/* 句柄表数量,共享句柄数量--最大限制 */
#define SCUI_HANDLE_TABLE_LIMIT                     ((scui_handle_t)5)
#define SCUI_HANDLE_SHARE_LIMIT                     ((scui_handle_t)512)
#define SCUI_HANDLE_SHARE_OFFSET                    ((scui_handle_t)0x9FFF)

/* 帧缓冲异步(同步:1块;异步:2块;) */
#define SCUI_FRAME_BUFFER_ASYNC                     (2)

/* 主事件调度最长占用 */
#define SCUI_EVENT_OCCUPY_LIMIT                     (300 * 1000)

/* 图片外存直达画布(节约峰值内存) */
#define SCUI_WIDGET_IMAGE_DIRECT                    (1)
#define SCUI_WIDGET_IMAGE_DIRECT_LIMIT              (SCUI_DRV_HOR_RES * SCUI_DRV_VER_RES / 3)

/* 移动时停止帧动画(假渲染) */
#define SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL           (1)

/* 窗口切换动画时间(以1整屏计算) */
#define SCUI_WINDOW_MGR_SWITCH_MOVE_MS              (175)
#define SCUI_WINDOW_MGR_SWITCH_JUMP_MS              (225)

/* 窗口管理栈数量限制 */
#define SCUI_WINDOW_STACK_NEST                      (10)
/* 窗口管理数量限制 */
#define SCUI_WINDOW_MGR_LIMIT                       (5)
#define SCUI_WINDOW_MGR_SWITCH_MODE                 (1)
#define SCUI_WINDOW_MGR_SWITCH_KEY_TO_U             (scui_event_key_val_up)
#define SCUI_WINDOW_MGR_SWITCH_KEY_TO_D             (scui_event_key_val_down)
#define SCUI_WINDOW_MGR_SWITCH_KEY_TO_L             (scui_event_key_val_left)
#define SCUI_WINDOW_MGR_SWITCH_KEY_TO_R             (scui_event_key_val_right)
#define SCUI_WINDOW_MGR_TYPE_CFG                    (scui_window_switch_move)
#define SCUI_WINDOW_MGR_JUMP_DIR                    (scui_opt_dir_to_l)

/* 文字多行排版限制,可被分隔的字符 */
#define SCUI_STRING_LIMIT_LINE                      (64)
#define SCUI_STRING_BREAK_CHAR                      " ,.;:-_"

/* scroll:按键 */
#define SCUI_WIDGET_SCROLL_KEY_FDIR                 (scui_event_key_val_down)
#define SCUI_WIDGET_SCROLL_KEY_BDIR                 (scui_event_key_val_up)
/* string:行间距,字间距,空字符宽度 */
#define SCUI_WIDGET_STRING_GAP_LINE                 (0)
#define SCUI_WIDGET_STRING_GAP_ITEM                 (0)
#define SCUI_WIDGET_STRING_GAP_NONE                 (8)
/* string:文本滚动时间与距离 */
#define SCUI_WIDGET_STRING_UNIT_MS                  (SCUI_ANIMA_TICK * 2)
#define SCUI_WIDGET_STRING_UNIT_DX                  (3)
/* string:文本滚动空余间隙 */
#define SCUI_WIDGET_STRING_SCROLL_ITEM              (75)
#define SCUI_WIDGET_STRING_SCROLL_LINE              (50)
/* button:按钮1缩放限制百分比 */
#define SCUI_WIDGET_OBJECT_PCT                      (90)
#define SCUI_WIDGET_BUTTON_BTN1_PCT                 (90)

/* indev猴子测试 */
#define SCUI_MONKEY_TEST                            (0)

/* 图形变换使用矩阵变换(包括:缩放旋转) */
#define SCUI_DRAW_MISC_USE_MATRIX                   (1)

#endif
