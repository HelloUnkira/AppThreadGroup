#ifndef APP_NANOPB_XFER_AUX_H
#define APP_NANOPB_XFER_AUX_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输应答
 */
void app_nanopb_xfer_notify_ack(void);

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
