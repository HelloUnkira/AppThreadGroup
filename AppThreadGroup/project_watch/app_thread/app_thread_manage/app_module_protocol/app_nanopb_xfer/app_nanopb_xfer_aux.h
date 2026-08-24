#ifndef APP_NANOPB_XFER_AUX_H
#define APP_NANOPB_XFER_AUX_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输应答
 */
void app_nanopb_xfer_notify_ack(void);

/*@brief 打包传输扩展应答(双边流控通用)
 *@param type       应答消息类型(小协议=MsgSet oneof tag;文件=File子消息tag)
 *@param error_code 应答错误码(SUCCEED/CRC_FAILED/CHK_FAILED)
 *@param index      应答对象索引(文件分包拉取游标/重传索引)
 */
void app_nanopb_xfer_notify_ack_ext(uint16_t type, uint8_t error_code, uint16_t index);

/*@brief 传输接收应答
 */
bool app_nanopb_xfer_respond_ack(AppPB_MsgSet *message);

/*@brief 打包传输系统追踪日志文本
 */
void app_nanopb_xfer_notify_trace_text(void);

/*@brief 传输接收系统追踪日志文本
 */
bool app_nanopb_xfer_respond_trace_text(AppPB_MsgSet *message);

#endif

#endif
