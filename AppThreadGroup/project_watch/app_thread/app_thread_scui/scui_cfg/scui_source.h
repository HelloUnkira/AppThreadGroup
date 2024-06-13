#ifndef SCUI_SOURCE_H
#define SCUI_SOURCE_H

#include "lz4.h"

#include "scui_event_indev.h"       /* 输入设备事件表 */
#include "scui_event_custom.h"      /* 用户自定义事件表 */
#include "scui_image_parser.h"      /* 静态图形索引表 */
#include "scui_widget_parser.h"     /* 静态控件布局表 */
#include "scui_multi_lang.h"        /* 多国语字符串表 */
#include "scui_font_type.h"         /* 字库类型表 */

/* 空回调,测试控件使用 */
#define SCUI_WIDGET_EVENT_USE_EMPTY                 (0)

/*****************************************************************************/
/* ui的一些配置参数 */
#define SCUI_UI_SCROLL_BAR_STOP_TIME                (2000)
#define SCUI_UI_SCROLL_BAR_FADE_TIME                (2000)
#define SCUI_UI_POPUP_STOP_TIME                     (2000)
#define SCUI_UI_POPUP_FADE_TIME                     (2000)

/*****************************************************************************/

#endif
