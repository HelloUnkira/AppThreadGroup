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
 *       累积多条日志条目到一个负载,尽量填满传输效率
 */
void app_nanopb_xfer_notify_trace_text(void)
{
    app_sys_log_text_peek_reset();
    while (true) {
        /* 数据打包 */
        AppPB_MsgSet message = {
            .which_payload = AppPB_MsgSet_trace_text_tag,
        };
        char *trace = message.payload.trace_text.trace_text;
        uint32_t used = 0;
        /* 循环提取日志信息并累积到负载 */
        while (true) {
            char item[APP_SYS_LOG_TEXT_LIMIT + 1] = {0};
            /* 无更多日志条目则结束 */
            if (!app_sys_log_text_peek(item))
                break;
            /* 空日志条目代表已到末尾 */
            if (item[0] == '\0')
                break;
            /* 放不下下一条则结束当前负载(该条已窥探消耗) */
            uint32_t item_len = strlen(item);
            if (used + item_len >= APP_SYS_LOG_TEXT_LIMIT)
                break;
            memcpy(trace + used, item, item_len);
            used += item_len;
            trace[used] = '\0';
        }
        /* 无累积则结束 */
        if (used == 0)
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
