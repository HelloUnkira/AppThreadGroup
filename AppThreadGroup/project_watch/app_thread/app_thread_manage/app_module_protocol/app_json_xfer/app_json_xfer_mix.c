/*实现目标:
 *    json协议-各分类消息的打包与接收
 *    复用AppPB_MsgSet中间结构体,由codegen生成的pack/unpack层做json序列化
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_JSON

/*@brief 打包推送文件传输阶段消息
 *       {"type":0x0080,"phase":0/1,"payload":<文件描述符/结束>}
 *@param message 装载的文件子消息
 *@param phase   文件传输阶段
 */
static void app_json_xfer_notify_file_phase(AppPB_MsgSet *message, uint32_t phase)
{
    /* 创建传输对象 */
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_item, "type", message->which_payload);
    cJSON_AddNumberToObject(json_item, "phase", phase);
    /* 匹配子消息并转换负载 */
    cJSON *payload = NULL;
    if (message->payload.file.which_payload == AppPB_File_descriptor_tag)
        payload = app_json_xfer_pack_file_des(&message->payload.file.payload.descriptor);
    else if (message->payload.file.which_payload == AppPB_File_done_tag)
        payload = app_json_xfer_pack_file_end(&message->payload.file.payload.done);
    if (payload == NULL) {
        cJSON_Delete(json_item);
        return;
    }
    cJSON_AddItemToObject(json_item, "payload", payload);
    /* 传输对象发送通知 */
    app_json_xfer_notify(app_module_transfer_chan_high, json_item);
    /* 销毁传输对象 */
    cJSON_Delete(json_item);
}

/*@brief 传输接收应答
 */
bool app_json_xfer_respond_ack(AppPB_MsgSet *message)
{
    APP_SYS_LOG_INFO("ack.error_code:%u", message->payload.ack.error_code);
    return true;
}

/*@brief 打包传输应答
 */
void app_json_xfer_notify_ack(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_ack_tag,
        .payload.ack = {
            .error_code = AppPB_ACK_ErrorCode_SUCCEED,
        },
    };
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 打包传输系统追踪日志文本
 *       累积多条日志条目到一个负载,尽量填满传输效率
 */
void app_json_xfer_notify_trace_text(void)
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
            if (used + item_len + 1 >= APP_SYS_LOG_TEXT_LIMIT)
                break;
            memcpy(trace + used, item, item_len);
            used += item_len;
            trace[used++] = '\n';
            trace[used] = '\0';
        }
        /* 无累积则结束 */
        if (used == 0)
            break;
        /* 传输对象发送通知 */
        app_json_xfer_notify_msg(app_module_transfer_chan_high, &message);
    }
}

/*@brief 传输接收系统追踪日志文本
 */
bool app_json_xfer_respond_trace_text(AppPB_MsgSet *message)
{
    APP_SYS_LOG_INFO("trace text:%s", message->payload.trace_text.trace_text);
    return true;
}

/*@brief 打包传输设备信息
 */
void app_json_xfer_notify_device_info(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_device_info_tag;
    app_module_protocol_test_fill_device_info(&message.payload.device_info);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收设备信息
 */
bool app_json_xfer_respond_device_info(AppPB_MsgSet *message)
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
void app_json_xfer_notify_device_param(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_device_param_tag;
    app_module_protocol_test_fill_device_param(&message.payload.device_param);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收设备参数
 */
bool app_json_xfer_respond_device_param(AppPB_MsgSet *message)
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
void app_json_xfer_notify_elec_card(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_elec_card_tag;
    app_module_protocol_test_fill_elec_card(&message.payload.elec_card);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收电子保卡
 */
bool app_json_xfer_respond_elec_card(AppPB_MsgSet *message)
{
    AppPB_ElecCard *msg = &message->payload.elec_card;
    APP_SYS_LOG_INFO("elec_card.is_activate:%u", msg->is_activate);
    APP_SYS_LOG_INFO("elec_card.is_reported:%u", msg->is_reported);
    return true;
}

/*@brief 打包传输系统时钟
 */
void app_json_xfer_notify_system_clock(void)
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
            .mode   = clock.is_24 ? AppPB_SysClock_Mode_IS_24 : AppPB_SysClock_Mode_IS_12,
        },
    };
    /* 传输对象发送通知 */
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收系统时钟
 */
bool app_json_xfer_respond_system_clock(AppPB_MsgSet *message)
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
    APP_SYS_LOG_INFO("system_clock.year:%u",   clock.year);
    APP_SYS_LOG_INFO("system_clock.month:%u",  clock.month);
    APP_SYS_LOG_INFO("system_clock.day:%u",    clock.day);
    APP_SYS_LOG_INFO("system_clock.hour:%u",   clock.hour);
    APP_SYS_LOG_INFO("system_clock.minute:%u", clock.minute);
    APP_SYS_LOG_INFO("system_clock.second:%u", clock.second);
    APP_SYS_LOG_INFO("system_clock.zone:%u",   clock.zone_sec);
    APP_SYS_LOG_INFO("system_clock.mode:%u",   clock.is_24);

    app_sys_clock_to_utc(&clock);
    app_sys_clock_to_week(&clock);
    app_module_clock_set_system_clock(&clock);
    return true;
}

/*@brief 打包传输世界时钟
 */
void app_json_xfer_notify_world_clock(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_world_clock_tag;
    app_module_protocol_test_fill_world_clock(&message.payload.world_clock);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收世界时钟
 */
bool app_json_xfer_respond_world_clock(AppPB_MsgSet *message)
{
    AppPB_WorldClock *msg = &message->payload.world_clock;
    APP_SYS_LOG_INFO("world_clock.now_index:%u", msg->now_index);
    APP_SYS_LOG_INFO("world_clock.max_count:%u", msg->max_count);
    APP_SYS_LOG_INFO("world_clock.city_name:%s", msg->city_name);
    APP_SYS_LOG_INFO("world_clock.zone_offset:%d", msg->zone_offset);
    return true;
}

/*@brief 打包传输闹钟
 */
void app_json_xfer_notify_alarm(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_alarm_tag;
    app_module_protocol_test_fill_alarm(&message.payload.alarm);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收闹钟
 */
bool app_json_xfer_respond_alarm(AppPB_MsgSet *message)
{
    AppPB_Alarm *msg = &message->payload.alarm;
    APP_SYS_LOG_INFO("alarm.index:%u", msg->index);
    APP_SYS_LOG_INFO("alarm.repeat:%u", msg->repeat);
    APP_SYS_LOG_INFO("alarm.on:%u", msg->on);
    APP_SYS_LOG_INFO("alarm.hour:%u", msg->hour);
    APP_SYS_LOG_INFO("alarm.min:%u", msg->min);
    APP_SYS_LOG_INFO("alarm.name:%s", msg->name);
    return true;
}

/*@brief 打包传输天气
 */
void app_json_xfer_notify_weather(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_weather_tag;
    app_module_protocol_test_fill_weather(&message.payload.weather);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收天气
 */
bool app_json_xfer_respond_weather(AppPB_MsgSet *message)
{
    AppPB_Weather *msg = &message->payload.weather;
    APP_SYS_LOG_INFO("weather.temp_cur:%d", msg->temp_cur);
    APP_SYS_LOG_INFO("weather.temp_max:%d", msg->temp_max);
    APP_SYS_LOG_INFO("weather.temp_min:%d", msg->temp_min);
    APP_SYS_LOG_INFO("weather.phen:%u", msg->phen);
    APP_SYS_LOG_INFO("weather.humi:%u", msg->humi);
    APP_SYS_LOG_INFO("weather.city:%s", msg->city);
    APP_SYS_LOG_INFO("weather.day_count:%u", msg->day_count);
    return true;
}

/*@brief 打包传输心率参数
 */
void app_json_xfer_notify_heart_rate(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_heart_rate_tag;
    app_module_protocol_test_fill_heart_rate(&message.payload.heart_rate);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收心率参数
 */
bool app_json_xfer_respond_heart_rate(AppPB_MsgSet *message)
{
    AppPB_HeartRate *msg = &message->payload.heart_rate;
    APP_SYS_LOG_INFO("heart_rate.is_auto:%u", msg->is_auto);
    APP_SYS_LOG_INFO("heart_rate.lwarn:%u", msg->lwarn);
    APP_SYS_LOG_INFO("heart_rate.hwarn:%u", msg->hwarn);
    APP_SYS_LOG_INFO("heart_rate.test_min:%u", msg->test_min);
    return true;
}

/*@brief 打包传输音乐
 */
void app_json_xfer_notify_music(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_music_tag;
    app_module_protocol_test_fill_music(&message.payload.music);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收音乐
 */
bool app_json_xfer_respond_music(AppPB_MsgSet *message)
{
    AppPB_Music *msg = &message->payload.music;
    APP_SYS_LOG_INFO("music.singer:%s", msg->singer);
    APP_SYS_LOG_INFO("music.song_name:%s", msg->song_name);
    APP_SYS_LOG_INFO("music.play_st:%u", msg->play_st);
    APP_SYS_LOG_INFO("music.max_vol:%u", msg->max_vol);
    APP_SYS_LOG_INFO("music.cur_vol:%u", msg->cur_vol);
    return true;
}

/*@brief 打包传输消息通知
 */
void app_json_xfer_notify_msg_info(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_msg_info_tag;
    app_module_protocol_test_fill_msg_info(&message.payload.msg_info);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收消息通知
 */
bool app_json_xfer_respond_msg_info(AppPB_MsgSet *message)
{
    AppPB_MsgInfo *msg = &message->payload.msg_info;
    APP_SYS_LOG_INFO("msg_info.app_name:%s", msg->app_name);
    APP_SYS_LOG_INFO("msg_info.contact:%s", msg->contact);
    APP_SYS_LOG_INFO("msg_info.content:%s", msg->content);
    APP_SYS_LOG_INFO("msg_info.msg_type:%u", msg->msg_type);
    APP_SYS_LOG_INFO("msg_info.vibrate:%u", msg->vibrate);
    return true;
}

/*@brief 打包传输联系人
 */
void app_json_xfer_notify_contact(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_contact_tag;
    app_module_protocol_test_fill_contact(&message.payload.contact);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收联系人
 */
bool app_json_xfer_respond_contact(AppPB_MsgSet *message)
{
    AppPB_Contact *msg = &message->payload.contact;
    APP_SYS_LOG_INFO("contact.name:%s", msg->name);
    APP_SYS_LOG_INFO("contact.phone:%s", msg->phone);
    APP_SYS_LOG_INFO("contact.state:%u", msg->state);
    return true;
}

/*@brief 打包传输运动目标
 */
void app_json_xfer_notify_sport_tgt(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_tgt_tag;
    app_module_protocol_test_fill_sport_tgt(&message.payload.sport_tgt);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动目标
 */
bool app_json_xfer_respond_sport_tgt(AppPB_MsgSet *message)
{
    AppPB_SportTgt *msg = &message->payload.sport_tgt;
    APP_SYS_LOG_INFO("sport_tgt.motion_t:%u", msg->motion_t);
    APP_SYS_LOG_INFO("sport_tgt.step:%u", msg->step);
    APP_SYS_LOG_INFO("sport_tgt.kcal:%u", msg->kcal);
    APP_SYS_LOG_INFO("sport_tgt.meter:%u", msg->meter);
    return true;
}

/*@brief 打包传输用户身体信息
 */
void app_json_xfer_notify_user_phys(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_user_phys_tag;
    app_module_protocol_test_fill_user_phys(&message.payload.user_phys);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收用户身体信息
 */
bool app_json_xfer_respond_user_phys(AppPB_MsgSet *message)
{
    AppPB_UserPhys *msg = &message->payload.user_phys;
    APP_SYS_LOG_INFO("user_phys.age:%u", msg->age);
    APP_SYS_LOG_INFO("user_phys.gender:%u", msg->gender);
    APP_SYS_LOG_INFO("user_phys.height:%u", msg->height);
    APP_SYS_LOG_INFO("user_phys.weight:%u", msg->weight);
    return true;
}

/*@brief 打包传输运动汇总
 */
void app_json_xfer_notify_motion_sum(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_motion_sum_tag;
    app_module_protocol_test_fill_motion_sum(&message.payload.motion_sum);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动汇总
 */
bool app_json_xfer_respond_motion_sum(AppPB_MsgSet *message)
{
    AppPB_MotionSum *msg = &message->payload.motion_sum;
    APP_SYS_LOG_INFO("motion_sum.hr_value:%u", msg->hr_value);
    APP_SYS_LOG_INFO("motion_sum.kcal:%u", msg->kcal);
    APP_SYS_LOG_INFO("motion_sum.distance:%u", msg->distance);
    APP_SYS_LOG_INFO("motion_sum.step:%u", msg->step);
    return true;
}

/*@brief 打包传输运动状态
 */
void app_json_xfer_notify_sport_state(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_state_tag;
    app_module_protocol_test_fill_sport_state(&message.payload.sport_state);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动状态
 */
bool app_json_xfer_respond_sport_state(AppPB_MsgSet *message)
{
    AppPB_SportState *msg = &message->payload.sport_state;
    APP_SYS_LOG_INFO("sport_state.monitor_st:%u", msg->monitor_st);
    APP_SYS_LOG_INFO("sport_state.sport_type:%u", msg->sport_type);
    APP_SYS_LOG_INFO("sport_state.workout_type:%u", msg->workout_type);
    return true;
}

/*@brief 打包传输勿扰
 */
void app_json_xfer_notify_not_disturb(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_not_disturb_tag;
    app_module_protocol_test_fill_not_disturb(&message.payload.not_disturb);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收勿扰
 */
bool app_json_xfer_respond_not_disturb(AppPB_MsgSet *message)
{
    AppPB_NotDisturb *msg = &message->payload.not_disturb;
    APP_SYS_LOG_INFO("not_disturb.on:%u", msg->on);
    APP_SYS_LOG_INFO("not_disturb.shour:%u", msg->shour);
    APP_SYS_LOG_INFO("not_disturb.smin:%u", msg->smin);
    APP_SYS_LOG_INFO("not_disturb.ehour:%u", msg->ehour);
    APP_SYS_LOG_INFO("not_disturb.emin:%u", msg->emin);
    APP_SYS_LOG_INFO("not_disturb.repeat:%u", msg->repeat);
    return true;
}

/*@brief 打包传输地理位置
 */
void app_json_xfer_notify_position(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_position_tag;
    app_module_protocol_test_fill_position(&message.payload.position);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收地理位置
 */
bool app_json_xfer_respond_position(AppPB_MsgSet *message)
{
    AppPB_Position *msg = &message->payload.position;
    APP_SYS_LOG_INFO("position.speed:%u", msg->speed);
    APP_SYS_LOG_INFO("position.altitude:%d", msg->altitude);
    APP_SYS_LOG_INFO("position.latitude:%d", msg->latitude);
    APP_SYS_LOG_INFO("position.longitude:%d", msg->longitude);
    APP_SYS_LOG_INFO("position.bearing:%u", msg->bearing);
    return true;
}

/*@brief 打包传输生理周期
 */
void app_json_xfer_notify_fem_cycle(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_fem_cycle_tag;
    app_module_protocol_test_fill_fem_cycle(&message.payload.fem_cycle);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收生理周期
 */
bool app_json_xfer_respond_fem_cycle(AppPB_MsgSet *message)
{
    AppPB_FemCycle *msg = &message->payload.fem_cycle;
    APP_SYS_LOG_INFO("fem_cycle.remind_sw:%u", msg->remind_sw);
    APP_SYS_LOG_INFO("fem_cycle.keep_days:%u", msg->keep_days);
    APP_SYS_LOG_INFO("fem_cycle.cycle_days:%u", msg->cycle_days);
    return true;
}

/*@brief 打包传输账户
 */
void app_json_xfer_notify_account(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_account_tag;
    app_module_protocol_test_fill_account(&message.payload.account);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收账户
 */
bool app_json_xfer_respond_account(AppPB_MsgSet *message)
{
    AppPB_Account *msg = &message->payload.account;
    APP_SYS_LOG_INFO("account.account:%s", msg->account);
    APP_SYS_LOG_INFO("account.pair_state:%u", msg->pair_state);
    APP_SYS_LOG_INFO("account.app_role:%u", msg->app_role);
    return true;
}

/*@brief 打包传输运动管理
 */
void app_json_xfer_notify_sport_mng(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_mng_tag;
    app_module_protocol_test_fill_sport_mng(&message.payload.sport_mng);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动管理
 */
bool app_json_xfer_respond_sport_mng(AppPB_MsgSet *message)
{
    AppPB_SportMng *msg = &message->payload.sport_mng;
    APP_SYS_LOG_INFO("sport_mng.max_add_num:%u", msg->max_add_num);
    APP_SYS_LOG_INFO("sport_mng.min_add_num:%u", msg->min_add_num);
    APP_SYS_LOG_INFO("sport_mng.sport_type_count:%u", msg->sport_type_count);
    return true;
}

/*@brief 打包传输运动记录
 */
void app_json_xfer_notify_sport_rcd(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_rcd_tag;
    app_module_protocol_test_fill_sport_rcd(&message.payload.sport_rcd);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动记录
 */
bool app_json_xfer_respond_sport_rcd(AppPB_MsgSet *message)
{
    AppPB_SportRcd *msg = &message->payload.sport_rcd;
    APP_SYS_LOG_INFO("sport_rcd.id:%u", msg->id);
    APP_SYS_LOG_INFO("sport_rcd.status:%u", msg->status);
    APP_SYS_LOG_INFO("sport_rcd.calorie:%u", msg->calorie);
    APP_SYS_LOG_INFO("sport_rcd.distance:%u", msg->distance);
    APP_SYS_LOG_INFO("sport_rcd.step:%u", msg->step);
    APP_SYS_LOG_INFO("sport_rcd.type:%u", msg->type);
    return true;
}

/*@brief 文件发送限速状态机
 *       限速原因:无ack流控前一口气发包会堵死线程管道;
 *       每个定时周期(APP_MODULE_XFER_FILE_SEND_PERIOD)发送一个文件子消息;
 *       JSON只传元数据,阶段:开始(描述符)-结束,与nanopb子协议保持一致
 */
typedef enum {
    app_json_file_send_start = 0,  /* 待发文件描述符 */
    app_json_file_send_end,        /* 待发文件结束 */
} app_json_file_send_phase_t;

typedef struct {
    app_sys_timer_t timer;                              /* 发包限速定时器 */
    bool            active;                             /* 发送流程是否进行中 */
    uint8_t         image[APP_MODULE_XFER_FILE_IMAGE_SIZE]; /* 待发送的文件内容 */
    uint32_t        file_size;                          /* 文件总大小 */
    uint32_t        file_crc32;                         /* 文件CRC32 */
    uint32_t        phase;                              /* 当前发送阶段 */
} app_json_file_send_t;
static app_json_file_send_t app_json_file_send = {0};

/*@brief 文件发送限速定时器回调
 *       每个定时周期向manage线程投递一次发包步进;
 *       实际发包在manage线程执行,与protocol_test同构,避免跨线程操作传输层
 */
static void app_json_file_send_timer_handler(void *timer)
{
    /* 发送流程结束则不再投递 */
    if (!app_json_file_send.active)
        return;
    app_module_protocol_t protocol = {.notify.status = 0,};
    protocol.notify.type = app_module_protocol_file_step;
    app_module_protocol_notify(&protocol);
}

/*@brief 文件传输限速发包步进
 *       每次步进发送一个文件子消息(开始(描述符)->结束)
 */
void app_json_xfer_file_step(void)
{
    /* 发送流程结束则停止限速定时器 */
    if (!app_json_file_send.active) {
        app_sys_timer_stop(&app_json_file_send.timer);
        return;
    }
    /* 阶段:文件传输开始(描述符) */
    if (app_json_file_send.phase == app_json_file_send_start) {
        AppPB_MsgSet msg_start = {
            .which_payload = AppPB_MsgSet_file_tag,
            .payload.file = {
                .which_payload = AppPB_File_descriptor_tag,
            },
        };
        AppPB_FileDes *des = &msg_start.payload.file.payload.descriptor;
        snprintf(des->name, sizeof(des->name), "%s", app_module_protocol_test_file_name());
        des->utc64 = 0;
        des->crc32 = app_json_file_send.file_crc32;
        des->size  = app_json_file_send.file_size;
        des->crc8  = app_module_xfer_file_descriptor_crc8(des);
        app_json_xfer_notify_file_phase(&msg_start, APP_JSON_XFER_FILE_PHASE_START);
        app_json_file_send.phase = app_json_file_send_end;
        return;
    }
    /* 阶段:文件传输结束 */
    if (app_json_file_send.phase == app_json_file_send_end) {
        AppPB_MsgSet msg_end = {
            .which_payload = AppPB_MsgSet_file_tag,
            .payload.file = {
                .which_payload = AppPB_File_done_tag,
                .payload.done = { .code = 0 },
            },
        };
        app_json_xfer_notify_file_phase(&msg_end, APP_JSON_XFER_FILE_PHASE_END);
        /* 传输完成,停止限速定时器并复位状态 */
        app_json_file_send.active = false;
        app_sys_timer_stop(&app_json_file_send.timer);
        APP_SYS_LOG_INFO("file transfer send done size:%u", app_json_file_send.file_size);
        return;
    }
}

/*@brief 打包传输文件(开始->结束)
 *       数据源为日志队列内容(设备上报log文件场景),与trace_text同源;
 *       二进制分包数据无法用 JSON 承载,故只传输文件元数据结构;
 *       限速发送:启动软件定时器,每个周期投递一次发包步进,避免发包过多堵死线程管道
 */
void app_json_xfer_notify_file(void)
{
    /* 发送流程进行中则忽略新请求 */
    if (app_json_file_send.active)
        return;
    /* 组装文件内容:从日志队列提取条目,以换行分隔 */
    app_json_file_send.file_size  = app_module_xfer_file_build(app_json_file_send.image, sizeof(app_json_file_send.image));
    app_json_file_send.file_crc32 = app_sys_crc32(app_json_file_send.image, app_json_file_send.file_size);
    app_json_file_send.phase      = app_json_file_send_start;
    app_json_file_send.active     = true;
    /* 启动发包限速定时器 */
    app_json_file_send.timer.expired = app_json_file_send_timer_handler;
    app_json_file_send.timer.peroid  = APP_MODULE_XFER_FILE_SEND_PERIOD;
    app_json_file_send.timer.reload  = 1;
    app_sys_timer_start(&app_json_file_send.timer);
}

/*@brief 传输接收文件(开始/结束)
 *@param payload JSON数据负载
 *@param phase   文件传输阶段(开始/结束)
 */
bool app_json_xfer_respond_file(cJSON *payload, uint32_t phase)
{
    if (phase == APP_JSON_XFER_FILE_PHASE_START) {
        AppPB_FileDes des = AppPB_FileDes_init_zero;
        if (!app_json_xfer_unpack_file_des(payload, &des)) {
            APP_SYS_LOG_WARN("file descriptor unpack fail");
            return false;
        }
        /* 描述符CRC8校验 */
        if (des.crc8 != app_module_xfer_file_descriptor_crc8(&des)) {
            APP_SYS_LOG_WARN("file descriptor crc8 fail");
            return false;
        }
        APP_SYS_LOG_INFO("file transfer start name:%s size:%u crc32:%08x", des.name, des.size, des.crc32);
        return true;
    }
    if (phase == APP_JSON_XFER_FILE_PHASE_END) {
        AppPB_FileEnd end = AppPB_FileEnd_init_zero;
        if (!app_json_xfer_unpack_file_end(payload, &end)) {
            APP_SYS_LOG_WARN("file end unpack fail");
            return false;
        }
        APP_SYS_LOG_INFO("file transfer end code:%u", end.code);
        return true;
    }
    APP_SYS_LOG_ERROR("file have unknown phase:%u", phase);
    return false;
}

/*@brief 打包传输OTA升级
 */
void app_json_xfer_notify_ota(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_ota_tag;
    app_module_protocol_test_fill_ota(&message.payload.ota);
    app_json_xfer_notify_msg(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收OTA升级
 */
bool app_json_xfer_respond_ota(AppPB_MsgSet *message)
{
    AppPB_Ota *msg = &message->payload.ota;
    APP_SYS_LOG_INFO("ota.cmd:%u", msg->cmd);
    APP_SYS_LOG_INFO("ota.state:%u", msg->state);
    APP_SYS_LOG_INFO("ota.ready_cond:%u", msg->ready_cond);
    return true;
}

#endif
