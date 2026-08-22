/*实现目标:
 *    nanopb协议-文件(flie)消息的打包与接收
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输OTA升级
 */
void app_nanopb_xfer_notify_ota(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_ota_tag,
        .payload.ota = {
            .cmd             = 1,
            .state           = 0,
            .ready_cond      = 0,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收OTA升级
 */
bool app_nanopb_xfer_respond_ota(AppPB_MsgSet *message)
{
    AppPB_Ota *msg = &message->payload.ota;
    APP_SYS_LOG_INFO("ota.cmd:%u", msg->cmd);
    APP_SYS_LOG_INFO("ota.state:%u", msg->state);
    APP_SYS_LOG_INFO("ota.ready_cond:%u", msg->ready_cond);
    return true;
}

#endif
