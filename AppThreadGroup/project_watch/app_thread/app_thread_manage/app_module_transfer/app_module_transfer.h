#ifndef APP_MODULE_TRANSFER_H
#define APP_MODULE_TRANSFER_H

/* 协议传输缓冲区最大块 */
#define APP_MODULE_TRANSFER_BLOCK   1024

/* 传输接收超时 */
#define APP_MODULE_TRANSFER_RESPOND_TIMEOUT 3000

/* 传输信道 */
/* 不同的信道对应不同的底层传输媒介 */
typedef enum {
    app_module_transfer_chan_low = 0,   /* 低速传输信道,默认信道 */
    app_module_transfer_chan_high,      /* 高速传输信道 */
    app_module_transfer_chan_num,
} app_module_transfer_chan_t;

#pragma pack(push, 1)
typedef struct {
    uint16_t chan:2;    /* 此包使用的传输信道 */
    uint16_t head:1;    /* 起始协议包(多协议包传输开始标记) */
    uint16_t tail:1;    /* 终止协议包(多协议包传输结束标记) */
    uint16_t size:12;   /* 动态数据流长度 */
    uint8_t  data[0];   /* 动态数据流 */
} app_module_transfer_t;
#pragma pack(pop)


/*@brief 接收协议数据并拆包发送
 *@param chan 传输信道
 *@param data 编码数据流
 *@param size 编码数据流大小
 */
bool app_module_transfer_notify(app_module_transfer_chan_t channel, uint8_t *data, uint32_t size);

/*@brief 接收协议数据并组包解析
 *@param data 编码数据流
 *@param size 编码数据流大小
 */
void app_module_transfer_respond(app_module_transfer_t *transfer);

#endif
