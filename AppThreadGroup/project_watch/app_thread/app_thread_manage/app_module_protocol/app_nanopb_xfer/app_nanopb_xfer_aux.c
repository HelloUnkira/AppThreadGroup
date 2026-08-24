/*实现目标:
 *    nanopb协议-应答(aux)消息的打包与接收
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输扩展应答(双边流控通用)
 *       小协议应答:type=MsgSet oneof tag;文件应答:type=File子消息tag,index=拉取/重传游标
 */
void app_nanopb_xfer_notify_ack_ext(uint16_t type, uint8_t error_code, uint16_t index)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_ack_tag,
        .payload.ack = {
            .error_code = (AppPB_ACK_ErrorCode)error_code,
            .type       = type,
            .index      = index,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 打包传输应答
 */
void app_nanopb_xfer_notify_ack(void)
{
    app_nanopb_xfer_notify_ack_ext(0, AppPB_ACK_ErrorCode_SUCCEED, 0);
}

/*@brief 传输接收应答
 *       ack特殊:不回环,不产生应答;注入文件发送状态机与通用确认引擎
 */
bool app_nanopb_xfer_respond_ack(AppPB_MsgSet *message)
{
    AppPB_ACK *ack = &message->payload.ack;
    APP_SYS_LOG_INFO("ack.type:%u code:%u index:%u", ack->type, ack->error_code, ack->index);
    /* 注入文件发送状态机(文件传输内部拉取流程) */
    app_nanopb_xfer_file_ack(ack);
    /* 注入通用确认引擎(常规协议notify确认:消费ack→完成回调→推进下一notify) */
    app_nanopb_xfer_ctrl_respond_ack((uint16_t)ack->type, (uint8_t)ack->error_code, (uint16_t)ack->index);
    return true;
}

/*@brief 打包传输系统追踪日志文本
 *       单条发送:与protocol_test同构,由定时器驱动逐条发送;
 *       避免while循环一口气发空队列导致日志自增风暴(上一次实测风暴根因)
 */
void app_nanopb_xfer_notify_trace_text(void)
{
    /* 数据打包 */
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_trace_text_tag,
    };
    char *trace = message.payload.trace_text.trace_text;
    uint32_t used = 0;
    app_sys_log_text_peek_reset();
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
        if (used + item_len + 1 >= APP_SYS_LOG_TEXT_LIMIT)
            break;
        memcpy(trace + used, item, item_len);
        used += item_len;
        trace[used++] = '\n';
        trace[used] = '\0';
    }
    /* 无累积则结束 */
    if (used == 0)
        return;
    /* 传输对象发送通知 */
    app_nanopb_xfer_notify(app_module_transfer_chan_high, &message);
}

/*@brief 传输接收系统追踪日志文本
 */
bool app_nanopb_xfer_respond_trace_text(AppPB_MsgSet *message)
{
    APP_SYS_LOG_INFO("trace text:%s", message->payload.trace_text.trace_text);
    return true;
}

#endif
