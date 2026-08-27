/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 协议适配层,整包crc8计算(清零编码后回填)
 *@param message nanopb集合对象
 *@retval 整包crc8值
 */
uint8_t app_nanopb_xfer_crc8_calc(AppPB_MsgSet *message)
{
    size_t size1 = 0; message->crc8 = 0;
    /* 清零编码,对整包求crc8(整包统一校验) */
    if (!pb_get_encoded_size(&size1, AppPB_MsgSet_fields, message)) {
        APP_SYS_LOG_ERROR("encode size fail:%d", size1);
        return 0;
    }
    uint8_t *buffer = app_mem_alloc(size1);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size1);
    if (!pb_encode(&stream, AppPB_MsgSet_fields, message)) {
        APP_SYS_LOG_ERROR("encode fail:%s", stream.errmsg);
        app_mem_free(buffer);
        return 0;
    }
    uint8_t crc8 = app_sys_crc8(buffer, size1);
    message->crc8 = crc8;
    app_mem_free(buffer);
    return crc8;
}

/*@brief 协议适配层,发送协议数据(底端)
 *@param message nanopb集合对象
 *@retval 推送是否成功
 */
bool app_nanopb_xfer_notify_lower(app_module_transfer_chan_t channel, AppPB_MsgSet *message)
{
    size_t size = 0;
    app_nanopb_xfer_crc8_calc(message);
    if (!pb_get_encoded_size(&size, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("encode size fail:%d", size);
    
    /* nanopb编码 */
    uint8_t *buffer = app_mem_alloc(size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    if (!pb_encode(&stream, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("encode fail:%s", stream.errmsg);
    
    /* 检查nanopb数据流 */
    APP_SYS_LOG_INFO_RAW("nanopb encode:%d%s", size, app_sys_log_line());
    for (uint32_t idx = 0; idx < size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", buffer[idx]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    /* 传输nanopb数据流 */
    bool retval = app_module_transfer_notify(channel, buffer, size);
    /* 回收nanopb缓冲区 */
    app_mem_free(buffer);
    return retval;
}

/*@brief 协议适配层,发送协议数据
 *@param node 协议发送节点
 *@retval 应答等待
 */
bool app_nanopb_xfer_notify(app_module_protocol_t *node)
{
    switch (node->type) {
    case app_module_protocol_ack: {
        app_nanopb_xfer_ack_arg_t *arg = (void *)node->data;
        app_nanopb_xfer_notify_ack(arg->code, arg->info, arg->msg);
        return false;
    }
    case app_module_protocol_sync: {
        app_nanopb_xfer_sync_arg_t *arg = (void *)node->data;
        app_nanopb_xfer_notify_sync(arg->type, arg->msg);
        return true;
    }
    case app_module_protocol_file:
        return app_nanopb_xfer_notify_file();
    
    case app_module_protocol_device_info:
        app_nanopb_xfer_notify_device_info();
        return true;
    case app_module_protocol_device_param:
        app_nanopb_xfer_notify_device_param();
        return true;
    case app_module_protocol_elec_card:
        app_nanopb_xfer_notify_elec_card();
        return true;
    case app_module_protocol_system_clock:
        app_nanopb_xfer_notify_system_clock();
        return true;
    case app_module_protocol_world_clock:
        app_nanopb_xfer_notify_world_clock();
        return true;
    case app_module_protocol_alarm:
        app_nanopb_xfer_notify_alarm();
        return true;
    case app_module_protocol_weather:
        app_nanopb_xfer_notify_weather();
        return true;
    case app_module_protocol_heart_rate:
        app_nanopb_xfer_notify_heart_rate();
        return true;
    case app_module_protocol_music:
        app_nanopb_xfer_notify_music();
        return true;
    case app_module_protocol_msg_info:
        app_nanopb_xfer_notify_msg_info();
        return true;
    case app_module_protocol_contact:
        app_nanopb_xfer_notify_contact();
        return true;
    case app_module_protocol_sport_tgt:
        app_nanopb_xfer_notify_sport_tgt();
        return true;
    case app_module_protocol_user_phys:
        app_nanopb_xfer_notify_user_phys();
        return true;
    case app_module_protocol_motion_sum:
        app_nanopb_xfer_notify_motion_sum();
        return true;
    case app_module_protocol_sport_state:
        app_nanopb_xfer_notify_sport_state();
        return true;
    case app_module_protocol_not_disturb:
        app_nanopb_xfer_notify_not_disturb();
        return true;
    case app_module_protocol_position:
        app_nanopb_xfer_notify_position();
        return true;
    case app_module_protocol_fem_cycle:
        app_nanopb_xfer_notify_fem_cycle();
        return true;
    case app_module_protocol_account:
        app_nanopb_xfer_notify_account();
        return true;
    case app_module_protocol_sport_mng:
        app_nanopb_xfer_notify_sport_mng();
        return true;
    case app_module_protocol_sport_rcd:
        app_nanopb_xfer_notify_sport_rcd();
        return true;
    case app_module_protocol_display_info:
        app_nanopb_xfer_notify_display_info();
        return true;
    case app_module_protocol_app_bind:
        app_nanopb_xfer_notify_app_bind();
        return true;
    case app_module_protocol_power:
        app_nanopb_xfer_notify_power();
        return true;
    case app_module_protocol_sleep_set:
        app_nanopb_xfer_notify_sleep_set();
        return true;
    case app_module_protocol_watch_cfg:
        app_nanopb_xfer_notify_watch_cfg();
        return true;
    default:
        APP_SYS_ASSERT(false);
        return false;
    }
}

/*@brief 协议适配层,接收协议数据
 *@param buffer nanopb编码流
 *@param size   nanopb编码流大小
 *@retval 解析是否成功
 */
bool app_nanopb_xfer_respond(uint8_t *buffer, uint32_t size)
{
    /* 检查nanopb数据流 */
    APP_SYS_LOG_INFO_RAW("nanopb decode:%d%s", size, app_sys_log_line());
    for (uint32_t idx = 0; idx < size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", buffer[idx]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    /* nanopb解码 */
    AppPB_MsgSet message = AppPB_MsgSet_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);
    if (!pb_decode(&stream, AppPB_MsgSet_fields, &message))
         APP_SYS_LOG_ERROR("decode fail:%s", stream.errmsg);
    APP_SYS_LOG_INFO_RAW("nanopb type:%d%s", message.which_payload, app_sys_log_line());
    
    /* 整包统一校验 */
    uint8_t crc8 = message.crc8;
    if (app_nanopb_xfer_crc8_calc(&message) != crc8) {
        APP_SYS_LOG_ERROR("nanopb crc8 fail");
        return false;
    }
    
    /* 匹配数据包 */
    bool retval = false;
    switch (message.which_payload) {
    case AppPB_MsgSet_ack_tag:
        retval = app_nanopb_xfer_respond_ack(&message);
        break;
    case AppPB_MsgSet_sync_tag:
        retval = app_nanopb_xfer_respond_sync(&message);
        break;
    case AppPB_MsgSet_device_info_tag:
        retval = app_nanopb_xfer_respond_device_info(&message);
        break;
    case AppPB_MsgSet_device_param_tag:
        retval = app_nanopb_xfer_respond_device_param(&message);
        break;
    case AppPB_MsgSet_elec_card_tag:
        retval = app_nanopb_xfer_respond_elec_card(&message);
        break;
    case AppPB_MsgSet_system_clock_tag:
        retval = app_nanopb_xfer_respond_system_clock(&message);
        break;
    case AppPB_MsgSet_world_clock_tag:
        retval = app_nanopb_xfer_respond_world_clock(&message);
        break;
    case AppPB_MsgSet_alarm_tag:
        retval = app_nanopb_xfer_respond_alarm(&message);
        break;
    case AppPB_MsgSet_weather_tag:
        retval = app_nanopb_xfer_respond_weather(&message);
        break;
    case AppPB_MsgSet_heart_rate_tag:
        retval = app_nanopb_xfer_respond_heart_rate(&message);
        break;
    case AppPB_MsgSet_music_tag:
        retval = app_nanopb_xfer_respond_music(&message);
        break;
    case AppPB_MsgSet_msg_info_tag:
        retval = app_nanopb_xfer_respond_msg_info(&message);
        break;
    case AppPB_MsgSet_contact_tag:
        retval = app_nanopb_xfer_respond_contact(&message);
        break;
    case AppPB_MsgSet_sport_tgt_tag:
        retval = app_nanopb_xfer_respond_sport_tgt(&message);
        break;
    case AppPB_MsgSet_user_phys_tag:
        retval = app_nanopb_xfer_respond_user_phys(&message);
        break;
    case AppPB_MsgSet_motion_sum_tag:
        retval = app_nanopb_xfer_respond_motion_sum(&message);
        break;
    case AppPB_MsgSet_sport_state_tag:
        retval = app_nanopb_xfer_respond_sport_state(&message);
        break;
    case AppPB_MsgSet_not_disturb_tag:
        retval = app_nanopb_xfer_respond_not_disturb(&message);
        break;
    case AppPB_MsgSet_position_tag:
        retval = app_nanopb_xfer_respond_position(&message);
        break;
    case AppPB_MsgSet_fem_cycle_tag:
        retval = app_nanopb_xfer_respond_fem_cycle(&message);
        break;
    case AppPB_MsgSet_account_tag:
        retval = app_nanopb_xfer_respond_account(&message);
        break;
    case AppPB_MsgSet_sport_mng_tag:
        retval = app_nanopb_xfer_respond_sport_mng(&message);
        break;
    case AppPB_MsgSet_sport_rcd_tag:
        retval = app_nanopb_xfer_respond_sport_rcd(&message);
        break;
    case AppPB_MsgSet_display_info_tag:
        retval = app_nanopb_xfer_respond_display_info(&message);
        break;
    case AppPB_MsgSet_app_bind_tag:
        retval = app_nanopb_xfer_respond_app_bind(&message);
        break;
    case AppPB_MsgSet_power_tag:
        retval = app_nanopb_xfer_respond_power(&message);
        break;
    case AppPB_MsgSet_sleep_set_tag:
        retval = app_nanopb_xfer_respond_sleep_set(&message);
        break;
    case AppPB_MsgSet_watch_cfg_tag:
        retval = app_nanopb_xfer_respond_watch_cfg(&message);
        break;
    case AppPB_MsgSet_file_tag:
        retval = app_nanopb_xfer_respond_file(&message);
        break;
    default:
        APP_SYS_LOG_INFO_RAW("unknown nanopb type:%d%s", message.which_payload, app_sys_log_line());
        break;
    }
    
    /* 特殊消息不应答ACK,不响应回环 */
    if (message.which_payload == AppPB_MsgSet_ack_tag  ||
        message.which_payload == AppPB_MsgSet_file_tag)
        return retval;
    
    /* 协议自动应答ACK */
    if (retval) app_nanopb_xfer_notify_ack_async(AppPB_ACK_Code_SUCCEED, AppPB_ACK_Info_NO_QUESTION, message.which_payload);
    return retval;
}

#endif
