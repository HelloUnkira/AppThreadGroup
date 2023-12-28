#ifndef SCUI_H
#define SCUI_H

/* AppThreadGroup */
#include "app_ext_lib.h"
#include "app_sys_lib.h"
/* config: */
#include "scui_config.h"
/* arch: */
#include "scui_tick.h"
#include "scui_util.h"
#include "scui_mem.h"
/* core: */
#include "scui_coord.h"         /* 坐标区域 */
#include "scui_general.h"       /* 基础图元 */
#include "scui_handle.h"        /* 泛用句柄 */
#include "scui_anima.h"         /* 简易动画 */
#include "scui_event.h"         /* 事件队列 */
#include "scui_event_mgr.h"     /* 事件管理器 */
#include "scui_engine.h"        /* 引擎 */
#include "scui_image.h"         /* 图像 */
/* misc */
#include "scui_math.h"
/* draw */
#include "scui_surface_fb.h"



/* widget: */
#include "scui_widget.h"
#include "scui_widget_ogz.h"
#include "scui_widget_list.h"
#include "scui_widget_gc.h"



/* 自定义事件表 */
#include "scui_event_custom.h"
/* 静态图形索引表 */
#include "scui_image_combine.h"



#endif
