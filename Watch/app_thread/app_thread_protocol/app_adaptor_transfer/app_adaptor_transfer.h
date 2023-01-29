#ifndef APP_ADAPTOR_TRANSFER_H
#define APP_ADAPTOR_TRANSFER_H

/*@brief     协议适配层,接收协议数据
 *@param[in] ptl_pkg 动态协议包
 */
void app_adaptor_transfer_rx(app_module_protocol_package_t *ptl_pkg);

/*@brief     协议适配层,发送协议数据
 *@param[in] ptl_pkg 动态协议包
 */
void app_adaptor_transfer_tx(app_module_protocol_package_t *ptl_pkg);

#endif
