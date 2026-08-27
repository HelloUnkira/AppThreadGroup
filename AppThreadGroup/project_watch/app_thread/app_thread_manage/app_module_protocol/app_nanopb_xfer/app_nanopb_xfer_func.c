/*实现目标:
 *    nanopb协议子部件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

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
    message.which_payload = AppPB_MsgSet_alarm_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Alarm *data = &message.payload.alarm;
    data->index  = 0;
    data->repeat = 0x3E;
    data->on     = 1;
    data->hour   = 7;
    data->min    = 30;
    snprintf(data->name, sizeof(data->name), "%s", "wakeup");
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收闹钟
 */
bool app_nanopb_xfer_respond_alarm(AppPB_MsgSet *message)
{
    AppPB_Alarm *msg = &message->payload.alarm;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("alarm.index:%u",  msg->index);
    APP_SYS_LOG_INFO("alarm.repeat:%u", msg->repeat);
    APP_SYS_LOG_INFO("alarm.on:%u",     msg->on);
    APP_SYS_LOG_INFO("alarm.hour:%u",   msg->hour);
    APP_SYS_LOG_INFO("alarm.min:%u",    msg->min);
    APP_SYS_LOG_INFO("alarm.name:%s",   msg->name);
#endif
    return true;
}

/*@brief 打包传输天气
 */
void app_nanopb_xfer_notify_weather(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_weather_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Weather *data = &message.payload.weather;
    data->temp_cur  = 26;
    data->temp_max  = 31;
    data->temp_min  = 22;
    data->phen      = 1;
    data->humi      = 60;
    data->uv        = 5;
    data->pm25      = 35;
    data->aqi       = 45;
    snprintf(data->city, sizeof(data->city), "%s", "shenzhen");
    data->day_count = 3;
    data->day[0] = (AppPB_WeatherDay){ .temp_max = 31, .temp_min = 22, .phen = 1, .humi = 60 };
    data->day[1] = (AppPB_WeatherDay){ .temp_max = 30, .temp_min = 21, .phen = 2, .humi = 65 };
    data->day[2] = (AppPB_WeatherDay){ .temp_max = 29, .temp_min = 20, .phen = 1, .humi = 70 };
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收天气
 */
bool app_nanopb_xfer_respond_weather(AppPB_MsgSet *message)
{
    AppPB_Weather *msg = &message->payload.weather;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("weather.temp_cur:%d",  msg->temp_cur);
    APP_SYS_LOG_INFO("weather.temp_max:%d",  msg->temp_max);
    APP_SYS_LOG_INFO("weather.temp_min:%d",  msg->temp_min);
    APP_SYS_LOG_INFO("weather.phen:%u",      msg->phen);
    APP_SYS_LOG_INFO("weather.humi:%u",      msg->humi);
    APP_SYS_LOG_INFO("weather.city:%s",      msg->city);
    APP_SYS_LOG_INFO("weather.day_count:%u", msg->day_count);
#endif
    return true;
}

/*@brief 打包传输心率参数
 */
void app_nanopb_xfer_notify_heart_rate(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_heart_rate_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_HeartRate *data = &message.payload.heart_rate;
    data->is_auto     = 1;
    data->lwarn_on    = 1;
    data->hwarn_on    = 1;
    data->lwarn       = 50;
    data->hwarn       = 120;
    data->test_min    = 30;
    data->static_type = 0;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收心率参数
 */
bool app_nanopb_xfer_respond_heart_rate(AppPB_MsgSet *message)
{
    AppPB_HeartRate *msg = &message->payload.heart_rate;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("heart_rate.is_auto:%u", msg->is_auto);
    APP_SYS_LOG_INFO("heart_rate.lwarn:%u",   msg->lwarn);
    APP_SYS_LOG_INFO("heart_rate.hwarn:%u",   msg->hwarn);
#endif
    return true;
}

/*@brief 打包传输音乐
 */
void app_nanopb_xfer_notify_music(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_music_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Music *data = &message.payload.music;
    snprintf(data->singer,    sizeof(data->singer),    "%s", "Jay");
    snprintf(data->song_name, sizeof(data->song_name), "%s", "Cloud");
    data->play_st = 1;
    data->max_vol = 10;
    data->cur_vol = 6;
    data->app_st  = 1;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收音乐
 */
bool app_nanopb_xfer_respond_music(AppPB_MsgSet *message)
{
    AppPB_Music *msg = &message->payload.music;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("music.singer:%s",    msg->singer);
    APP_SYS_LOG_INFO("music.song_name:%s", msg->song_name);
    APP_SYS_LOG_INFO("music.play_st:%u",   msg->play_st);
    APP_SYS_LOG_INFO("music.max_vol:%u",   msg->max_vol);
    APP_SYS_LOG_INFO("music.cur_vol:%u",   msg->cur_vol);
#endif
    return true;
}

/*@brief 打包传输消息通知
 */
void app_nanopb_xfer_notify_msg_info(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_msg_info_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_MsgInfo *data = &message.payload.msg_info;
    snprintf(data->app_name, sizeof(data->app_name), "%s", "wechat");
    snprintf(data->contact,  sizeof(data->contact),  "%s", "tom");
    snprintf(data->content,  sizeof(data->content),  "%s", "hello");
    snprintf(data->phone,    sizeof(data->phone),    "%s", "13800138000");
    data->msg_id   = 1;
    data->msg_type = 1;
    data->vibrate  = 1;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收消息通知
 */
bool app_nanopb_xfer_respond_msg_info(AppPB_MsgSet *message)
{
    AppPB_MsgInfo *msg = &message->payload.msg_info;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("msg_info.app_name:%s", msg->app_name);
    APP_SYS_LOG_INFO("msg_info.contact:%s",  msg->contact);
    APP_SYS_LOG_INFO("msg_info.content:%s",  msg->content);
    APP_SYS_LOG_INFO("msg_info.msg_type:%u", msg->msg_type);
    APP_SYS_LOG_INFO("msg_info.vibrate:%u",  msg->vibrate);
#endif
    return true;
}

/*@brief 打包传输联系人
 */
void app_nanopb_xfer_notify_contact(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_contact_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Contact *data = &message.payload.contact;
    snprintf(data->name,  sizeof(data->name),  "%s", "tom");
    snprintf(data->phone, sizeof(data->phone), "%s", "13800138000");
    data->name_len  = 3;
    data->phone_len = 11;
    data->state     = 1;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收联系人
 */
bool app_nanopb_xfer_respond_contact(AppPB_MsgSet *message)
{
    AppPB_Contact *msg = &message->payload.contact;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("contact.name:%s",  msg->name);
    APP_SYS_LOG_INFO("contact.phone:%s", msg->phone);
    APP_SYS_LOG_INFO("contact.state:%u", msg->state);
#endif
    return true;
}

/*@brief 打包传输运动目标
 */
void app_nanopb_xfer_notify_sport_tgt(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_tgt_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportTgt *data = &message.payload.sport_tgt;
    data->motion_t  = 1;
    data->goal_type = 1;
    data->step      = 10000;
    data->kcal      = 500;
    data->meter     = 8000;
    data->duration  = 7200;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动目标
 */
bool app_nanopb_xfer_respond_sport_tgt(AppPB_MsgSet *message)
{
    AppPB_SportTgt *msg = &message->payload.sport_tgt;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("sport_tgt.motion_t:%u", msg->motion_t);
    APP_SYS_LOG_INFO("sport_tgt.step:%u",     msg->step);
    APP_SYS_LOG_INFO("sport_tgt.kcal:%u",     msg->kcal);
    APP_SYS_LOG_INFO("sport_tgt.meter:%u",    msg->meter);
#endif
    return true;
}

/*@brief 打包传输用户身体信息
 */
void app_nanopb_xfer_notify_user_phys(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_user_phys_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_UserPhys *data = &message.payload.user_phys;
    data->age       = 30;
    data->birthday  = 0;
    data->gender    = 1;
    data->height    = 175;
    data->weight    = 65;
    data->vo2max    = 45;
    data->run_step  = 80;
    data->walk_step = 70;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收用户身体信息
 */
bool app_nanopb_xfer_respond_user_phys(AppPB_MsgSet *message)
{
    AppPB_UserPhys *msg = &message->payload.user_phys;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("user_phys.age:%u",    msg->age);
    APP_SYS_LOG_INFO("user_phys.height:%u", msg->height);
    APP_SYS_LOG_INFO("user_phys.weight:%u", msg->weight);
#endif
    return true;
}

/*@brief 打包传输运动汇总
 */
void app_nanopb_xfer_notify_motion_sum(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_motion_sum_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_MotionSum *data = &message.payload.motion_sum;
    data->hr_value   = 75;
    data->hr_ts      = 0;
    data->kcal       = 300;
    data->distance   = 5000;
    data->elevation  = 100;
    data->sleep_time = 480;
    data->step       = 8000;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动汇总
 */
bool app_nanopb_xfer_respond_motion_sum(AppPB_MsgSet *message)
{
    AppPB_MotionSum *msg = &message->payload.motion_sum;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("motion_sum.hr_value:%u", msg->hr_value);
    APP_SYS_LOG_INFO("motion_sum.kcal:%u",     msg->kcal);
    APP_SYS_LOG_INFO("motion_sum.distance:%u", msg->distance);
    APP_SYS_LOG_INFO("motion_sum.step:%u",     msg->step);
#endif
    return true;
}

/*@brief 打包传输运动状态
 */
void app_nanopb_xfer_notify_sport_state(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_state_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportState *data = &message.payload.sport_state;
    data->monitor_st   = 0;
    data->operator_t   = 0;
    data->sport_type   = 1;
    data->start_time   = 0;
    data->workout_type = 1;
    data->op_time      = 0;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动状态
 */
bool app_nanopb_xfer_respond_sport_state(AppPB_MsgSet *message)
{
    AppPB_SportState *msg = &message->payload.sport_state;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("sport_state.monitor_st:%u",  msg->monitor_st);
    APP_SYS_LOG_INFO("sport_state.sport_type:%u",  msg->sport_type);
    APP_SYS_LOG_INFO("sport_state.workout_type:%u", msg->workout_type);
#endif
    return true;
}

/*@brief 打包传输勿扰
 */
void app_nanopb_xfer_notify_not_disturb(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_not_disturb_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_NotDisturb *data = &message.payload.not_disturb;
    data->on     = 1;
    data->shour  = 22;
    data->smin   = 0;
    data->ehour  = 7;
    data->emin   = 0;
    data->repeat = 0x7F;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收勿扰
 */
bool app_nanopb_xfer_respond_not_disturb(AppPB_MsgSet *message)
{
    AppPB_NotDisturb *msg = &message->payload.not_disturb;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("not_disturb.on:%u",    msg->on);
    APP_SYS_LOG_INFO("not_disturb.shour:%u", msg->shour);
    APP_SYS_LOG_INFO("not_disturb.smin:%u",  msg->smin);
    APP_SYS_LOG_INFO("not_disturb.ehour:%u", msg->ehour);
    APP_SYS_LOG_INFO("not_disturb.emin:%u",  msg->emin);
    APP_SYS_LOG_INFO("not_disturb.repeat:%u", msg->repeat);
#endif
    return true;
}

/*@brief 打包传输地理位置
 */
void app_nanopb_xfer_notify_position(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_position_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Position *data = &message.payload.position;
    data->speed      = 10;
    data->distance   = 1000;
    data->altitude   = 30;
    data->total_dist = 10000;
    data->start_time = 0;
    data->end_time   = 0;
    data->latitude   = 22540000;
    data->longitude  = 114000000;
    data->bearing    = 90;
    data->accuracy   = 5;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收地理位置
 */
bool app_nanopb_xfer_respond_position(AppPB_MsgSet *message)
{
    AppPB_Position *msg = &message->payload.position;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("position.speed:%u",     msg->speed);
    APP_SYS_LOG_INFO("position.altitude:%d",  msg->altitude);
    APP_SYS_LOG_INFO("position.latitude:%d",  msg->latitude);
    APP_SYS_LOG_INFO("position.longitude:%d", msg->longitude);
    APP_SYS_LOG_INFO("position.bearing:%u",   msg->bearing);
#endif
    return true;
}

/*@brief 打包传输生理周期
 */
void app_nanopb_xfer_notify_fem_cycle(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_fem_cycle_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_FemCycle *data = &message.payload.fem_cycle;
    data->remind_sw         = 1;
    data->menstr_remind     = 1;
    data->menstr_end_remind = 1;
    data->ovulat_remind     = 0;
    data->ovulat_end_remind = 0;
    data->cycle_start       = 0;
    data->cycle_end         = 0;
    data->keep_days         = 5;
    data->cycle_days        = 28;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收生理周期
 */
bool app_nanopb_xfer_respond_fem_cycle(AppPB_MsgSet *message)
{
    AppPB_FemCycle *msg = &message->payload.fem_cycle;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("fem_cycle.remind_sw:%u", msg->remind_sw);
    APP_SYS_LOG_INFO("fem_cycle.keep_days:%u", msg->keep_days);
    APP_SYS_LOG_INFO("fem_cycle.cycle_days:%u", msg->cycle_days);
#endif
    return true;
}

/*@brief 打包传输账户
 */
void app_nanopb_xfer_notify_account(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_account_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Account *data = &message.payload.account;
    snprintf(data->account, sizeof(data->account), "%s", "user01");
    data->acc_len    = 6;
    data->pair_state = 1;
    data->app_role   = 1;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收账户
 */
bool app_nanopb_xfer_respond_account(AppPB_MsgSet *message)
{
    AppPB_Account *msg = &message->payload.account;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("account.account:%s",    msg->account);
    APP_SYS_LOG_INFO("account.pair_state:%u", msg->pair_state);
    APP_SYS_LOG_INFO("account.app_role:%u",   msg->app_role);
#endif
    return true;
}

/*@brief 打包传输运动管理
 */
void app_nanopb_xfer_notify_sport_mng(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_mng_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportMng *data = &message.payload.sport_mng;
    data->max_add_num      = 10;
    data->min_add_num      = 1;
    data->sport_type_count = 3;
    data->sport_type[0]    = 1;
    data->sport_type[1]    = 2;
    data->sport_type[2]    = 3;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动管理
 */
bool app_nanopb_xfer_respond_sport_mng(AppPB_MsgSet *message)
{
    AppPB_SportMng *msg = &message->payload.sport_mng;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("sport_mng.max_add_num:%u",      msg->max_add_num);
    APP_SYS_LOG_INFO("sport_mng.min_add_num:%u",      msg->min_add_num);
    APP_SYS_LOG_INFO("sport_mng.sport_type_count:%u", msg->sport_type_count);
#endif
    return true;
}

/*@brief 打包传输运动记录
 */
void app_nanopb_xfer_notify_sport_rcd(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sport_rcd_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportRcd *data = &message.payload.sport_rcd;
    data->id         = 1;
    data->status     = 1;
    data->start_time = 0;
    data->end_time   = 3600;
    data->calorie    = 300;
    data->distance   = 5000;
    data->step       = 6000;
    data->duration   = 3600;
    data->speed      = 10;
    data->type       = 1;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收运动记录
 */
bool app_nanopb_xfer_respond_sport_rcd(AppPB_MsgSet *message)
{
    AppPB_SportRcd *msg = &message->payload.sport_rcd;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("sport_rcd.id:%u",       msg->id);
    APP_SYS_LOG_INFO("sport_rcd.status:%u",   msg->status);
    APP_SYS_LOG_INFO("sport_rcd.calorie:%u",  msg->calorie);
    APP_SYS_LOG_INFO("sport_rcd.distance:%u", msg->distance);
    APP_SYS_LOG_INFO("sport_rcd.step:%u",     msg->step);
    APP_SYS_LOG_INFO("sport_rcd.type:%u",     msg->type);
#endif
    return true;
}

/*@brief 打包传输显示与菜单
 */
void app_nanopb_xfer_notify_display_info(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_display_info_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_DisplayInfo *data = &message.payload.display_info;
    data->display_off_sec = 15;
    data->backlight       = 50;
    data->last_brt        = 50;
    data->theme_screen    = 0;
    data->effect_style    = 0;
    data->interface_style = 0;
    data->jump_home       = 1;
    data->dial_lock       = 0;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收显示与菜单
 */
bool app_nanopb_xfer_respond_display_info(AppPB_MsgSet *message)
{
    AppPB_DisplayInfo *msg = &message->payload.display_info;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("display_info.display_off_sec:%u", msg->display_off_sec);
    APP_SYS_LOG_INFO("display_info.backlight:%u", msg->backlight);
    APP_SYS_LOG_INFO("display_info.theme_screen:%u", msg->theme_screen);
#endif
    return true;
}

/*@brief 打包传输App绑定
 */
void app_nanopb_xfer_notify_app_bind(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_app_bind_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_AppBind *data = &message.payload.app_bind;
    data->new_bind_uid  = 0;
    data->save_bind_uid = 0;
    data->qr_random     = 0;
    data->conn_before   = 1;
    data->bind_notify   = 0;
    data->our_app_conn  = 1;
    data->bind_start    = 0;
    data->disp_screen   = 0;
    data->conn_sec_cnt  = 0;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收App绑定
 */
bool app_nanopb_xfer_respond_app_bind(AppPB_MsgSet *message)
{
    AppPB_AppBind *msg = &message->payload.app_bind;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("app_bind.new_bind_uid:%u", msg->new_bind_uid);
    APP_SYS_LOG_INFO("app_bind.our_app_conn:%u", msg->our_app_conn);
#endif
    return true;
}

/*@brief 打包传输省电/影院/运动识别
 */
void app_nanopb_xfer_notify_power(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_power_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Power *data = &message.payload.power;
    data->power_save    = 0;
    data->everbright    = 0;
    data->hr_auto       = 1;
    data->spo2_auto     = 1;
    data->pressure_auto = 1;
    data->emotion_auto  = 1;
    data->theater       = 0;
    data->motion_reco   = 1;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收省电/影院/运动识别
 */
bool app_nanopb_xfer_respond_power(AppPB_MsgSet *message)
{
    AppPB_Power *msg = &message->payload.power;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("power.power_save:%u", msg->power_save);
    APP_SYS_LOG_INFO("power.theater:%u",    msg->theater);
#endif
    return true;
}

/*@brief 打包传输睡眠设置
 */
void app_nanopb_xfer_notify_sleep_set(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_sleep_set_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SleepSet *data = &message.payload.sleep_set;
    data->schedule_count = 7;
    for (uint32_t idx = 0; idx < 7; idx++) {
        data->schedule[idx].day_of_week = 0;
        data->schedule[idx].fall_hour   = 23;
        data->schedule[idx].fall_min    = 0;
        data->schedule[idx].wake_hour   = 7;
        data->schedule[idx].wake_min    = 0;
        data->schedule[idx].is_alarm    = 1;
    }
    data->mode_switch   = 1;
    data->mode_status   = 0;
    data->target_dur    = 480;
    data->remind_switch = 1;
    data->remind        = 30;
    data->snooze_on     = 1;
    data->snooze_time   = 3000;
    data->snooze_cnt    = 3;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收睡眠设置
 */
bool app_nanopb_xfer_respond_sleep_set(AppPB_MsgSet *message)
{
    AppPB_SleepSet *msg = &message->payload.sleep_set;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("sleep_set.mode_switch:%u", msg->mode_switch);
    APP_SYS_LOG_INFO("sleep_set.target_dur:%u",  msg->target_dur);
    APP_SYS_LOG_INFO("sleep_set.remind:%u",      msg->remind);
#endif
    return true;
}

/*@brief 打包传输表盘配置
 */
void app_nanopb_xfer_notify_watch_cfg(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_watch_cfg_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_WatchCfg *data = &message.payload.watch_cfg;
    data->pixel_width  = 466;
    data->pixel_height = 466;
    data->file_crc32   = 0;
    data->cur_index    = 0;
    data->aod_index    = 0;
    data->sort_count   = 8;
    for (uint32_t idx = 0; idx < 8; idx++)
        data->sort[idx] = idx;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收表盘配置
 */
bool app_nanopb_xfer_respond_watch_cfg(AppPB_MsgSet *message)
{
    AppPB_WatchCfg *msg = &message->payload.watch_cfg;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("watch_cfg.cur_index:%u", msg->cur_index);
    APP_SYS_LOG_INFO("watch_cfg.aod_index:%u", msg->aod_index);
#endif
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
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
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
    app_sys_clock_to_utc(&clock);
    app_sys_clock_to_week(&clock);
    app_module_clock_set_system_clock(&clock);
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("system_clock.year:%u",   clock.year);
    APP_SYS_LOG_INFO("system_clock.month:%u",  clock.month);
    APP_SYS_LOG_INFO("system_clock.day:%u",    clock.day);
    APP_SYS_LOG_INFO("system_clock.hour:%u",   clock.hour);
    APP_SYS_LOG_INFO("system_clock.minute:%u", clock.minute);
    APP_SYS_LOG_INFO("system_clock.second:%u", clock.second);
    APP_SYS_LOG_INFO("system_clock.zone:%u",   clock.zone_sec);
    APP_SYS_LOG_INFO("system_clock.mode:%u",   clock.is_24);
#endif
    return true;
}

/*@brief 打包传输世界时钟
 */
void app_nanopb_xfer_notify_world_clock(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_world_clock_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_WorldClock *data = &message.payload.world_clock;
    data->now_index   = 0;
    data->max_count   = 1;
    snprintf(data->city_name, sizeof(data->city_name), "%s", "Beijing");
    data->zone_offset = 8 * 3600;
    data->city_id     = 0;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收世界时钟
 */
bool app_nanopb_xfer_respond_world_clock(AppPB_MsgSet *message)
{
    AppPB_WorldClock *msg = &message->payload.world_clock;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("world_clock.now_index:%u", msg->now_index);
    APP_SYS_LOG_INFO("world_clock.max_count:%u", msg->max_count);
    APP_SYS_LOG_INFO("world_clock.city_name:%s", msg->city_name);
    APP_SYS_LOG_INFO("world_clock.zone_offset:%d", msg->zone_offset);
#endif
    return true;
}

/*@brief 打包传输设备信息
 */
void app_nanopb_xfer_notify_device_info(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_device_info_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_DevInfo *data = &message.payload.device_info;
    snprintf(data->model,   sizeof(data->model),   "%s", "NAL-WB00");
    snprintf(data->hw_ver,  sizeof(data->hw_ver),  "%s", "V1.0");
    snprintf(data->sw_ver,  sizeof(data->sw_ver),  "%s", "V1.0");
    snprintf(data->sn,      sizeof(data->sn),      "%s", "AT3380123");
    snprintf(data->bt_addr, sizeof(data->bt_addr), "%s", "11:22:33:44:55:66");
    snprintf(data->pid,     sizeof(data->pid),     "%s", "AT338");
    data->battery = 85;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收设备信息
 */
bool app_nanopb_xfer_respond_device_info(AppPB_MsgSet *message)
{
    AppPB_DevInfo *msg = &message->payload.device_info;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("device_info.model:%s",   msg->model);
    APP_SYS_LOG_INFO("device_info.hw_ver:%s",  msg->hw_ver);
    APP_SYS_LOG_INFO("device_info.sw_ver:%s",  msg->sw_ver);
    APP_SYS_LOG_INFO("device_info.sn:%s",      msg->sn);
    APP_SYS_LOG_INFO("device_info.bt_addr:%s", msg->bt_addr);
    APP_SYS_LOG_INFO("device_info.pid:%s",     msg->pid);
    APP_SYS_LOG_INFO("device_info.battery:%u", msg->battery);
#endif
    return true;
}

/*@brief 打包传输设备参数
 */
void app_nanopb_xfer_notify_device_param(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_device_param_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    message.payload.device_param.lang_id = 0;
    message.payload.device_param.zone    = 8 * 3600;
    message.payload.device_param.is_12h  = 0;
    message.payload.device_param.is_mi   = 0;
    message.payload.device_param.brt     = 50;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收设备参数
 */
bool app_nanopb_xfer_respond_device_param(AppPB_MsgSet *message)
{
    AppPB_DevParam *msg = &message->payload.device_param;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("device_param.lang_id:%u", msg->lang_id);
    APP_SYS_LOG_INFO("device_param.zone:%u",    msg->zone);
    APP_SYS_LOG_INFO("device_param.is_12h:%u",  msg->is_12h);
    APP_SYS_LOG_INFO("device_param.is_mi:%u",   msg->is_mi);
#endif
    return true;
}

/*@brief 打包传输电子保卡
 */
void app_nanopb_xfer_notify_elec_card(void)
{
    AppPB_MsgSet message = {0};
    message.which_payload = AppPB_MsgSet_elec_card_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    message.payload.elec_card.is_activate = true;
    message.payload.elec_card.is_reported = false;
#else
    /* 等待系统适配:填充真实数据 */
#endif
    app_nanopb_xfer_notify_lower(app_module_transfer_chan_low, &message);
}

/*@brief 传输接收电子保卡
 */
bool app_nanopb_xfer_respond_elec_card(AppPB_MsgSet *message)
{
    AppPB_ElecCard *msg = &message->payload.elec_card;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    APP_SYS_LOG_INFO("elec_card.is_activate:%u", msg->is_activate);
    APP_SYS_LOG_INFO("elec_card.is_reported:%u", msg->is_reported);
#endif
    return true;
}

#endif
