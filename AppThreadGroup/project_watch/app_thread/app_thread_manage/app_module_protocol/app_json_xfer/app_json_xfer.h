#ifndef APP_JSON_XFER_H
#define APP_JSON_XFER_H

#if APP_MODULE_PROTOCOL_USE_JSON

#include "app_nanopb_set.pb.h"

/* 文件传输阶段:json元数据载荷,与nanopb子协议保持语义一致 */
#define APP_JSON_XFER_FILE_PHASE_START   0   /* 开始(描述符) */
#define APP_JSON_XFER_FILE_PHASE_END     1   /* 结束 */

/* JSON消息类型,与nanopb协议tag值保持一致 */
typedef enum {
    app_json_xfer_msg_is_default      = 0x0000,
    app_json_xfer_msg_is_ack          = 0x0001,
    app_json_xfer_msg_is_trace_text   = 0x0021,
    app_json_xfer_msg_is_device_info  = 0x0030,
    app_json_xfer_msg_is_device_param = 0x0031,
    app_json_xfer_msg_is_elec_card    = 0x0032,
    app_json_xfer_msg_is_system_clock = 0x0050,
    app_json_xfer_msg_is_world_clock  = 0x0051,
    app_json_xfer_msg_is_alarm        = 0x0052,
    app_json_xfer_msg_is_weather      = 0x0053,
    app_json_xfer_msg_is_heart_rate   = 0x0054,
    app_json_xfer_msg_is_music        = 0x0055,
    app_json_xfer_msg_is_msg_info     = 0x0056,
    app_json_xfer_msg_is_contact      = 0x0057,
    app_json_xfer_msg_is_sport_tgt    = 0x0058,
    app_json_xfer_msg_is_user_phys    = 0x0059,
    app_json_xfer_msg_is_motion_sum   = 0x005a,
    app_json_xfer_msg_is_sport_state  = 0x005b,
    app_json_xfer_msg_is_not_disturb  = 0x005c,
    app_json_xfer_msg_is_position     = 0x005d,
    app_json_xfer_msg_is_fem_cycle    = 0x005e,
    app_json_xfer_msg_is_account      = 0x005f,
    app_json_xfer_msg_is_sport_mng    = 0x0060,
    app_json_xfer_msg_is_sport_rcd    = 0x0061,
    app_json_xfer_msg_is_file         = 0x0080,
    app_json_xfer_msg_is_ota          = 0x0082,
} app_json_xfer_msg_type_t;

/*@brief 协议适配层,推送协议数据
 *@param channel 传输通道
 *@param json_object JSON对象
 *@retval 推送是否成功
 */
bool app_json_xfer_notify(app_module_transfer_chan_t channel, cJSON *json_object);

/*@brief 协议适配层,打包推送中间结构体消息
 *@param channel 传输通道
 *@param message nanopb中间结构体
 *@retval 推送是否成功
 */
bool app_json_xfer_notify_msg(app_module_transfer_chan_t channel, AppPB_MsgSet *message);

/*@brief 协议适配层,接收协议数据
 *@param json_stream JSON数据流
 *@retval 解析是否成功
 */
bool app_json_xfer_respond(uint8_t *json_stream);

/* 应答消息 */
void app_json_xfer_notify_ack(void);
bool app_json_xfer_respond_ack(AppPB_MsgSet *message);
void app_json_xfer_notify_trace_text(void);
bool app_json_xfer_respond_trace_text(AppPB_MsgSet *message);
/* 系统消息 */
void app_json_xfer_notify_device_info(void);
bool app_json_xfer_respond_device_info(AppPB_MsgSet *message);
void app_json_xfer_notify_device_param(void);
bool app_json_xfer_respond_device_param(AppPB_MsgSet *message);
void app_json_xfer_notify_elec_card(void);
bool app_json_xfer_respond_elec_card(AppPB_MsgSet *message);
/* 功能消息 */
void app_json_xfer_notify_system_clock(void);
bool app_json_xfer_respond_system_clock(AppPB_MsgSet *message);
void app_json_xfer_notify_world_clock(void);
bool app_json_xfer_respond_world_clock(AppPB_MsgSet *message);
void app_json_xfer_notify_alarm(void);
bool app_json_xfer_respond_alarm(AppPB_MsgSet *message);
void app_json_xfer_notify_weather(void);
bool app_json_xfer_respond_weather(AppPB_MsgSet *message);
void app_json_xfer_notify_heart_rate(void);
bool app_json_xfer_respond_heart_rate(AppPB_MsgSet *message);
void app_json_xfer_notify_music(void);
bool app_json_xfer_respond_music(AppPB_MsgSet *message);
void app_json_xfer_notify_msg_info(void);
bool app_json_xfer_respond_msg_info(AppPB_MsgSet *message);
void app_json_xfer_notify_contact(void);
bool app_json_xfer_respond_contact(AppPB_MsgSet *message);
void app_json_xfer_notify_sport_tgt(void);
bool app_json_xfer_respond_sport_tgt(AppPB_MsgSet *message);
void app_json_xfer_notify_user_phys(void);
bool app_json_xfer_respond_user_phys(AppPB_MsgSet *message);
void app_json_xfer_notify_motion_sum(void);
bool app_json_xfer_respond_motion_sum(AppPB_MsgSet *message);
void app_json_xfer_notify_sport_state(void);
bool app_json_xfer_respond_sport_state(AppPB_MsgSet *message);
void app_json_xfer_notify_not_disturb(void);
bool app_json_xfer_respond_not_disturb(AppPB_MsgSet *message);
void app_json_xfer_notify_position(void);
bool app_json_xfer_respond_position(AppPB_MsgSet *message);
void app_json_xfer_notify_fem_cycle(void);
bool app_json_xfer_respond_fem_cycle(AppPB_MsgSet *message);
void app_json_xfer_notify_account(void);
bool app_json_xfer_respond_account(AppPB_MsgSet *message);
void app_json_xfer_notify_sport_mng(void);
bool app_json_xfer_respond_sport_mng(AppPB_MsgSet *message);
void app_json_xfer_notify_sport_rcd(void);
bool app_json_xfer_respond_sport_rcd(AppPB_MsgSet *message);
/* 文件消息 */
void app_json_xfer_notify_file(void);
void app_json_xfer_file_step(void);
bool app_json_xfer_respond_file(cJSON *payload, uint32_t phase);
void app_json_xfer_notify_ota(void);
bool app_json_xfer_respond_ota(AppPB_MsgSet *message);

#endif

#endif
