#ifndef APP_EXT_ARCH_DEV_H
#define APP_EXT_ARCH_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *cfg;    /* 设备操作参数 */
    const void *api;    /* 设备操作集合 */
    void *data;         /* 设备操作数据 */
} app_arch_dev_t;

/* temperature */
#include "app_ext_arch_temp.h"

#endif
