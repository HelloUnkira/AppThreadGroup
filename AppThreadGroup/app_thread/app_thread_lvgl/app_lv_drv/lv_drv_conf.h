#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lv_conf.h"

/* display:屏幕尺寸,宽与高 */
#define LV_DRV_HOR_RES     320
#define LV_DRV_VER_RES     360
/* display:屏幕缩放因子 */
#define LV_DRV_ZOOM         1
/* display:多缓冲区 */
/*         插个眼:现在多缓冲区有渲染问题,显示错误 */
#define LV_DRV_DBUFFER      1

/* mouse:鼠标 */
#define LV_DRV_MOUSE            1
/* mousewheel:鼠标滚轮 */
#define LV_DRV_MOUSEWHEEL       1
/* keyboard:键盘 */
#define LV_DRV_KEYBOARD         1

#endif
