#ifndef APP_MODULE_TRANSFER_ADAPTOR_H
#define APP_MODULE_TRANSFER_ADAPTOR_H

/*@brief     协议适配层,接收协议数据
 *@param[in] data 数据流
 *@param[in] size 数据流大小
 */
void app_module_transfer_adaptor_rx(uint8_t *data, uint32_t size);

/*@brief     协议适配层,发送协议数据
 *@param[in] data 数据流
 *@param[in] size 数据流大小
 */
void app_module_transfer_adaptor_tx(uint8_t *data, uint32_t size);

/*@brief      协议适配层,发送协议数据最大大小
 *@retval[in] 数据流最大大小
 */
uint32_t app_module_transfer_adaptor_tx_max(void);

#endif
