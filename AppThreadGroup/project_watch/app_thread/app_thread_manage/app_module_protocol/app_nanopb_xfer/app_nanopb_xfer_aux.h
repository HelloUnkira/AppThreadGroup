#ifndef APP_NANOPB_XFER_AUX_H
#define APP_NANOPB_XFER_AUX_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* ack异步发送参数(经协议事件包携带) */
typedef struct {
    uint8_t  code;
    uint8_t  info;
    uint16_t msg;
} app_nanopb_xfer_ack_arg_t;

/* sync异步发送参数(经协议事件包携带) */
typedef struct {
    uint8_t  type;
    uint16_t msg;
} app_nanopb_xfer_sync_arg_t;

/*@brief 打包传输应答
 *@param code 应答错误码(AppPB_ACK_Code)
 *@param info 请求语义(AppPB_ACK_Info)
 *@param msg  被应答的消息标识
 */
void app_nanopb_xfer_notify_ack_async(uint8_t code, uint8_t info, uint16_t msg);

/*@brief 打包传输同步读请求
 *@param type 同步类型(AppPB_Sync_Type)
 *@param msg  目标消息标识(传参:要启动的流程)
 */
void app_nanopb_xfer_notify_sync_async(uint8_t type, uint16_t msg);

/*@brief 打包传输应答
 *@param code 应答错误码(AppPB_ACK_Code)
 *@param info 请求语义(AppPB_ACK_Info)
 *@param msg  被应答的消息标识
 */
void app_nanopb_xfer_notify_ack(uint8_t code, uint8_t info, uint16_t msg);

/*@brief 传输接收应答
 *@param message 响应消息
 */
bool app_nanopb_xfer_respond_ack(AppPB_MsgSet *message);

/*@brief 打包传输同步读请求(系统指令)
 *@param type 同步类型(AppPB_Sync_Type)
 *@param msg  目标消息标识(传参:要启动的流程)
 */
void app_nanopb_xfer_notify_sync(uint8_t type, uint16_t msg);

/*@brief 传输接收同步读请求:回ack后产生对应notify启动流程
 *@param message 读请求
 *@retval 是否成功
 */
bool app_nanopb_xfer_respond_sync(AppPB_MsgSet *message);

#endif

#endif
