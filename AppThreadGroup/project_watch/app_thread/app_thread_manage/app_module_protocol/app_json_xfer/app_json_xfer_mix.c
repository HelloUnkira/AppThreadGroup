/*实现目标:
 *    json协议-各分类消息的打包与接收
 *    复用AppPB_MsgSet中间结构体,由codegen生成的pack/unpack层做json序列化
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_JSON

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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_elec_card_tag,
        .payload.elec_card = {
            .is_activate = true,
            .is_reported = false,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_world_clock_tag,
        .payload.world_clock = {
            .now_index   = 0,
            .max_count   = 1,
            .city_name   = "Beijing",
            .zone_offset = 8 * 3600,
            .city_id     = 0,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_alarm_tag,
        .payload.alarm = {
            .index  = 0,
            .repeat = 0x3E,
            .on     = 1,
            .hour   = 7,
            .min    = 30,
            .name   = "wakeup",
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_weather_tag,
        .payload.weather = {
            .temp_cur = 26,
            .temp_max = 31,
            .temp_min = 22,
            .phen     = 1,
            .humi     = 60,
            .uv       = 5,
            .pm25     = 35,
            .aqi      = 45,
            .city     = "shenzhen",
            .day_count = 3,
            .day = {
                { .temp_max = 31, .temp_min = 22, .phen = 1, .humi = 60 },
                { .temp_max = 30, .temp_min = 21, .phen = 2, .humi = 65 },
                { .temp_max = 29, .temp_min = 20, .phen = 1, .humi = 70 },
            },
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_heart_rate_tag,
        .payload.heart_rate = {
            .is_auto     = 1,
            .lwarn_on    = 1,
            .hwarn_on    = 1,
            .lwarn       = 50,
            .hwarn       = 120,
            .test_min    = 30,
            .static_type = 0,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_music_tag,
        .payload.music = {
            .singer    = "Jay",
            .song_name = "Cloud",
            .play_st   = 1,
            .max_vol   = 10,
            .cur_vol   = 6,
            .app_st    = 1,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_msg_info_tag,
        .payload.msg_info = {
            .app_name = "wechat",
            .contact  = "tom",
            .content  = "hello",
            .msg_id   = 1,
            .msg_type = 1,
            .vibrate  = 1,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_contact_tag,
        .payload.contact = {
            .name      = "tom",
            .name_len  = 3,
            .phone     = "13800138000",
            .phone_len = 11,
            .state     = 1,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_sport_tgt_tag,
        .payload.sport_tgt = {
            .motion_t  = 1,
            .goal_type = 1,
            .step      = 10000,
            .kcal      = 500,
            .meter     = 8000,
            .duration  = 7200,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_user_phys_tag,
        .payload.user_phys = {
            .age        = 30,
            .birthday   = 0,
            .gender     = 1,
            .height     = 175,
            .weight     = 65,
            .vo2max     = 45,
            .run_step   = 80,
            .walk_step  = 70,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_motion_sum_tag,
        .payload.motion_sum = {
            .hr_value   = 75,
            .hr_ts      = 0,
            .kcal       = 300,
            .distance   = 5000,
            .elevation  = 100,
            .sleep_time = 480,
            .step       = 8000,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_sport_state_tag,
        .payload.sport_state = {
            .monitor_st   = 0,
            .operator_t   = 0,
            .sport_type   = 1,
            .start_time   = 0,
            .workout_type = 1,
            .op_time      = 0,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_not_disturb_tag,
        .payload.not_disturb = {
            .on     = 1,
            .shour  = 22,
            .smin   = 0,
            .ehour  = 7,
            .emin   = 0,
            .repeat = 0x7F,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_position_tag,
        .payload.position = {
            .speed      = 10,
            .distance   = 1000,
            .altitude   = 30,
            .total_dist = 10000,
            .start_time = 0,
            .end_time   = 0,
            .latitude   = 22540000,
            .longitude  = 114000000,
            .bearing    = 90,
            .accuracy   = 5,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_fem_cycle_tag,
        .payload.fem_cycle = {
            .remind_sw         = 1,
            .menstr_remind     = 1,
            .menstr_end_remind = 1,
            .ovulat_remind     = 0,
            .ovulat_end_remind = 0,
            .cycle_start       = 0,
            .cycle_end         = 0,
            .keep_days         = 5,
            .cycle_days        = 28,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_account_tag,
        .payload.account = {
            .account    = "user01",
            .acc_len    = 6,
            .pair_state = 1,
            .app_role   = 1,
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_sport_mng_tag,
        .payload.sport_mng = {
            .max_add_num      = 10,
            .min_add_num      = 1,
            .sport_type_count = 3,
            .sport_type       = { 1, 2, 3 },
        },
    };
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
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_sport_rcd_tag,
        .payload.sport_rcd = {
            .id         = 1,
            .status     = 1,
            .start_time = 0,
            .end_time   = 3600,
            .calorie    = 300,
            .distance   = 5000,
            .step       = 6000,
            .duration   = 3600,
            .speed      = 10,
            .type       = 1,
        },
    };
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

/*@brief 打包传输OTA升级
 */
void app_json_xfer_notify_ota(void)
{
    AppPB_MsgSet message = {
        .which_payload = AppPB_MsgSet_ota_tag,
        .payload.ota = {
            .cmd             = 1,
            .state           = 0,
            .ready_cond      = 0,
        },
    };
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
