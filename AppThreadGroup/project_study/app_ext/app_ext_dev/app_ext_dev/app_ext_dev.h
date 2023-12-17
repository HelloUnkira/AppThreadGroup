#ifndef APP_EXT_DEV_H
#define APP_EXT_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *api;    /* 设备操作参数 */
    void *cfg;          /* 设备操作参数(内部数据) */
    void *data;         /* 设备操作数据(对外数据) */
} app_dev_t;

/* 外设设备入app_ext库 */
/* 定制设备入app_dev库 */

/* 设备抽象实例(peripheral) */
extern const app_dev_t app_dev_log;
extern const app_dev_t app_dev_ext_mem;
extern const app_dev_t app_dev_rtc;
extern const app_dev_t app_dev_watchdog;
/* 设备抽象实例(custom) */

/* 设备抽象动作接口(template,不做实际的使用,只做编码结构参考) */
#include "app_ext_dev_empty.h"          /* empty */
/* 设备抽象动作接口(peripheral) */
#include "app_ext_dev_log.h"            /* log */
#include "app_ext_dev_ext_mem.h"        /* ext_mem */
#include "app_ext_dev_rtc.h"            /* rtc */
#include "app_ext_dev_watchdog.h"       /* watchdog */
/* 设备抽象动作接口(custom) */

/*@brief 初始化设备
 */
void app_dev_ready(void);

#endif
