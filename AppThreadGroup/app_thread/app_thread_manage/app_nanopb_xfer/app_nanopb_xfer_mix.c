/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_log_text.h"
#include "app_thread_group.h"
#include "app_module_transfer.h"
#include "app_module_protocol.h"
#include "app_module_clock.h"

#if       APP_MODULE_PROTOCOL_USE_NANOPB
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "app_nanopb_set.pb.h"
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"

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
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO("trace text:%s", message->payload.trace_text.trace_text);
    #endif
}

/*@brief 打包传输系统时钟
 */
void app_nanopb_xfer_notify_system_clock(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 数据打包 */
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_system_clock_tag,
        .payload.system_clock = {
            .year   = clock.year,
            .month  = clock.month,
            .day    = clock.day,
            .hour   = clock.hour,
            .minute = clock.minute,
            .second = clock.second,
            .zone   = clock.zone_sec,
            .mode   = clock.is_24 ? AppPB_SystemClock_Mode_IS_24 : AppPB_SystemClock_Mode_IS_12,
        },
    };
    /* 传输对象发送通知 */
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收系统时钟
 */
bool app_nanopb_xfer_respond_system_clock(AppPB_MsgSet *message)
{
    app_module_clock_t clock = {
        .year       = message->payload.system_clock.year,
        .month      = message->payload.system_clock.month,
        .day        = message->payload.system_clock.day,
        .hour       = message->payload.system_clock.hour,
        .minute     = message->payload.system_clock.minute,
        .second     = message->payload.system_clock.second,
        .zone_sec   = message->payload.system_clock.zone,
        .is_24      = message->payload.system_clock.mode,
    };
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO("system_clock.year:%u",    clock.year);
    APP_SYS_LOG_INFO("system_clock.month:%u",   clock.month);
    APP_SYS_LOG_INFO("system_clock.day:%u",     clock.day);
    APP_SYS_LOG_INFO("system_clock.hour:%u",    clock.hour);
    APP_SYS_LOG_INFO("system_clock.minute:%u",  clock.minute);
    APP_SYS_LOG_INFO("system_clock.second:%u",  clock.second);
    APP_SYS_LOG_INFO("system_clock.zone:%u",    clock.zone_sec);
    APP_SYS_LOG_INFO("system_clock.mode:%u",    clock.is_24);
    #endif
    app_module_clock_to_utc(&clock);
    app_module_clock_to_week(&clock);
    app_module_clock_set_system_clock(&clock);
    return true;
}

#endif
