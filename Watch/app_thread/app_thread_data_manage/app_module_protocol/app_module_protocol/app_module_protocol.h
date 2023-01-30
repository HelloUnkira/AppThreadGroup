#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

#define APP_MODULE_PROTOCOL_PKG_CNT_SIZE    10  /* k = log2(协议包最大数量) + 1 */
#define APP_MODULE_PROTOCOL_PKG_TYPE_SIZE   10  /* k = log2(协议包命令及事件最大数量) + 1 */
#define APP_MODULE_PROTOCOL_PKG_DATA_SIZE   12  /* k = log2(协议包最大大小) + 1 */

#pragma pack(1)

typedef struct {
    uint64_t head:1;    /* 起始协议包(多协议包传输开始标记) */
    uint64_t tail:1;    /* 终止协议包(多协议包传输结束标记) */
    uint64_t count:     APP_MODULE_PROTOCOL_PKG_CNT_SIZE;   /* 当前协议包编号(传输乱序检查) */
    uint64_t command:   APP_MODULE_PROTOCOL_PKG_TYPE_SIZE;  /* 命令字 */
    uint64_t event:     APP_MODULE_PROTOCOL_PKG_TYPE_SIZE;  /* 事件,关键字 */
    uint64_t size:      APP_MODULE_PROTOCOL_PKG_DATA_SIZE;  /* 协议包数据总长度 */
    uint64_t crc8:8;    /* 校验协议包数据,传输乱序检查 */
} app_module_protocol_pkg_t;

#pragma pack()

typedef struct {
    /* 填充tx协议包 or 解析rx协议包 */
    void (*rx_pkg_cb)(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat);
    void (*tx_pkg_cb)(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat);
} app_module_protocol_callback;

/*@brief     解析已经接收的协议包
 *@param[in] ptl_pkg 协议包
 *@param[in] ptl_dat 协议包数据
 */
void app_module_protocol_rx(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat);

/*@brief      填充准备发送的协议包
 *@param[in]  ptl_pkg 协议包
 *@param[out] ptl_dat 协议包数据
 */
void app_module_protocol_tx(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat);

#endif
