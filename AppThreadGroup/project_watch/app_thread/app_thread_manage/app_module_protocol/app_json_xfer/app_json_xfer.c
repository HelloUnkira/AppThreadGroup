/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_JSON

/*@brief 协议适配层,推送协议数据
 *@param json_object JSON对象
 *@retval 推送是否成功
 */
bool app_json_xfer_notify(app_module_transfer_chan_t channel, cJSON *json_object)
{
    /* 获得JSON对象 */
    cJSON *json_item = json_object;
    /* 将JSON对象转为JSON数据流 */
    char *json_stream = cJSON_Print(json_item);
    /* 检查JSON */
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO_RAW("json:%d%s", strlen(json_stream), app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("%s%s", json_stream, app_sys_log_line());
    /* 压缩JSON数据流 */
    cJSON_Minify(json_stream);
    /* 检查JSON */
    APP_SYS_LOG_INFO_RAW("json minify:%d%s", strlen(json_stream), app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("%s%s", json_stream, app_sys_log_line());
    /* 传输JSON数据流 */
    app_module_transfer_notify(channel, json_stream, strlen(json_stream));
    /* 回收JSON对象 */
    cJSON_free(json_stream);
    return true;
}

/*@brief 协议适配层,打包推送中间结构体消息
 *@param channel 传输通道
 *@param message nanopb中间结构体
 *@retval 推送是否成功
 */
bool app_json_xfer_notify_msg(app_module_transfer_chan_t channel, AppPB_MsgSet *message)
{
    /* 创建传输对象 */
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_item, "type", message->which_payload);
    /* 匹配消息包 */
    cJSON *payload = NULL;
    switch (message->which_payload) {
    case AppPB_MsgSet_ack_tag:
        payload = app_json_xfer_pack_ack(&message->payload.ack);
        break;
    case AppPB_MsgSet_trace_text_tag:
        payload = app_json_xfer_pack_trace_txt(&message->payload.trace_text);
        break;
    case AppPB_MsgSet_device_info_tag:
        payload = app_json_xfer_pack_dev_info(&message->payload.device_info);
        break;
    case AppPB_MsgSet_device_param_tag:
        payload = app_json_xfer_pack_dev_param(&message->payload.device_param);
        break;
    case AppPB_MsgSet_elec_card_tag:
        payload = app_json_xfer_pack_elec_card(&message->payload.elec_card);
        break;
    case AppPB_MsgSet_system_clock_tag:
        payload = app_json_xfer_pack_sys_clock(&message->payload.system_clock);
        break;
    case AppPB_MsgSet_world_clock_tag:
        payload = app_json_xfer_pack_world_clock(&message->payload.world_clock);
        break;
    case AppPB_MsgSet_alarm_tag:
        payload = app_json_xfer_pack_alarm(&message->payload.alarm);
        break;
    case AppPB_MsgSet_weather_tag:
        payload = app_json_xfer_pack_weather(&message->payload.weather);
        break;
    case AppPB_MsgSet_heart_rate_tag:
        payload = app_json_xfer_pack_heart_rate(&message->payload.heart_rate);
        break;
    case AppPB_MsgSet_music_tag:
        payload = app_json_xfer_pack_music(&message->payload.music);
        break;
    case AppPB_MsgSet_msg_info_tag:
        payload = app_json_xfer_pack_msg_info(&message->payload.msg_info);
        break;
    case AppPB_MsgSet_contact_tag:
        payload = app_json_xfer_pack_contact(&message->payload.contact);
        break;
    case AppPB_MsgSet_sport_tgt_tag:
        payload = app_json_xfer_pack_sport_tgt(&message->payload.sport_tgt);
        break;
    case AppPB_MsgSet_user_phys_tag:
        payload = app_json_xfer_pack_user_phys(&message->payload.user_phys);
        break;
    case AppPB_MsgSet_motion_sum_tag:
        payload = app_json_xfer_pack_motion_sum(&message->payload.motion_sum);
        break;
    case AppPB_MsgSet_sport_state_tag:
        payload = app_json_xfer_pack_sport_state(&message->payload.sport_state);
        break;
    case AppPB_MsgSet_not_disturb_tag:
        payload = app_json_xfer_pack_not_disturb(&message->payload.not_disturb);
        break;
    case AppPB_MsgSet_position_tag:
        payload = app_json_xfer_pack_position(&message->payload.position);
        break;
    case AppPB_MsgSet_fem_cycle_tag:
        payload = app_json_xfer_pack_fem_cycle(&message->payload.fem_cycle);
        break;
    case AppPB_MsgSet_account_tag:
        payload = app_json_xfer_pack_account(&message->payload.account);
        break;
    case AppPB_MsgSet_sport_mng_tag:
        payload = app_json_xfer_pack_sport_mng(&message->payload.sport_mng);
        break;
    case AppPB_MsgSet_sport_rcd_tag:
        payload = app_json_xfer_pack_sport_rcd(&message->payload.sport_rcd);
        break;
    case AppPB_MsgSet_ota_tag:
        payload = app_json_xfer_pack_ota(&message->payload.ota);
        break;
    default:
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", message->which_payload);
        break;
    }
    if (payload == NULL) {
        cJSON_Delete(json_item);
        return false;
    }
    /* 挂载消息包 */
    cJSON_AddItemToObject(json_item, "payload", payload);
    /* 传输对象发送通知 */
    app_json_xfer_notify(channel, json_item);
    /* 销毁传输对象 */
    cJSON_Delete(json_item);
    return true;
}

/*@brief 协议适配层,接收协议数据
 *@param json_stream JSON数据流
 *@retval 解析是否成功
 */
bool app_json_xfer_respond(uint8_t *json_stream)
{
    /* 解析JSON */
    cJSON *json_item = cJSON_Parse(json_stream);
    if (json_item == NULL) {
        char *error = cJSON_GetErrorPtr();
        APP_SYS_LOG_ERROR("");
        APP_SYS_LOG_ERROR_RAW("error:%s%s", error, app_sys_log_line());
        APP_SYS_LOG_ERROR_RAW("%s%s", json_stream, app_sys_log_line());
        return false;
    }
    /* 检查JSON */
    char *json_format = cJSON_Print(json_item);
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO_RAW("json:%d%s", strlen(json_stream), app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("%s%s", json_format, app_sys_log_line());
    cJSON_free(json_format);
    /* 解析JSON */
    uint32_t type = cJSON_GetNumberValue(cJSON_GetObjectItem(json_item, "type"));
    cJSON *payload = cJSON_GetObjectItem(json_item, "payload");
    /* 检查JSON */
    APP_SYS_LOG_INFO_RAW("json type:%d%s", type, app_sys_log_line());
    /* 匹配数据包 */
    AppPB_MsgSet message = AppPB_MsgSet_init_zero;
    bool retval = false;
    switch (type) {
    case app_json_xfer_msg_is_ack:
        if (app_json_xfer_unpack_ack(payload, &message.payload.ack))
            retval = app_json_xfer_respond_ack(&message);
        break;
    case app_json_xfer_msg_is_trace_text:
        if (app_json_xfer_unpack_trace_txt(payload, &message.payload.trace_text))
            retval = app_json_xfer_respond_trace_text(&message);
        break;
    case app_json_xfer_msg_is_device_info:
        if (app_json_xfer_unpack_dev_info(payload, &message.payload.device_info))
            retval = app_json_xfer_respond_device_info(&message);
        break;
    case app_json_xfer_msg_is_device_param:
        if (app_json_xfer_unpack_dev_param(payload, &message.payload.device_param))
            retval = app_json_xfer_respond_device_param(&message);
        break;
    case app_json_xfer_msg_is_elec_card:
        if (app_json_xfer_unpack_elec_card(payload, &message.payload.elec_card))
            retval = app_json_xfer_respond_elec_card(&message);
        break;
    case app_json_xfer_msg_is_system_clock:
        if (app_json_xfer_unpack_sys_clock(payload, &message.payload.system_clock))
            retval = app_json_xfer_respond_system_clock(&message);
        break;
    case app_json_xfer_msg_is_world_clock:
        if (app_json_xfer_unpack_world_clock(payload, &message.payload.world_clock))
            retval = app_json_xfer_respond_world_clock(&message);
        break;
    case app_json_xfer_msg_is_alarm:
        if (app_json_xfer_unpack_alarm(payload, &message.payload.alarm))
            retval = app_json_xfer_respond_alarm(&message);
        break;
    case app_json_xfer_msg_is_weather:
        if (app_json_xfer_unpack_weather(payload, &message.payload.weather))
            retval = app_json_xfer_respond_weather(&message);
        break;
    case app_json_xfer_msg_is_heart_rate:
        if (app_json_xfer_unpack_heart_rate(payload, &message.payload.heart_rate))
            retval = app_json_xfer_respond_heart_rate(&message);
        break;
    case app_json_xfer_msg_is_music:
        if (app_json_xfer_unpack_music(payload, &message.payload.music))
            retval = app_json_xfer_respond_music(&message);
        break;
    case app_json_xfer_msg_is_msg_info:
        if (app_json_xfer_unpack_msg_info(payload, &message.payload.msg_info))
            retval = app_json_xfer_respond_msg_info(&message);
        break;
    case app_json_xfer_msg_is_contact:
        if (app_json_xfer_unpack_contact(payload, &message.payload.contact))
            retval = app_json_xfer_respond_contact(&message);
        break;
    case app_json_xfer_msg_is_sport_tgt:
        if (app_json_xfer_unpack_sport_tgt(payload, &message.payload.sport_tgt))
            retval = app_json_xfer_respond_sport_tgt(&message);
        break;
    case app_json_xfer_msg_is_user_phys:
        if (app_json_xfer_unpack_user_phys(payload, &message.payload.user_phys))
            retval = app_json_xfer_respond_user_phys(&message);
        break;
    case app_json_xfer_msg_is_motion_sum:
        if (app_json_xfer_unpack_motion_sum(payload, &message.payload.motion_sum))
            retval = app_json_xfer_respond_motion_sum(&message);
        break;
    case app_json_xfer_msg_is_sport_state:
        if (app_json_xfer_unpack_sport_state(payload, &message.payload.sport_state))
            retval = app_json_xfer_respond_sport_state(&message);
        break;
    case app_json_xfer_msg_is_not_disturb:
        if (app_json_xfer_unpack_not_disturb(payload, &message.payload.not_disturb))
            retval = app_json_xfer_respond_not_disturb(&message);
        break;
    case app_json_xfer_msg_is_position:
        if (app_json_xfer_unpack_position(payload, &message.payload.position))
            retval = app_json_xfer_respond_position(&message);
        break;
    case app_json_xfer_msg_is_fem_cycle:
        if (app_json_xfer_unpack_fem_cycle(payload, &message.payload.fem_cycle))
            retval = app_json_xfer_respond_fem_cycle(&message);
        break;
    case app_json_xfer_msg_is_account:
        if (app_json_xfer_unpack_account(payload, &message.payload.account))
            retval = app_json_xfer_respond_account(&message);
        break;
    case app_json_xfer_msg_is_sport_mng:
        if (app_json_xfer_unpack_sport_mng(payload, &message.payload.sport_mng))
            retval = app_json_xfer_respond_sport_mng(&message);
        break;
    case app_json_xfer_msg_is_sport_rcd:
        if (app_json_xfer_unpack_sport_rcd(payload, &message.payload.sport_rcd))
            retval = app_json_xfer_respond_sport_rcd(&message);
        break;
    case app_json_xfer_msg_is_file: {
        uint32_t phase = (uint32_t)cJSON_GetNumberValue(cJSON_GetObjectItem(json_item, "phase"));
        retval = app_json_xfer_respond_file(payload, phase);
        break;
    }
    case app_json_xfer_msg_is_ota:
        if (app_json_xfer_unpack_ota(payload, &message.payload.ota))
            retval = app_json_xfer_respond_ota(&message);
        break;
    default:
        APP_SYS_LOG_INFO_RAW("unknown json type:%d%s", type, app_sys_log_line());
        break;
    }
    /* 回收JSON对象 */
    cJSON_Delete(json_item);
    return retval;
}

#endif
