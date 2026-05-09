#ifndef SCUI_SOURCE_H
#define SCUI_SOURCE_H

/* 图标库 */
/* 缺图标可从此取用https://www.iconfont.cn */

/* #define SCUI_HANDLE_OFFSET_WIDGET    ((scui_handle_t)0x1000)     *//* 句柄表偏移:控件 */
/* #define SCUI_HANDLE_OFFSET_IMAGE     ((scui_handle_t)0x2000)     *//* 句柄表偏移:图片描述 */
/* #define SCUI_HANDLE_OFFSET_FONT      ((scui_handle_t)0x3000)     *//* 句柄表偏移:字库描述 */
/* #define SCUI_HANDLE_OFFSET_LANG      ((scui_handle_t)0x4000)     *//* 句柄表偏移:多语言表 */

/* 空回调,测试控件使用 */
#define SCUI_WIDGET_PARSER_EVENT_CB_EMPTY       (0)
/* 控制语言文件是否参与编译 */
#define SCUI_LANG_PARSER_BIN_USE                (1)



#include "scui_event_indev.h"           /* 输入设备事件表 */
#include "scui_event_custom.h"          /* 用户自定义事件表 */
#include "scui_image_parser.h"          /* 静态图形索引表 */
#include "scui_widget_parser.h"         /* 静态控件布局表 */
#include "scui_font_package.h"          /* 字库资源信息表 */
#include "scui_lang_parser.h"           /* 多国语字符串表 */
#include "scui_lang_font.h"             /* 多国语字库表 */
#include "scui_presenter.h"             /* presenter */
#include "scui_ready.h"                 /* ready */
#include "scui_ui.h"                    /* ui */

#endif
