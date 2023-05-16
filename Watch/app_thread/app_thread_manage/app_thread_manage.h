#ifndef APP_THREAD_MANAGE_H
#define APP_THREAD_MANAGE_H

typedef enum {  /* 线程模组 */
    app_thread_manage_system = 0,  /* 系统模组 */
    app_thread_manage_dump,        /* 资源转储模组 */
    app_thread_manage_load,        /* 资源加载模组 */
    app_thread_manage_protocol,    /* 协议传输模组 */
} app_thread_manage_module_t;

typedef enum {  /* 协议传输模组事件 */
    app_thread_manage_protocol_notify,
    app_thread_manage_protocol_respond,
} app_thread_manage_protocol_event_t;

/* 序列化及反序列化传输协议 */
#define APP_MODULE_PROTOCOL_USE_JSON            0
#define APP_MODULE_PROTOCOL_USE_NANOPB          1
/* 本地回环:用于测试数据通路的上层环路 */
#define APP_MODULE_PROTOCOL_LOCAL_LOOPBACK      1

#endif
