/*实现目标:
 *    nanopb协议-应答(aux)消息的打包与接收
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 传输接收应答
 */
bool app_nanopb_xfer_respond_ack(AppPB_MsgSet *message)
{
    APP_SYS_LOG_INFO("ack.error_code:%u", message->payload.ack.error_code);
    return true;
}

/*@brief 打包传输应答
 */
void app_nanopb_xfer_notify_ack(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_ack_tag,
        .payload.ack = {
            .error_code = AppPB_ACK_ErrorCode_SUCCEED,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 打包传输系统追踪日志文本
 */
void app_nanopb_xfer_notify_trace_text(void)
{
    app_sys_log_text_peek_reset();
    while (true) {
        /* 数据打包 */
        AppPB_MsgSet message = {
            .which_payload = AppPB_MsgSet_trace_text_tag,
        };
        /* 循环提取日志信息 */
        app_sys_log_text_peek(message.payload.trace_text.trace_text);
        if (strlen(message.payload.trace_text.trace_text) == 0)
            break;
        /* 传输对象发送通知 */
        app_nanopb_xfer_notify(app_module_transfer_chan_high, &message);
    }
}

/*@brief 传输接收系统追踪日志文本
 */
bool app_nanopb_xfer_respond_trace_text(AppPB_MsgSet *message)
{
    APP_SYS_LOG_INFO("trace text:%s", message->payload.trace_text.trace_text);
    return true;
}

#endif
