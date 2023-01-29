#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

#define APP_MODULE_PROTOCOL_TYPE_SIZE   10  /* k = log2(协议包命令及事件最大数量) + 1 */
#define APP_MODULE_PROTOCOL_DATA_SIZE   12  /* k = log2(协议包最大大小) + 1 */

#pragma pack(1)
typedef struct {
    uint64_t head:1;    /* 起始协议包(多包传输开始标记) */
    uint64_t tail:1;    /* 终止协议包(多包传输结束标记) */
    uint64_t offset:    APP_MODULE_PROTOCOL_DATA_SIZE;  /* 协议包数据偏移(多包传输使用) */
    uint64_t command:   APP_MODULE_PROTOCOL_TYPE_SIZE;  /* 命令字 */
    uint64_t event:     APP_MODULE_PROTOCOL_TYPE_SIZE;  /* 事件,关键字 */
    uint64_t size:      APP_MODULE_PROTOCOL_DATA_SIZE;  /* 协议包数据长度 */
    uint8_t *data;      /* 协议包数据实体 */
} app_module_protocol_package_t;
#pragma pack()

typedef struct {
    /* 填充tx协议包 or 解析rx协议包 */
    void (*tx_pkg_cb)(app_module_protocol_package_t *ptl_pkg);
    void (*rx_pkg_cb)(app_module_protocol_package_t *ptl_pkg);
} app_module_protocol_callback;

/*@brief     填充准备发送的协议包
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_tx(app_module_protocol_package_t *ptl_pkg);

/*@brief     解析已经接收的协议包
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_rx(app_module_protocol_package_t *ptl_pkg);

#endif
