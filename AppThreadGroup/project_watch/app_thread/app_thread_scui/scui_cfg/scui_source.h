#ifndef SCUI_SOURCE_H
#define SCUI_SOURCE_H

/* 图标库 */
/* 缺图标可从此取用https://www.iconfont.cn */

/* 空回调,测试控件使用 */
#define SCUI_WIDGET_PARSER_EVENT_CB_EMPTY       (0)
/* 控制语言文件是否参与编译 */
#define SCUI_LANG_PARSER_BIN_USE                (1)

#include "scui_event_indev.h"           /* 输入设备事件表 */
#include "scui_event_custom.h"          /* 用户自定义事件表 */
#include "scui_res_widget.h"            /* 静态控件布局表 */
#include "scui_res_image.h"             /* 静态图形索引表 */
#include "scui_res_font.h"              /* 字库资源信息表 */
#include "scui_res_lang.h"              /* 多国语字符串表 */
#include "scui_lang_font.h"             /* 多国语字库表 */
#include "scui_presenter.h"             /* presenter */
#include "scui_ui_maker.h"              /* maker */
#include "scui_ui_apply.h"              /* apply */
#include "scui_ready.h"                 /* ready */
#include "scui_ui.h"                    /* ui */

#endif
