/*实现目标:
 *    nanopb协议子部件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输应答
 *@param code 应答错误码(AppPB_ACK_Code)
 *@param info 请求语义(AppPB_ACK_Info)
 *@param msg  被应答的消息标识
 */
void app_nanopb_xfer_notify_ack_async(uint8_t code, uint8_t info, uint16_t msg)
{
    app_nanopb_xfer_ack_arg_t *arg = app_mem_alloc(sizeof(app_nanopb_xfer_ack_arg_t));
    arg->code = code; arg->info = info; arg->msg = msg;
    app_module_protocol_t protocol = {
        .type    = app_module_protocol_ack,
        .status  = 0,
        .data    = (uint8_t *)arg,
        .size    = sizeof(app_nanopb_xfer_ack_arg_t),
        .dynamic = true,
    };
    app_module_protocol_notify(&protocol, app_module_protocol_ack_priority);
}

/*@brief 打包传输同步读请求
 *@param type 同步类型(AppPB_Sync_Type)
 *@param msg  目标消息标识(传参:要启动的流程)
 */
void app_nanopb_xfer_notify_sync_async(uint8_t type, uint16_t msg)
{
    app_nanopb_xfer_sync_arg_t *arg = app_mem_alloc(sizeof(app_nanopb_xfer_sync_arg_t));
    arg->type = type; arg->msg = msg;
    app_module_protocol_t protocol = {
        .type    = app_module_protocol_sync,
        .status  = 0,
        .data    = (uint8_t *)arg,
        .size    = sizeof(app_nanopb_xfer_sync_arg_t),
        .dynamic = true,
    };
    app_module_protocol_notify(&protocol, app_module_protocol_sync_priority);
}

/*@brief 打包传输应答
 *@param code 应答错误码(AppPB_ACK_Code)
 *@param info 请求语义(AppPB_ACK_Info)
 *@param msg  被应答的消息标识
 */
void app_nanopb_xfer_notify_ack(uint8_t code, uint8_t info, uint16_t msg)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_ack_tag,
        .payload.ack = {
            .code = (AppPB_ACK_Code)code,
            .type = AppPB_ACK_Type_MINI_MSG,
            .info = (AppPB_ACK_Info)info,
            .msg  = msg,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收应答
 *@param message 响应消息
 */
bool app_nanopb_xfer_respond_ack(AppPB_MsgSet *message)
{
    AppPB_ACK *ack = &message->payload.ack;
    APP_SYS_LOG_INFO("ack.code:%u msg:%u", ack->code, ack->msg);
    
    /* 注入文件发送状态机(文件传输内部流程) */
    app_nanopb_xfer_file_ack(ack);
    /* 注入通用确认引擎(常规协议notify确认:消费ack→完成回调→推进下一notify) */
    app_nanopb_xfer_ctrl_respond_ack((uint16_t)ack->msg, (uint8_t)ack->code);
    return true;
}

/*@brief 打包传输同步读请求(系统指令)
 *@param type 同步类型(AppPB_Sync_Type)
 *@param msg  目标消息标识(传参:要启动的流程)
 */
void app_nanopb_xfer_notify_sync(uint8_t type, uint16_t msg)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_sync_tag,
        .payload.sync = {
            .type = (AppPB_Sync_Type)type,
            .msg  = msg,
        },
    };
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收同步读请求:回ack后按目标msg分派启动器
 *@param message 读请求
 *@retval 是否成功
 */
bool app_nanopb_xfer_respond_sync(AppPB_MsgSet *message)
{
    AppPB_Sync *sync = &message->payload.sync;
    APP_SYS_LOG_INFO("sync.type:%u msg:%u", sync->type, sync->msg);
    
    /* 收到sync:产生对应notify事件,交由协议层按目标启动流程 */
    app_module_protocol_t protocol = {.status = 0,};
    switch (sync->msg) {
    case AppPB_MsgSet_file_tag:
        protocol.type = app_module_protocol_file;
        app_module_protocol_notify(&protocol, 0);
        break;
    default:
        break;
    }
    return true;
}

#endif
