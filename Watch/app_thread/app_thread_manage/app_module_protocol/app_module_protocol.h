#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

typedef enum {
    app_module_protocol_default = 0,
    app_module_protocol_system_clock,
} app_module_protocol_type;

/*@brief     传输协议
 *@param[in] type   传输类型
 *@param[in] status 传输流程状态,内部约定
 */
void app_module_protocol_notify(uint32_t type, uint32_t status);

#endif
