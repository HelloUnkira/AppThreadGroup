#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lv_conf.h"

/* lvgl使用模拟器 */
#define LV_DRV_USE_MONITOR  1

#if LV_DRV_USE_MONITOR

/* display:屏幕尺寸,宽与高 */
#define LV_DRV_HOR_RES     280
#define LV_DRV_VER_RES     240

/* display:屏幕缩放因子 */
#define LV_DRV_ZOOM         1

/* lvgl:多缓冲区 */
#define LV_DRV_DBUFFER      1

/* mouse:鼠标 */
#define LV_DRV_MOUSE            1
/* mousewheel:鼠标滚轮 */
#define LV_DRV_MOUSEWHEEL       1
/* keyboard:键盘 */
#define LV_DRV_KEYBOARD         1

#endif

#endif
