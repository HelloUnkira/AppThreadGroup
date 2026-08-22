/*实现目标:
 *    nanopb协议-系统(sys)消息的打包与接收
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输设备信息
 */
void app_nanopb_xfer_notify_device_info(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_device_info_tag,
        .payload.device_info = {
            .model   = "NAL-WB00",
            .hw_ver  = "V1.0",
            .sw_ver  = "V1.0",
            .sn      = "AT3380123",
            .bt_addr = "11:22:33:44:55:66",
            .pid     = "AT338",
            .battery = 85,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收设备信息
 */
bool app_nanopb_xfer_respond_device_info(AppPB_MsgSet *message)
{
    AppPB_DevInfo *msg = &message->payload.device_info;
    APP_SYS_LOG_INFO("device_info.model:%s", msg->model);
    APP_SYS_LOG_INFO("device_info.hw_ver:%s", msg->hw_ver);
    APP_SYS_LOG_INFO("device_info.sw_ver:%s", msg->sw_ver);
    APP_SYS_LOG_INFO("device_info.sn:%s", msg->sn);
    APP_SYS_LOG_INFO("device_info.bt_addr:%s", msg->bt_addr);
    APP_SYS_LOG_INFO("device_info.pid:%s", msg->pid);
    APP_SYS_LOG_INFO("device_info.battery:%u", msg->battery);
    return true;
}

/*@brief 打包传输设备参数
 */
void app_nanopb_xfer_notify_device_param(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_device_param_tag,
        .payload.device_param = {
            .lang_id = 0,
            .zone    = 8 * 3600,
            .is_12h  = 0,
            .is_mi   = 0,
            .brt     = 50,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收设备参数
 */
bool app_nanopb_xfer_respond_device_param(AppPB_MsgSet *message)
{
    AppPB_DevParam *msg = &message->payload.device_param;
    APP_SYS_LOG_INFO("device_param.lang_id:%u", msg->lang_id);
    APP_SYS_LOG_INFO("device_param.zone:%u", msg->zone);
    APP_SYS_LOG_INFO("device_param.is_12h:%u", msg->is_12h);
    APP_SYS_LOG_INFO("device_param.is_mi:%u", msg->is_mi);
    APP_SYS_LOG_INFO("device_param.brt:%u", msg->brt);
    return true;
}

/*@brief 打包传输电子保卡
 */
void app_nanopb_xfer_notify_elec_card(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_elec_card_tag,
        .payload.elec_card = {
            .is_activate = true,
            .is_reported = false,
        },
    };
    app_nanopb_xfer_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收电子保卡
 */
bool app_nanopb_xfer_respond_elec_card(AppPB_MsgSet *message)
{
    AppPB_ElecCard *msg = &message->payload.elec_card;
    APP_SYS_LOG_INFO("elec_card.is_activate:%u", msg->is_activate);
    APP_SYS_LOG_INFO("elec_card.is_reported:%u", msg->is_reported);
    return true;
}

#endif
