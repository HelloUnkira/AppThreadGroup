#ifndef APP_THREAD_DATA_CENTER_H
#define APP_THREAD_DATA_CENTER_H

typedef enum {  /* 线程模组 */
    app_thread_data_manage_system = 0,  /* 系统模组 */
    app_thread_data_manage_protocol,    /* 协议传输模组 */
    app_thread_data_manage_transfer,    /* 数据流传输模组 */
    app_thread_data_manage_dump,        /* 资源转储模组 */
    app_thread_data_manage_load,        /* 资源加载模组 */
} app_thread_data_manage_module_t;

typedef enum {  /* 协议传输模组事件 */
    app_thread_data_manage_protocol_rx,
    app_thread_data_manage_protocol_tx,
} app_thread_data_manage_protocol_event_t;

typedef enum {  /* 数据流传输模组事件 */
    app_thread_data_manage_transfer_rx,
    app_thread_data_manage_transfer_tx,
} app_thread_data_manage_transfer_event_t;

#endif
