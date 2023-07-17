#ifndef APP_EXT_DEV_H
#define APP_EXT_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *cfg;    /* 设备操作参数 */
    const void *api;    /* 设备操作集合 */
    void *data;         /* 设备操作数据 */
} app_dev_t;

/* 设备抽象实例 */
extern const app_dev_t app_dev_empty;
extern const app_dev_t app_dev_log;
extern const app_dev_t app_dev_rtc;
extern const app_dev_t app_dev_watchdog;
extern const app_dev_t app_dev_battery;
extern const app_dev_t app_dev_vibrate;
extern const app_dev_t app_dev_backlight;
extern const app_dev_t app_dev_temperature;

/* 设备抽象动作接口 */
#include "app_ext_dev_empty.h"          /* template */
#include "app_ext_dev_log.h"            /* log(common device) */
#include "app_ext_dev_rtc.h"            /* rtc */
#include "app_ext_dev_watchdog.h"       /* watchdog */
#include "app_ext_dev_battery.h"        /* battery */
#include "app_ext_dev_vibrate.h"        /* vibrate */
#include "app_ext_dev_backlight.h"      /* backlight */
#include "app_ext_dev_temperature.h"    /* temperature */

#endif
