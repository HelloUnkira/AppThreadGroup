#ifndef APP_NANOPB_XFER_H
#define APP_NANOPB_XFER_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 规定nanopb 缓存区最大尺寸 */
#define APP_NANOPB_BUFFER_SIZE  1024

/*@brief     协议适配层,推送协议数据
 *@param[in] message nanopb集合对象
 *@retval    推送是否成功
 */
bool app_nanopb_xfer_notify(AppPB_MsgSet *message);

/*@brief     协议适配层,接收协议数据
 *@param[in] buffer nanopb编码流
 *@param[in] size   nanopb编码流大小
 *@retval    解析是否成功
 */
bool app_nanopb_xfer_respond(uint8_t *buffer, uint32_t size);

/*@brief     协议适配层,接收协议数据
 *@param[in] buffer nanopb编码流
 *@param[in] size   nanopb编码流大小
 *@retval    解析是否成功
 */
bool app_nanopb_xfer_throw(uint8_t *buffer, uint32_t size);

/*@brief     协议适配层,派发协议数据
 *@param[in] message nanopb集合对象
 *@retval    派发是否成功
 */
bool app_nanopb_xfer_dispatch(AppPB_MsgSet *message);

#endif

#endif
