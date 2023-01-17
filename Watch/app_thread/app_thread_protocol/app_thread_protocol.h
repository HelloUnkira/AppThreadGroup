#ifndef APP_THREAD_PROTOCOL_H
#define APP_THREAD_PROTOCOL_H

typedef enum {  /* 线程模组 */
    app_thread_protocol_work,           /* 工作队列模组 */
    app_thread_protocol_transfer,       /* 传输模组 */
} app_thread_protocol_module;

typedef struct {    /* 工作队列模组数据项 */
    void (*routine)(void *parameter);   /* 数据处理例程 */
    void  *parameter;                   /* 数据处理例程参数 */
} app_thread_protocol_work_t;

typedef enum {
    app_thread_protocol_transfer_tx,
    app_thread_protocol_transfer_rx,
} app_thread_protocol_transfer_event;

#endif
