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

/* 设备抽象动作接口(peripheral) */
#include "app_dev_ext_mem.h"        /* ext_mem */
#include "app_dev_log.h"            /* log */
#include "app_dev_rtc.h"            /* rtc */
#include "app_dev_wdt.h"            /* watchdog */

/*@brief 初始化设备
 */
void app_dev_ready(void);

#endif
