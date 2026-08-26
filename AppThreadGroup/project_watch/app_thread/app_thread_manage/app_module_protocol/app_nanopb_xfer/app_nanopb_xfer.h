#ifndef APP_NANOPB_XFER_H
#define APP_NANOPB_XFER_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 协议适配层,整包crc8计算(清零编码后回填)
 *@param message nanopb集合对象
 *@retval 整包crc8值
 */
uint8_t app_nanopb_xfer_crc8_calc(AppPB_MsgSet *message);

/*@brief 协议适配层,发送协议数据(底端)
 *@param channel 传输信道
 *@param message nanopb集合对象
 *@retval 推送是否成功
 */
bool app_nanopb_xfer_notify_lower(app_module_transfer_chan_t channel, AppPB_MsgSet *message);

/*@brief 协议适配层,发送协议数据
 *@param node 协议发送节点
 *@retval 应答等待
 */
bool app_nanopb_xfer_notify(app_module_protocol_t *node);

/*@brief 协议适配层,接收协议数据
 *@param buffer nanopb编码流
 *@param size   nanopb编码流大小
 *@retval 解析是否成功
 */
bool app_nanopb_xfer_respond(uint8_t *buffer, uint32_t size);

#endif

#endif
