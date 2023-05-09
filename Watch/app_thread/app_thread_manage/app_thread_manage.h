#ifndef APP_THREAD_MANAGE_H
#define APP_THREAD_MANAGE_H

typedef enum {  /* 线程模组 */
    app_thread_manage_system = 0,  /* 系统模组 */
    app_thread_manage_dump,        /* 资源转储模组 */
    app_thread_manage_load,        /* 资源加载模组 */
    app_thread_manage_transfer,    /* JSON传输模组 */
} app_thread_manage_module_t;

typedef enum {  /* JSON传输模组事件 */
    app_thread_manage_transfer_notify,
    app_thread_manage_transfer_respond,
    app_thread_manage_transfer_throw,
    app_thread_manage_transfer_dispatch,
} app_thread_manage_transfer_event_t;

#endif
