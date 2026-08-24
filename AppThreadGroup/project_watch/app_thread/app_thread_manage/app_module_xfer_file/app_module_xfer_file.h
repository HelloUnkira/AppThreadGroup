#ifndef APP_MODULE_XFER_FILE_H
#define APP_MODULE_XFER_FILE_H

#include "app_nanopb_set.pb.h"

/* 文件传输公共常量:json/nanopb共用 */
#define APP_MODULE_XFER_FILE_IMAGE_SIZE   1024   /* 文件内容缓冲大小 */
#define APP_MODULE_XFER_FILE_SEND_PERIOD  100    /* ack/分包轮询周期 */
#define APP_MODULE_XFER_FILE_POLL_MAX     3      /* ack轮询超限 */
#define APP_MODULE_XFER_FILE_CHUNK        224    /* 分包负载大小 */

/* 文件发送阶段(ack拉取状态机) */
typedef enum {
    app_module_xfer_file_phase_start = 0,  /* 已发描述符,等对端ack拉取 */
    app_module_xfer_file_phase_package,    /* 分包拉取应答中 */
    app_module_xfer_file_phase_end,        /* 已发结束,等完成ack */
    app_module_xfer_file_phase_done,       /* 传输完成 */
} app_module_xfer_file_phase_t;

/* 文件传输引擎原语:由具体序列化协议注入(动态部分) */
typedef struct {
    bool     meta_only;                    /* 元数据模式:只传描述符+结束(JSON) */
    uint16_t chunk;                        /* 分包负载大小 */
    void (*send_file)(AppPB_MsgSet *msg);  /* 发送文件子消息 */
    void (*send_ack)(uint16_t type, uint8_t code, uint16_t index); /* 发送ack */
    void (*post_step)(void);               /* 投递轮询/发包步进到manage线程 */
} app_module_xfer_file_ops_t;

/*@brief 注入文件传输引擎原语
 *@param ops 原语表
 */
void app_module_xfer_file_config(const app_module_xfer_file_ops_t *ops);

/*@brief 设置发送完成回调
 *@param done 回调
 */
void app_module_xfer_file_set_done(void (*done)(bool ok));

/*@brief 启动文件发送(拉取式:发描述符后等对端ack)
 *@param name 文件名
 *@retval 是否启动
 */
bool app_module_xfer_file_notify(const char *name);

/*@brief 文件发送轮询步进:消费ack拉取或超时退出 */
void app_module_xfer_file_step(void);

/*@brief 注入对端ack(发送状态机消费)
 *@param type  消息tag
 *@param code  错误码
 *@param index 索引
 */
void app_module_xfer_file_ack(uint16_t type, uint8_t code, uint16_t index);

/*@brief 传输接收文件(描述符/分包/结束):校验+回ack
 *@param file 文件子消息
 *@retval 是否成功
 */
bool app_module_xfer_file_respond(AppPB_File *file);

/*@brief 组装文件内容:日志条目换行拼接,无日志用样本兜底
 *@param image      内容缓冲
 *@param image_size 缓冲大小
 *@retval 文件大小
 */
uint32_t app_module_xfer_file_build(uint8_t *image, uint32_t image_size);

/*@brief 文件描述符元数据CRC8:name+utc64+crc32+size
 *@param des 描述符
 *@retval CRC8
 */
uint8_t app_module_xfer_file_descriptor_crc8(const AppPB_FileDes *des);

#endif