#ifndef APP_EXT_ARCH_DEV_H
#define APP_EXT_ARCH_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *cfg;    /* 设备操作参数 */
    const void *api;    /* 设备操作集合 */
    void *data;         /* 设备操作数据 */
} app_arch_dev_t;

/* 设备抽象实例 */
extern const app_arch_dev_t app_arch_dev_empty;
extern const app_arch_dev_t app_arch_log;
extern const app_arch_dev_t app_arch_rtc;
extern const app_arch_dev_t app_arch_watchdog;
extern const app_arch_dev_t app_arch_battery;
extern const app_arch_dev_t app_arch_vibrate;
extern const app_arch_dev_t app_arch_backlight;
extern const app_arch_dev_t app_arch_temperature;

/* 设备抽象动作接口 */
#include "app_ext_arch_dev_empty.h"     /* template */
#include "app_ext_arch_log.h"           /* log(common device) */
#include "app_ext_arch_rtc.h"           /* rtc */
#include "app_ext_arch_watchdog.h"      /* watchdog */
#include "app_ext_arch_battery.h"       /* battery */
#include "app_ext_arch_vibrate.h"       /* vibrate */
#include "app_ext_arch_backlight.h"     /* backlight */
#include "app_ext_arch_temperature.h"   /* temperature */

#endif
