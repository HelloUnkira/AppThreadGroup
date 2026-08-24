/*实现目标:
 *    nanopb协议子部件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输闹钟
 */
void app_nanopb_xfer_notify_alarm(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_alarm(&message);
#else
    message.which_payload = AppPB_MsgSet_alarm_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收闹钟
 */
bool app_nanopb_xfer_respond_alarm(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_weather(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_weather(&message);
#else
    message.which_payload = AppPB_MsgSet_weather_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收天气
 */
bool app_nanopb_xfer_respond_weather(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_heart_rate(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_heart_rate(&message);
#else
    message.which_payload = AppPB_MsgSet_heart_rate_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收心率参数
 */
bool app_nanopb_xfer_respond_heart_rate(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_music(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_music(&message);
#else
    message.which_payload = AppPB_MsgSet_music_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收音乐
 */
bool app_nanopb_xfer_respond_music(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_msg_info(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_msg_info(&message);
#else
    message.which_payload = AppPB_MsgSet_msg_info_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收消息通知
 */
bool app_nanopb_xfer_respond_msg_info(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_contact(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_contact(&message);
#else
    message.which_payload = AppPB_MsgSet_contact_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收联系人
 */
bool app_nanopb_xfer_respond_contact(AppPB_MsgSet *message)
{
    AppPB_Contact *msg = &message->payload.contact;
    APP_SYS_LOG_INFO("contact.name:%s", msg->name);
    APP_SYS_LOG_INFO("contact.phone:%s", msg->phone);
    APP_SYS_LOG_INFO("contact.state:%u", msg->state);
    return true;
}

/*@brief 打包传输运动目标
 */
void app_nanopb_xfer_notify_sport_tgt(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_sport_tgt(&message);
#else
    message.which_payload = AppPB_MsgSet_sport_tgt_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动目标
 */
bool app_nanopb_xfer_respond_sport_tgt(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_user_phys(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_user_phys(&message);
#else
    message.which_payload = AppPB_MsgSet_user_phys_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收用户身体信息
 */
bool app_nanopb_xfer_respond_user_phys(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_motion_sum(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_motion_sum(&message);
#else
    message.which_payload = AppPB_MsgSet_motion_sum_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动汇总
 */
bool app_nanopb_xfer_respond_motion_sum(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_sport_state(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_sport_state(&message);
#else
    message.which_payload = AppPB_MsgSet_sport_state_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动状态
 */
bool app_nanopb_xfer_respond_sport_state(AppPB_MsgSet *message)
{
    AppPB_SportState *msg = &message->payload.sport_state;
    APP_SYS_LOG_INFO("sport_state.monitor_st:%u", msg->monitor_st);
    APP_SYS_LOG_INFO("sport_state.sport_type:%u", msg->sport_type);
    APP_SYS_LOG_INFO("sport_state.workout_type:%u", msg->workout_type);
    return true;
}

/*@brief 打包传输勿扰
 */
void app_nanopb_xfer_notify_not_disturb(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_not_disturb(&message);
#else
    message.which_payload = AppPB_MsgSet_not_disturb_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收勿扰
 */
bool app_nanopb_xfer_respond_not_disturb(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_position(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_position(&message);
#else
    message.which_payload = AppPB_MsgSet_position_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收地理位置
 */
bool app_nanopb_xfer_respond_position(AppPB_MsgSet *message)
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
void app_nanopb_xfer_notify_fem_cycle(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_fem_cycle(&message);
#else
    message.which_payload = AppPB_MsgSet_fem_cycle_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收生理周期
 */
bool app_nanopb_xfer_respond_fem_cycle(AppPB_MsgSet *message)
{
    AppPB_FemCycle *msg = &message->payload.fem_cycle;
    APP_SYS_LOG_INFO("fem_cycle.remind_sw:%u", msg->remind_sw);
    APP_SYS_LOG_INFO("fem_cycle.keep_days:%u", msg->keep_days);
    APP_SYS_LOG_INFO("fem_cycle.cycle_days:%u", msg->cycle_days);
    return true;
}

/*@brief 打包传输账户
 */
void app_nanopb_xfer_notify_account(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_account(&message);
#else
    message.which_payload = AppPB_MsgSet_account_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收账户
 */
bool app_nanopb_xfer_respond_account(AppPB_MsgSet *message)
{
    AppPB_Account *msg = &message->payload.account;
    APP_SYS_LOG_INFO("account.account:%s", msg->account);
    APP_SYS_LOG_INFO("account.pair_state:%u", msg->pair_state);
    APP_SYS_LOG_INFO("account.app_role:%u", msg->app_role);
    return true;
}

/*@brief 打包传输运动管理
 */
void app_nanopb_xfer_notify_sport_mng(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_sport_mng(&message);
#else
    message.which_payload = AppPB_MsgSet_sport_mng_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动管理
 */
bool app_nanopb_xfer_respond_sport_mng(AppPB_MsgSet *message)
{
    AppPB_SportMng *msg = &message->payload.sport_mng;
    APP_SYS_LOG_INFO("sport_mng.max_add_num:%u", msg->max_add_num);
    APP_SYS_LOG_INFO("sport_mng.min_add_num:%u", msg->min_add_num);
    APP_SYS_LOG_INFO("sport_mng.sport_type_count:%u", msg->sport_type_count);
    return true;
}

/*@brief 打包传输运动记录
 */
void app_nanopb_xfer_notify_sport_rcd(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_sport_rcd(&message);
#else
    message.which_payload = AppPB_MsgSet_sport_rcd_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动记录
 */
bool app_nanopb_xfer_respond_sport_rcd(AppPB_MsgSet *message)
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
            .mode   = clock.is_24 ? AppPB_SysClock_Mode_IS_24 : AppPB_SysClock_Mode_IS_12,
        },
    };
    /* 传输对象发送通知 */
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
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
void app_nanopb_xfer_notify_world_clock(void)
{
    AppPB_MsgSet message = {0};
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_test_fill_world_clock(&message);
#else
    message.which_payload = AppPB_MsgSet_world_clock_tag;
#endif
    app_nanopb_xfer_ctrl_notify(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收世界时钟
 */
bool app_nanopb_xfer_respond_world_clock(AppPB_MsgSet *message)
{
    AppPB_WorldClock *msg = &message->payload.world_clock;
    APP_SYS_LOG_INFO("world_clock.now_index:%u", msg->now_index);
    APP_SYS_LOG_INFO("world_clock.max_count:%u", msg->max_count);
    APP_SYS_LOG_INFO("world_clock.city_name:%s", msg->city_name);
    APP_SYS_LOG_INFO("world_clock.zone_offset:%d", msg->zone_offset);
    return true;
}

#endif
