#ifndef APP_NANOPB_XFER_FILE_H
#define APP_NANOPB_XFER_FILE_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 文件发送阶段 */
#define APP_NANOPB_XFER_FILE_PHASE_IDLE     0
#define APP_NANOPB_XFER_FILE_PHASE_DES      1
#define APP_NANOPB_XFER_FILE_PHASE_PKG      2
#define APP_NANOPB_XFER_FILE_PHASE_END      3
#define APP_NANOPB_XFER_FILE_PHASE_DONE     4

/* 文件传输实例 */
typedef struct {
    /* 发送上下文 */
    struct {
        uint32_t phase;     /* 发送阶段 */
        bool     active;
        uint8_t  ftype;
        uint8_t *fname;
        uint8_t  buffer[APP_MODULE_XFER_FILE_BUFFER_SIZE];
        uint32_t pkg_next;
        uint32_t file_size;
        uint8_t  file_crc8;
        uint8_t  file_cks8;
    } send;
    /* 接收上下文 */
    struct {
        bool     active;
        uint8_t  buffer[APP_MODULE_XFER_FILE_BUFFER_SIZE];
        uint32_t offset;
        uint16_t idx_last;
        uint32_t exp_size;
        uint8_t  exp_crc8;
        uint8_t  exp_cks8;
    } recv;
} app_nanopb_xfer_file_t;

/*@brief 文件ack到达
 *@param ok ack是否成功
 */
void app_nanopb_xfer_file_ack(bool ok);

/*@brief 文件ack超时
 */
void app_nanopb_xfer_file_timeout(void);

/*@brief 文件发送应答
 *@retval ack等待标记
 */
bool app_nanopb_xfer_notify_file(void);

/*@brief 传输接收应答
 *@param message 响应消息
 */
bool app_nanopb_xfer_respond_file(AppPB_MsgSet *message);

#endif

#endif