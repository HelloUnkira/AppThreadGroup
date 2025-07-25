#ifndef SCUI_CORE_H
#define SCUI_CORE_H

/* linker */
#include "scui_os.h"
#include "scui_log.h"
#include "scui_macro.h"
#include "scui_list_ll.h"       // 侵入式容器:链表(以及为载体的:队列,优先队列,栈)
#include "scui_table_dl.h"      // 侵入式容器:双链表为载体的哈希表(rbs平替目标)
#include "scui_table_rbs.h"     // 侵入式容器:红黑树为载体的哈希表
/* AppThreadGroup */
#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
/* linker */
#include "scui_mem.h"

/* misc */
#include "scui_math.h"          /* 数学库 */
#include "scui_coord.h"         /* 坐标区域 */
#include "scui_coord3.h"        /* 三维坐标区域 */
#include "scui_matrix.h"        /* 矩阵库 */
#include "scui_general.h"       /* 基础图元 */
#include "scui_handle.h"        /* 泛用句柄 */
#include "scui_anima.h"         /* 路径动画 */
#include "scui_event.h"         /* 事件队列 */
#include "scui_event_sys.h"     /* 事件系统表 */
#include "scui_event_mgr.h"     /* 事件管理器 */
#include "scui_image.h"         /* 图像 */
#include "scui_image_cache.h"   /* 图像缓存 */
#include "scui_frame_buffer.h"  /* 帧缓冲 */
#include "scui_clip.h"          /* 剪切域 */
/* device */
#include "scui_engine.h"        /* 引擎 */
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
/* draw */
#include "scui_draw_graph.h"
#include "scui_draw_basic.h"
#include "scui_draw_image.h"
#include "scui_draw_letter.h"
#include "scui_draw_misc.h"
#include "scui_draw_ring.h"

/* widget: */
#include "scui_widget.h"
#include "scui_widget_inf.h"
#include "scui_window.h"            /* 控件(窗口) */
#include "scui_window_inf.h"        /* 控件(窗口)API */
/* widget: */
#include "scui_string.h"            /* 控件(字符串) */
#include "scui_string_inf.h"        /* 控件(字符串)API */
#include "scui_custom.h"            /* 控件(自定义) */
#include "scui_custom_inf.h"        /* 控件(自定义)API */
#include "scui_scroll.h"            /* 控件(滚动) */
#include "scui_scroll_inf.h"        /* 控件(滚动)API */
#include "scui_linear.h"            /* 控件(列表) */
#include "scui_linear_inf.h"        /* 控件(列表)API */
#include "scui_roller.h"            /* 控件(滚轮) */
#include "scui_roller_inf.h"        /* 控件(滚轮)API */
/* widget: */
#include "scui_button.h"            /* 控件(按钮) */
#include "scui_button_inf.h"        /* 控件(按钮)API */
#include "scui_chart.h"             /* 控件(图表) */
#include "scui_chart_inf.h"         /* 控件(图表)API */

/* linker: */
#include "scui_image_frame.h"       /* 图像帧资源加载卸载 */
#include "scui_image_src.h"         /* 图像资源加载卸载 */
#include "scui_font_src.h"          /* 字体资源获取 */

#endif
