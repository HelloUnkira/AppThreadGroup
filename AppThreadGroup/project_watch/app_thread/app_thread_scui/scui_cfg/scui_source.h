#ifndef SCUI_SOURCE_H
#define SCUI_SOURCE_H

#include "scui_event_indev.h"           /* 输入设备事件表 */
#include "scui_event_custom.h"          /* 用户自定义事件表 */
#include "scui_image_parser.h"          /* 静态图形索引表 */
#include "scui_widget_parser.h"         /* 静态控件布局表 */
#include "scui_multi_lang.h"            /* 多国语字符串表 */
#include "scui_multi_lang_font.h"       /* 多国语字库表 */

// monkey
#include "scui_monkey.h"
// presenter
#include "scui_presenter.h"
// cwf json
#include "scui_cwf_json_proto.h"
#include "scui_cwf_json_parser.h"
#include "scui_cwf_json_maker.h"

// 图标库
// 缺图标可从此取用https://www.iconfont.cn

/* 空回调,测试控件使用 */
#define SCUI_WIDGET_EVENT_USE_EMPTY     (0)

// #define SCUI_HANDLE_OFFSET_WIDGET    ((scui_handle_t)0x1000)    /* 句柄表偏移:控件 */
// #define SCUI_HANDLE_OFFSET_IMAGE     ((scui_handle_t)0x2000)    /* 句柄表偏移:图片描述 */
// #define SCUI_HANDLE_OFFSET_FONT      ((scui_handle_t)0x3000)    /* 句柄表偏移:字库描述 */
// #define SCUI_HANDLE_OFFSET_LANG      ((scui_handle_t)0x4000)    /* 句柄表偏移:多语言表 */

#endif
