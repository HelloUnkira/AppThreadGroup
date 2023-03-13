#ifndef APP_ARCH_DRIVER_H
#define APP_ARCH_DRIVER_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *api;    /* 设备操作集 */
    const void *cfg;    /* 设备操作集参数 */
} app_driver_t;

#endif
