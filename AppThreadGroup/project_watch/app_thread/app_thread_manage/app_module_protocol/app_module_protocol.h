#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

typedef enum {
    app_module_protocol_default = 0,
    app_module_protocol_trace_text,
    app_module_protocol_system_clock,
} app_module_protocol_notify_type_t;

typedef struct {
    struct {
        uint32_t type;      //传输类型
        uint32_t status;    //传输流程状态,内部约定
    } notify;
    struct {
        uint8_t *data;      //传输数据
        uint32_t size;      //传输数据大小
        uint64_t dynamic:1; //传输数据是否为动态
    } respond;
} app_module_protocol_t;

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_notify(app_module_protocol_t *protocol);

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_respond(app_module_protocol_t *protocol);

/*@brief 传输协议
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_notify_handler(uint8_t *data, uint32_t size);

/*@brief 传输协议
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_respond_handler(uint8_t *data, uint32_t size);

/*@brief 系统时钟模组初始化
 *       内部使用: 被namage线程使用
 */
void app_module_protocol_ready(void);

#endif
