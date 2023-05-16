/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_protocol.h"
#include "app_module_clock.h"

#if       APP_MODULE_PROTOCOL_USE_NANOPB
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "app_pb_msg_set.pb.h"
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"

/*@brief 打包传输系统时钟
 */
void app_nanopb_xfer_notify_system_clock(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 数据打包 */
    AppPB_MsgSet message = {
        .type = AppPB_MsgTypeSet_AppPB_MsgType_Is_SystemClock,
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
    app_nanopb_xfer_notify(&message);
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
    app_module_clock_to_utc(&clock);
    app_module_clock_to_week(&clock);
    app_module_clock_set_system_clock(&clock);
    return true;
}

#endif
