#ifndef APP_DEV_LIB_H
#define APP_DEV_LIB_H

#include "app_dev_def.h"
#include "app_dev_empty.h"

/* 外设设备入app_dev库 */
/* 设备抽象实例(peripheral) */
extern const app_dev_t app_dev_ext_mem;
extern const app_dev_t app_dev_log;
extern const app_dev_t app_dev_rtc;
extern const app_dev_t app_dev_wdt;
/* 定制设备入app_dev库 */
/* 设备抽象实例(gui) */
extern const app_dev_t app_dev_gui_drv;
extern const app_dev_t app_dev_gui_disp;
extern const app_dev_t app_dev_gui_ptr;
extern const app_dev_t app_dev_gui_enc;
extern const app_dev_t app_dev_gui_key;
/* 设备抽象实例(custom) */
extern const app_dev_t app_dev_battery;
extern const app_dev_t app_dev_vibrate;
extern const app_dev_t app_dev_gesture;
extern const app_dev_t app_dev_backlight;
extern const app_dev_t app_dev_temp;

/* 设备抽象动作接口(peripheral) */
#include "app_dev_ext_mem.h"        /* ext_mem */
#include "app_dev_log.h"            /* log */
#include "app_dev_rtc.h"            /* rtc */
#include "app_dev_wdt.h"            /* watchdog */
/* 设备抽象动作接口(gui) */
#include "app_dev_gui.h"            /* gui */
#include "app_dev_gui_drv.h"        /* driver */
#include "app_dev_gui_disp.h"       /* display */
#include "app_dev_gui_ptr.h"        /* pointer */
#include "app_dev_gui_enc.h"        /* encode */
#include "app_dev_gui_key.h"        /* key */
/* 设备抽象动作接口(custom) */
#include "app_dev_battery.h"        /* battery */
#include "app_dev_vibrate.h"        /* vibrate */
#include "app_dev_backlight.h"      /* backlight */
#include "app_dev_gesture.h"        /* gesture */
#include "app_dev_temp.h"           /* temperature */

/*@brief 初始化设备
 */
void app_dev_ready(void);

#endif
