#ifndef APP_EXT_ARCH_DEV_H
#define APP_EXT_ARCH_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *api;    /* 设备操作集 */
    const void *cfg;    /* 设备操作集参数 */
} app_dev_t;

#endif
