#ifndef APP_THREAD_MANAGE_H
#define APP_THREAD_MANAGE_H

/* 线程模组 */
typedef enum {
    app_thread_manage_none = 0,    /* 系统模组,占位符(线程组公共事件) */
    app_thread_manage_protocol,    /* 协议传输模组 */
    app_thread_manage_transfer,    /* 协议传输模组 */
} app_thread_manage_module_t;

/* 线程模组事件 */
typedef enum {
    /* 协议传输模组事件 */
    app_thread_manage_protocol_notify,
    app_thread_manage_protocol_respond,
    /* 协议传输模组事件 */
    app_thread_manage_transfer_notify,
    app_thread_manage_transfer_respond,
    /*  */
} app_thread_manage_module_event_t;

/* 序列化及反序列化传输协议 */
#define APP_MODULE_PROTOCOL_USE_JSON            0
#define APP_MODULE_PROTOCOL_USE_NANOPB          1
/* 本地回环:用于测试数据通路的上层环路 */
#define APP_MODULE_PROTOCOL_LOCAL_LOOPBACK      1

/* 子线程模组 */
#include "app_module_transfer.h"
#include "app_module_protocol.h"

#endif
