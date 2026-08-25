#ifndef APP_NANOPB_XFER_FLIE_H
#define APP_NANOPB_XFER_FLIE_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 初始化文件传输引擎(注入nanopb原语)
 */
void app_nanopb_xfer_file_init(void);

/*@brief 打包传输文件(日志上传:发描述符,等ack顺序推进分包,结束xfer)
 */
void app_nanopb_xfer_notify_file(void);

/*@brief 文件传输轮询步进
 */
void app_nanopb_xfer_file_step(void);

/*@brief 传输接收文件(开始/包/结束):委托通用引擎完成校验并回ack/断点
 */
bool app_nanopb_xfer_respond_file(AppPB_MsgSet *message);

/*@brief 传输接收应答注入(文件发送状态机)
 *@param ack 应答消息
 */
void app_nanopb_xfer_file_ack(AppPB_ACK *ack);

#endif

#endif