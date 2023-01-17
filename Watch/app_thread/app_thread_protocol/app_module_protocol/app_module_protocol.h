#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

typedef union {
    uint8_t bytes[0];       /* 流式协议包 */
    struct {
        #if 0               /* 小尺寸包: */
        uint16_t command;   /* 命令字 */
        uint16_t event;     /* 事件,关键字 */
        uint16_t count;     /* 包编号(1~n) */
        #else               /* 大尺寸包: */
        uint32_t command;   /* 命令字 */
        uint32_t event;     /* 事件,关键字 */
        uint32_t count;     /* 包编号(1~n) */
        #endif
        uint8_t  fake:1;    /* 虚假协议包(内部触发时使用) */
        uint8_t  head:1;    /* 起始协议包 */
        uint8_t  tail:1;    /* 终止协议包 */
        uint8_t  size;      /* 数据长度: */
                            /*     rx时为实际长度 */
                            /*     tx时为最大载荷 */
        uint8_t  data[0];   /* 数据实体 */
    };
} app_module_protocol_package_t;

typedef struct {
    /* 填充tx协议包 or 解析rx协议包 */
    void (*tx_pkg_make)( app_module_protocol_package_t *package);
    void (*rx_pkg_parse)(app_module_protocol_package_t *package);
} app_module_protocol_callback;

/*@brief  获得动态协议包可携带数据最大载荷
 *@retval 数据最大载荷
 */
uint32_t app_module_protocol_data_size(void);

/*@brief  获得动态协议包最大尺寸
 *@retval 动态协议包最大尺寸
 */
uint32_t app_module_protocol_package_size(void);

/*@brief     通知协议包
 *@param[in] pkg 动态协议包
 */
void app_module_protocol_notify(app_module_protocol_package_t *pkg);

/*@brief     响应协议包
 *@param[in] pkg 动态协议包
 */
void app_module_protocol_respond(app_module_protocol_package_t *pkg);

#endif
