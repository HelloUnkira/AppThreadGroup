#ifndef SCUI_CORE_H
#define SCUI_CORE_H

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
/* linker */
#include "scui_mem.h"

/* misc */
#include "scui_coord.h"         /* 坐标区域 */
#include "scui_general.h"       /* 基础图元 */
#include "scui_handle.h"        /* 泛用句柄 */
#include "scui_anima.h"         /* 简易动画 */
#include "scui_event.h"         /* 事件队列 */
#include "scui_event_sys.h"     /* 事件系统表 */
#include "scui_event_mgr.h"     /* 事件管理器 */
#include "scui_image.h"         /* 图像 */
#include "scui_image_cache.h"   /* 图像缓存 */
#include "scui_surface_fb.h"    /* 画布帧缓冲 */
#include "scui_clip.h"          /* 画布剪切域 */
#include "scui_math.h"          /* 数学库 */
#include "scui_coord3.h"        /* 三维坐标区域 */
#include "scui_matrix.h"        /* 矩阵库 */
#include "scui_engine.h"        /* 引擎 */
/* device */
#include "scui_indev.h"         /* 输入设备 */
#include "scui_indev_ptr.h"     /* 输入设备:ptr */
#include "scui_indev_enc.h"     /* 输入设备:enc */
#include "scui_indev_key.h"     /* 输入设备:key */
#include "scui_tick.h"          /* 滴答器 */
#include "scui_disp.h"          /* 显示器 */
/* font */
#include "scui_font.h"
#include "scui_font_cache.h"
#include "scui_font_glyph_cache.h"
#include "scui_string_layout.h"
#include "scui_string_layout_cache.h"

/* widget: */
#include "scui_widget.h"
#include "scui_widget_event.h"
#include "scui_widget_surface.h"
#include "scui_widget_opt.h"
#include "scui_window.h"
#include "scui_window_mgr.h"        /* 窗口管理器 */
#include "scui_window_opt.h"        /* 窗口管理器 */
#include "scui_window_switch.h"     /* 窗口管理器 */
#include "scui_window_float.h"      /* 窗口管理器 */
#include "scui_custom.h"
#include "scui_scroll.h"
#include "scui_string.h"
/* widget: */
#include "scui_watch.h"
#include "scui_chart.h"
#include "scui_ring.h"

/* draw */
#include "scui_draw_graph.h"
#include "scui_draw_basic.h"
#include "scui_draw_image.h"
#include "scui_draw_letter.h"
#include "scui_draw_string.h"

/* linker: */
#include "scui_image_src.h"     /* 图像资源加载卸载 */
#include "scui_font_src.h"      /* 字体资源获取 */

#endif
