#ifndef APP_OS_INTERFACE_H
#define APP_OS_INTERFACE_H

/* C std lib */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
/* C std lib need hook perhaps */
#include <stdlib.h>
#include <stdio.h>
/* 导出外部接口补充...... */
#include "app_os_adaptor.h"
#include "app_os_ext_mem.h"

/* 全局宏控开关及其等级 */
#define   APP_OS_LOG_GLOBAL_STATUS  1   /* 1:ENABLE,0:DISABLE */
#include "app_os_log.h"
#include "app_os_build_time.h"

#endif
