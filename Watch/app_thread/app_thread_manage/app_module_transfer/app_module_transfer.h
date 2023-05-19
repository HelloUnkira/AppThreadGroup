#ifndef APP_MODULE_TRANSFER_H
#define APP_MODULE_TRANSFER_H

/* 协议传输缓冲区最大块 */
#define APP_MODULE_TRANSFER_BLOCK   4096

#pragma pack(push, 1)
typedef struct {
    uint16_t head:1;    /* 起始协议包(多协议包传输开始标记) */
    uint16_t tail:1;    /* 终止协议包(多协议包传输结束标记) */
    uint16_t size:12;   /* 动态数据流长度 */
    uint8_t  data[0];   /* 动态数据流 */
} app_module_transfer_t;
#pragma pack(pop)

/*@brief     接收协议数据并拆包发送
 *@param[in] data 编码数据流
 *@param[in] size 编码数据流大小
 */
void app_module_transfer_notify(uint8_t *data, uint32_t size);

/*@brief     接收协议数据并组包解析
 *@param[in] data 编码数据流
 *@param[in] size 编码数据流大小
 */
void app_module_transfer_respond(app_module_transfer_t *transfer);

#endif
