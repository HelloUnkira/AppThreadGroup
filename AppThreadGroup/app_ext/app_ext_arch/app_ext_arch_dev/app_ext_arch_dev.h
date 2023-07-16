#ifndef APP_EXT_ARCH_DEV_H
#define APP_EXT_ARCH_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *cfg;    /* 设备操作参数 */
    const void *api;    /* 设备操作集合 */
    void *data;         /* 设备操作数据 */
} app_arch_dev_t;

#include "app_ext_arch_dev_empty.h"     /* template */
#include "app_ext_arch_battery.h"       /* battery */
#include "app_ext_arch_vibrate.h"       /* vibrate */
#include "app_ext_arch_temperature.h"   /* temperature */

/* 设备抽象实例 */
extern const app_arch_dev_t app_arch_dev_empty;
extern const app_arch_dev_t app_arch_battery;
extern const app_arch_dev_t app_arch_vibrate;
extern const app_arch_dev_t app_arch_temperature;

#endif
