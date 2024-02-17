#ifndef SCUI_H
#define SCUI_H

/* linker */
#include "scui_os.h"
#include "scui_log.h"
#include "scui_macro.h"
#include "scui_list_l.h"        // 侵入式容器:单链表双链表以及为载体的队列,优先队列,栈
#include "scui_table_rbs.h"     // 侵入式容器:红黑树为载体的哈希表
/* AppThreadGroup */
#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

/* config: */
#include "scui_config.h"
/* misc */
#include "scui_coord.h"         /* 坐标区域 */
#include "scui_general.h"       /* 基础图元 */
#include "scui_handle.h"        /* 泛用句柄 */
#include "scui_anima.h"         /* 简易动画 */
#include "scui_event.h"         /* 事件队列 */
#include "scui_event_mgr.h"     /* 事件管理器 */
#include "scui_image.h"         /* 图像 */
#include "scui_image_cache.h"   /* 图像缓存 */
#include "scui_surface.h"       /* 画布 */
#include "scui_surface_fb.h"    /* 画布帧缓冲 */
#include "scui_math.h"          /* 数学库 */
#include "scui_engine.h"        /* 引擎 */
/* device */
#include "scui_indev.h"         /* 输入设备 */
#include "scui_indev_ptr.h"     /* 输入设备:ptr */
#include "scui_indev_enc.h"     /* 输入设备:enc */
#include "scui_indev_key.h"     /* 输入设备:key */
#include "scui_tick.h"          /* 滴答器 */
#include "scui_disp.h"          /* 显示器 */
/* widget: */
#include "scui_widget.h"
#include "scui_widget_cb.h"
#include "scui_widget_opt.h"
#include "scui_widget_attr.h"
#include "scui_widget_event.h"
#include "scui_widget_surface.h"
#include "scui_window.h"
#include "scui_window_mgr.h"    /* 窗口管理器 */
#include "scui_window_opt.h"    /* 窗口管理器 */
#include "scui_custom.h"
#include "scui_scroll.h"
/* draw */
#include "scui_draw_graph.h"
#include "scui_draw_basic.h"
#include "scui_draw_image.h"

/* linker: */
#include "scui_mem.h"
#include "scui_image_src.h"     /* 图像资源加载卸载 */
/* config: */
#include "scui_source.h"

#endif
