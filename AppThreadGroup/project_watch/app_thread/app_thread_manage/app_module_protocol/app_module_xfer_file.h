#ifndef APP_MODULE_XFER_FILE_H
#define APP_MODULE_XFER_FILE_H

/* 通用文件传输引擎:与具体序列化协议解耦
 * 文件类型/阶段/结果/子对象均为引擎自有抽象,协议层经原语注入并桥接打包/解析
 */

/* 文件传输公共常量 */
#define APP_MODULE_XFER_FILE_LIMIT_POLL     10     /* ack轮询超限(给足ack送达窗口) */
#define APP_MODULE_XFER_FILE_PERIOD_SEND    100    /* ack轮询周期 */
#define APP_MODULE_XFER_FILE_BUFFER_SIZE    1024   /* 文件内容缓冲大小 */
#define APP_MODULE_XFER_FILE_CHUNK_SIZE     224    /* 分包负载大小 */

/* 文件类型(协议层映射到具体字节流) */
typedef enum {
    APP_MODULE_XFER_FILE_TYPE_LOG = 0,  /* 日志 */
    APP_MODULE_XFER_FILE_TYPE_FW  = 1,  /* 固件 */
    APP_MODULE_XFER_FILE_TYPE_CWF = 2,  /* 云表盘 */
    APP_MODULE_XFER_FILE_TYPE_SWF = 3,  /* 息屏表盘 */
} app_module_xfer_file_type_t;

/* 传输阶段(协议层映射到传输字/子消息) */
typedef enum {
    APP_MODULE_XFER_FILE_STAGE_GO    = 0,  /* 传输通知 */
    APP_MODULE_XFER_FILE_STAGE_START = 1,  /* 传输开始 */
    APP_MODULE_XFER_FILE_STAGE_END   = 2,  /* 传输结束 */
} app_module_xfer_file_stage_t;

/* 传输结果码 */
typedef enum {
    APP_MODULE_XFER_FILE_RESULT_OK       = 0,  /* 成功 */
    APP_MODULE_XFER_FILE_RESULT_CHK_FAIL = 1,  /* 校验失败 */
    APP_MODULE_XFER_FILE_RESULT_OTHER    = 2,  /* 其他 */
} app_module_xfer_file_result_t;

/* 子对象(协议层映射到n号子消息,用于应答tag) */
typedef enum {
    APP_MODULE_XFER_FILE_SUB_DES  = 0,  /* 描述符 */
    APP_MODULE_XFER_FILE_SUB_PKG  = 1,  /* 分包 */
    APP_MODULE_XFER_FILE_SUB_XFER = 2,  /* 传输字 */
} app_module_xfer_file_sub_t;

/* 文件发送阶段 */
typedef enum {
    app_module_xfer_file_phase_start,   /* 已发描述符,等对端ack */
    app_module_xfer_file_phase_pkg,     /* 分包发送中 */
    app_module_xfer_file_phase_end,     /* 已发结束,等完成ack */
    app_module_xfer_file_phase_done,    /* 传输完成 */
} app_module_xfer_file_phase_t;

/* 文件发送状态机上下文 */
typedef struct {
    app_sys_timer_t timer;                             /* ack轮询定时器 */
    bool            active;                            /* 发送流程进行中 */
    uint8_t         image[APP_MODULE_XFER_FILE_BUFFER_SIZE]; /* 待发送文件内容 */
    uint32_t        file_size;                         /* 文件总大小 */
    uint8_t         file_crc8;                         /* 文件整体CRC8 */
    uint8_t         file_cks8;                         /* 文件整体Checksum8 */
    uint32_t        phase;                             /* 当前发送阶段 */
    uint8_t         ftype;                             /* 文件类型 */
    const char     *fname;                             /* 文件名(重发描述符用) */
    uint32_t        next_pkg;                          /* 下一待发分包序号 */
    uint32_t        retry;                             /* 当前包重发计数 */
    uint32_t        fb;                                /* 待消费的对端反馈 */
    uint16_t        fb_offset;                         /* 断点偏移 */
    uint32_t        poll_count;                        /* 未收反馈轮询计数 */
} app_module_xfer_file_send_t;

/* 文件接收上下文 */
typedef struct {
    bool     active;                                   /* 传输流程进行中 */
    uint8_t  buffer[APP_MODULE_XFER_FILE_BUFFER_SIZE]; /* 已组包文件内容 */
    uint32_t offset;                                   /* 已组包字节数 */
    uint16_t last_index;                               /* 期望下一分包索引 */
    uint32_t expect_size;                              /* 期望文件大小 */
    uint8_t  expect_crc8;                              /* 期望文件CRC8 */
    uint8_t  expect_cks8;                              /* 期望文件Checksum8 */
} app_module_xfer_file_recv_t;

/* 发送原语:由具体序列化协议注入(打包/解析桥接) */
typedef struct {
    uint16_t chunk;                                     /* 分包负载大小 */
    void (*send_des)(uint8_t type, const char *name, uint32_t size, uint8_t crc8, uint8_t cks8); /* 发描述符 */
    void (*send_pkg)(uint16_t index, uint32_t base, const uint8_t *data, uint16_t size); /* 发分包 */
    void (*send_xfer)(uint8_t stage, uint8_t result, uint32_t offset);                 /* 发传输字 */
    void (*send_state)(uint32_t offset); /* 发状态同步(期望续传位置) */
    void (*send_ack)(uint8_t result, uint8_t sub, bool repeat); /* 发应答(result=错误码,repeat=请求重发) */
    void (*post_step)(void);                            /* 投递轮询步进到manage线程 */
} app_module_xfer_file_ops_t;

/*@brief 注入文件传输引擎原语
 *@param ops 原语表
 */
void app_module_xfer_file_config(const app_module_xfer_file_ops_t *ops);

/*@brief 设置发送完成回调
 *@param done 回调
 */
void app_module_xfer_file_set_done(void (*done)(bool ok));

/*@brief 启动文件发送(发描述符,顺序分包,等ack推进;结束用xfer(END))
 *@param name  文件名
 *@param type  文件类型
 *@retval 是否启动
 */
bool app_module_xfer_file_notify(const char *name, app_module_xfer_file_type_t type);

/*@brief 发送轮询步进:消费对端反馈(ack/xfer断点)或超时退出 */
void app_module_xfer_file_step(void);

/*@brief 注入对端ack(发送状态机推进)
 *@param result 错误码
 */
void app_module_xfer_file_ack_respond(uint8_t result);

/*@brief 注入对端断点(发送状态机续传)
 *@param offset 断点偏移
 */
void app_module_xfer_file_xfer_respond(uint16_t offset);

/*@brief 注入对端状态同步(按期望续传位置续发)
 *@param offset 对端期望续传位置(已组包字节数)
 */
void app_module_xfer_file_state_respond(uint16_t offset);

/*@brief 接收描述符(协议层解析后注入)
 *@param type  文件类型
 *@param name  文件名
 *@param size  文件大小
 *@param crc8  文件整体CRC8
 *@param cks8  文件整体Checksum8
 *@retval true-接受并应回成功ack
 */
bool app_module_xfer_file_recv_start(uint8_t type, const char *name, uint32_t size, uint8_t crc8, uint8_t cks8);

/*@brief 接收分包(协议层解析后注入)
 *@param index 分包序号
 *@param base  分包偏移
 *@param data  分包数据
 *@param size  分包数据大小
 *@retval 去向: 1=推进并应回成功ack; 0=失败需发状态同步
 */
bool app_module_xfer_file_recv_pkg(uint16_t index, uint32_t base, const uint8_t *data, uint16_t size);

/*@brief 接收传输字(结束):整体校验
 *@param stage 传输阶段
 *@retval true-校验通过(应回成功ack)
 */
bool app_module_xfer_file_recv_xfer(uint8_t stage);

/*@brief 组装文件内容:日志条目换行拼接
 *@param image      内容缓冲
 *@param image_size 缓冲大小
 *@retval 文件大小
 */
uint32_t app_module_xfer_file_build(uint8_t *image, uint32_t image_size);

#endif