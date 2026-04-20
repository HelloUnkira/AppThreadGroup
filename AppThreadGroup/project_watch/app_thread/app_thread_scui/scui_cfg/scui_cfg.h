#ifndef SCUI_CFG_H
#define SCUI_CFG_H

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* thread */
/* 多线程任务调度优先级(数字越小优先级越高) */
/* 输出设备送显任务优先级(0) */
/* 输入设备监听任务优先级(1) */
/* 事件派发调度任务优先级(2) */
/* 硬件绘制调度任务优先级(3) */
/* 软件绘制调度任务优先级(4) */

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* mem: */

/* 内存特性选择 */
#define SCUI_MEM_FEAT_MINI                          (0)
/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_CHECK                       (0)
#define SCUI_MEM_RECORD_ANALYSIS                    (1)
#define SCUI_MEM_RECORD_STATISTIC                   (1)
/* 内存使用记录监控: */
#define SCUI_MEM_RECORD_ITEM_MIX                    (1024)
#define SCUI_MEM_RECORD_ITEM_FONT                   (1024)
#define SCUI_MEM_RECORD_ITEM_GRAPH                  (1024)
#define SCUI_MEM_RECORD_ITEM_USER                   (1024)
/* 内存使用记录监控(子类型): */
#define SCUI_MEM_RECORD_CHECK_MIX                   (0)
#define SCUI_MEM_RECORD_CHECK_FONT                  (0) /* SCUI_MEM_FEAT_MINI: 不支持, 必须为0 */
#define SCUI_MEM_RECORD_CHECK_GRAPH                 (0) /* SCUI_MEM_FEAT_MINI: 不支持, 必须为0 */
#define SCUI_MEM_RECORD_CHECK_USER                  (0) /* SCUI_MEM_FEAT_MINI: 不支持, 必须为0 */
/* 内存哨兵监控(内存边界追查) */
#define SCUI_MEM_SENTRY_CHECK                       (1)

#if SCUI_MEM_FEAT_MINI
/* 小内存方案(开发中......) */
/* 分块双Buffer + 缓存(可选) */

/* 不支持特性(不推荐, 缺内存): */
/* SCUI_MEM_FEAT_MINI == true: */
/* 字符串ttf字库, 字符串色彩级联, 字符串绘制缓存块(不推荐) */
/* 子控件树, 静态画布真缩放, 窗口转场变换, 多任务调度 */

#define SCUI_MEM_TYPE_SIZE_MIX                      (0x80000 + /* 临时缓存: */ 0x400000)
#define SCUI_MEM_TYPE_SIZE_FONT                     (0)
#define SCUI_MEM_TYPE_SIZE_GRAPH                    (0)
#define SCUI_MEM_TYPE_SIZE_USER                     (0)

/* 缓存散列值(可选:不使用为0) */
#define SCUI_CACHE_HASH_FONT                        (3)
#define SCUI_CACHE_HASH_GLYPH                       (23)
#define SCUI_CACHE_HASH_IMAGE                       (17)
/* 缓存最大限制 */
#define SCUI_CACHE_TOTAL_FONT                       (SCUI_MEM_TYPE_SIZE_MIX * 10 / 100)
#define SCUI_CACHE_TOTAL_GLYPH                      (SCUI_MEM_TYPE_SIZE_MIX * 10 / 100)
#define SCUI_CACHE_TOTAL_IMAGE                      (SCUI_MEM_TYPE_SIZE_MIX * 30 / 100)
#else
/* 大内存方案(完整效果丰盈线: 6M左右): */
/* MIX:128K; FONT:192K; USER:64K; GRAPH:7.5M - other */

#define SCUI_MEM_TYPE_SIZE_MIX                      (0x20000)
#define SCUI_MEM_TYPE_SIZE_FONT                     (0x30000)
#define SCUI_MEM_TYPE_SIZE_GRAPH                    (0x780000 - 0x60000)
#define SCUI_MEM_TYPE_SIZE_USER                     (0x10000)

/* 缓存散列值(必要使用) */
#define SCUI_CACHE_HASH_FONT                        (5)
#define SCUI_CACHE_HASH_GLYPH                       (53)
#define SCUI_CACHE_HASH_IMAGE                       (61)
/* 缓存最大限制 */
#define SCUI_CACHE_TOTAL_FONT                       (SCUI_MEM_TYPE_SIZE_FONT  * 20 / 100)
#define SCUI_CACHE_TOTAL_GLYPH                      (SCUI_MEM_TYPE_SIZE_FONT  * 50 / 100)
#define SCUI_CACHE_TOTAL_IMAGE                      (SCUI_MEM_TYPE_SIZE_GRAPH * 60 / 100)
#endif

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* indev: */

/* 事件合并:多次点击等等 */
#define SCUI_INDEV_EVENT_MERGE                      (0)

/* 输入设备:ptr */
#define SCUI_INDEV_PTR_CLICK_TIME                   (200)   /* click: 点击时间 */
#define SCUI_INDEV_PTR_CLICK_RANGE                  (10)    /* click: 点击抖动范围 */
#define SCUI_INDEV_PTR_CLICK_SPAN                   (300)   /* click: 连续点击最大间隔 */
#define SCUI_INDEV_PTR_FLING_RATE                   (448)   /* fling: 距离 * 系数 / 时间 == 速度 * 系数 */
#define SCUI_INDEV_PTR_MOVE_CNT                     (6)     /* fling: 敏感度调整(允许掠过部分move) */
#define SCUI_INDEV_PTR_MOVE_DELTA                   (1)     /* move:  移动消抖, 最小移动距离 */

/* 输入设备:key */
#define SCUI_INDEV_KEY_LIMIT                        (10)
#define SCUI_INDEV_KEY_CLICK_TIME                   (200)   /* click: 点击时间 */
#define SCUI_INDEV_KEY_CLICK_SPAN                   (300)   /* click: 连续点击最大间隔 */

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* sys: */

/* 句柄表数量,共享句柄数量--最大限制 */
#define SCUI_HANDLE_TABLE_LIMIT                     ((scui_handle_t)5)
#define SCUI_HANDLE_SHARE_LIMIT                     ((scui_handle_t)512)
#define SCUI_HANDLE_SHARE_OFFSET                    ((scui_handle_t)0x9FFF)

/* 像素颜色格式位深 */
#define SCUI_PCF_DEP                                (16)

/* 屏幕尺寸(帧尺寸, 全局窗口尺寸) */
#define SCUI_HOR_RES                                (466)
#define SCUI_VER_RES                                (466)

/* 放大系数以及偏移值 */
#define SCUI_SCALE_COF                              (1024)
#define SCUI_SCALE_OFS                              (10)

/* 设备像素格式(默认) */
#define SCUI_PIXEL_CF_DEF                           (scui_pixel_cf_bmp565)
#define SCUI_PIXEL_CF_DEF_A                         (scui_pixel_cf_bmp8565)

/* LRU缓存器最大散列限制 */
#define SCUI_CACHE_LRU_HASH_LIMIT                   (101)

/* 动画更新频率,动画最大数量 */
#define SCUI_ANIMA_TICK                             (1000 / 60)
#define SCUI_ANIMA_LIMIT                            (10 + 20)   /* system + widget */

/* 主事件调度最长占用 */
#define SCUI_EVENT_OCCUPY_LIMIT                     (300 * 1000)

/* 帧缓冲异步(同步:1块;异步:2块;) */
#define SCUI_FRAME_BUFFER_ASYNC                     (2)
#define SCUI_FRAME_BUFFER_SEG                       (95 /* 95 SCUI_VER_RES / 3 */)

/* 绘制任务序列 */
#define SCUI_DRAW_TASK_SEQ                          (0 && !SCUI_MEM_FEAT_MINI)
#if     SCUI_DRAW_TASK_SEQ
#define SCUI_DRAW_TASK_HASH_HBIT                    (8 * 2)
#define SCUI_DRAW_TASK_HASH_VBIT                    (8 * 2)
#define SCUI_DRAW_TASK_HWACC_NUM                    (2) /* 硬件加速器单例数量 */
#define SCUI_DRAW_TASK_ASYNC_NUM                    (1 + SCUI_DRAW_TASK_HWACC_NUM)
#define SCUI_DRAW_TASK_DSC_NUM                      (300)
/* 使用任务序列需增大该目标 */
#else
#define SCUI_DRAW_TASK_ASYNC_NUM                    (1)
#define SCUI_DRAW_TASK_DSC_NUM                      (SCUI_MEM_FEAT_MINI ? 10 : 20)
#endif

/* 矢量绘图引擎 */
#define SCUI_DRAW_USE_THORVG                        (1)
/* 第三方图元库 */
#define SCUI_DRAW_GRAPH_USE_EGUI                    (1)
#define SCUI_DRAW_GRAPH_USE_LVGL                    (0) /* unsupport yet */
#define SCUI_DRAW_GRAPH_USE_XXXX                    (0) /* unsupport yet */
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* feat: */

/* 移动时停止帧动画(假渲染) */
#define SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL           (1 && !SCUI_MEM_FEAT_MINI)

/* 图片外存直达画布(节约峰值内存, 但会加重绘制时间) */
#define SCUI_WIDGET_IMAGE_DIRECT                    (0 && !SCUI_MEM_FEAT_MINI)
#define SCUI_WIDGET_IMAGE_DIRECT_LIMIT              (SCUI_HOR_RES * SCUI_VER_RES / 3)

/* 窗口管理器参数配置 */
/* 窗口切换动画速度[ptr, enc, key, auto] */
#define SCUI_WINDOW_LIST_LIMIT                      (5)
#define SCUI_WINDOW_STACK_NEST                      (10)
#define SCUI_WINDOW_SWITCH_MODE                     (1 && !SCUI_MEM_FEAT_MINI)
#define SCUI_WINDOW_SWITCH_MOVE_SPD                 {2000, 1700, 1700, 1000, 300}

/* 控件布局标记(为控件添加随机颜色轮廓) */
#define SCUI_WIDGET_BORDER_TAG                      (0)

/* 文字多行排版限制,可被分隔的字符 */
#define SCUI_STRING_LIMIT_LINE                      (64)
#define SCUI_STRING_BREAK_CHAR                      " \n.:,;-_"
#define SCUI_STRING_SPACE_CHAR                      " \n"

/* custom:文本绘制的最大缓存条目 */
#define SCUI_CUSTOM_STR_ITEM_LIMIT                  (32)
/* scroll:按键, 动画速度[ptr, enc, key, auto] */
#define SCUI_WIDGET_SCROLL_SPD_ANIM                 {1200, 900, 900, 900}
#define SCUI_WIDGET_SCROLL_KEY_FDIR                 (scui_event_key_val_down)
#define SCUI_WIDGET_SCROLL_KEY_BDIR                 (scui_event_key_val_up)
/* string:行间距,字间距,空字符宽度 */
#define SCUI_WIDGET_STRING_GAP_LINE                 (0)
#define SCUI_WIDGET_STRING_GAP_ITEM                 (0)
#define SCUI_WIDGET_STRING_GAP_NONE                 (8)
/* string:文本滚动时间与距离 */
#define SCUI_WIDGET_STRING_UNIT_MS                  (SCUI_ANIMA_TICK)
#define SCUI_WIDGET_STRING_UNIT_DX                  (2)
/* string:文本滚动空余间隙 */
#define SCUI_WIDGET_STRING_SCROLL_ITEM              (75)
#define SCUI_WIDGET_STRING_SCROLL_LINE              (50)
/* roller:按键, 动画速度[ptr, enc, key, auto] */
#define SCUI_WIDGET_ROLLER_SPD_ANIM                 {1000, 75, 75, 75}

/* menial(btn):缩放限制百分比 */
#define SCUI_WIDGET_MENIAL_BTN_TIME                 (100)
#define SCUI_WIDGET_MENIAL_BTN_PCT                  (90)
/* menial(arc):进度动画时间(360度) */
#define SCUI_WIDGET_MENIAL_ARC_TIME                 (500)
/* menial(bar):进度动画时间(100%) */
#define SCUI_WIDGET_MENIAL_BAR_TIME                 (500)
#define SCUI_WIDGET_MENIAL_BAR_EXT_SLIDER_TIME      (500)
#define SCUI_WIDGET_MENIAL_BAR_EXT_SWITCH_TIME      (100)



/* button:按钮1缩放限制百分比 */
#define SCUI_WIDGET_BUTTON_BTN1_PCT                 (90)

/* indev猴子测试 */
#define SCUI_MONKEY_TEST                            (0)

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
