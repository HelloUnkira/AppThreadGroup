#ifndef APP_MODULE_TRANSFER_H
#define APP_MODULE_TRANSFER_H

#pragma pack(push, 1)
typedef struct {
    uint8_t package:1;  /* 协议包(1)或数据包(0) */
    uint8_t stream[0];  /* 流式数据(数据长度取决于该包实际大小) */
} app_module_transfer_pkg_t;
#pragma pack(pop)

/*@brief     协议适配层,接收协议数据
 *@param[in] tsf_pkg      流式传输包
 *@param[in] tsf_pkg_size 流式传输包大小
 */
bool app_module_transfer_respond(app_module_transfer_pkg_t *tsf_pkg, uint32_t tsf_pkg_size);

/*@brief     协议适配层,接收协议数据
 *@param[in] ptl_pkg      协议传输包
 *@param[in] ptl_pkg_size 协议传输包数据
 */
void app_module_transfer_respond_finish(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat);

/*@brief     协议适配层,发送协议数据
 *@param[in] tsf_pkg      流式传输包
 *@param[in] tsf_pkg_size 流式传输包大小
 */
void app_module_transfer_notify(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat);

#endif
