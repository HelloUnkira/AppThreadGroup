/*实现目标:
 *    协议自检(ACK推进式回环发包,验证各协议消息收发通路)
 *    通用确认引擎驱动:发一条等一条ack,确认后推进下一条测试指令,替代定时器无脑发包;
 *                  超时未获ack由引擎重发,3次超时则当次notify自动结束
 *    file:内部多步调度(描述符/分包/结束),结束后经同一完成回调推进
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

/*@brief 待测试的协议消息类型序列 */
static const app_module_protocol_notify_type_t app_module_protocol_test_msg[] = {
    app_module_protocol_device_info,
    app_module_protocol_device_param,
    app_module_protocol_elec_card,
    app_module_protocol_system_clock,
    app_module_protocol_world_clock,
    app_module_protocol_alarm,
    app_module_protocol_weather,
    app_module_protocol_heart_rate,
    app_module_protocol_music,
    app_module_protocol_msg_info,
    app_module_protocol_contact,
    app_module_protocol_sport_tgt,
    app_module_protocol_user_phys,
    app_module_protocol_motion_sum,
    app_module_protocol_sport_state,
    app_module_protocol_not_disturb,
    app_module_protocol_position,
    app_module_protocol_fem_cycle,
    app_module_protocol_account,
    app_module_protocol_sport_mng,
    app_module_protocol_sport_rcd,
    app_module_protocol_ota,

    app_module_protocol_file,        /* 内部多步调度,完成后经完成回调推进 */
    app_module_protocol_trace_text,  /* 单发无确认,发出即完成 */
};

static uint32_t app_module_protocol_test_index = 0;
static bool     app_module_protocol_test_over  = false;

/*@brief 发送下一条测试指令(ACK推进式)
 *       常规协议与OTA:经确认引擎发送,等待对端ack后由完成回调推进;
 *       文件传输:内部多步调度,结束后由完成回调推进;
 *       trace_text:单发无确认,发出后立即推进
 */
static void app_module_protocol_test_next(void)
{
    if (app_module_protocol_test_over)
        return;
    /* 发送完所有消息则停止自检 */
    if (app_module_protocol_test_index >= app_sys_arr_len(app_module_protocol_test_msg)) {
        app_module_protocol_test_over = true;
        APP_SYS_LOG_WARN("protocol test over");
        return;
    }
    app_module_protocol_notify_type_t type = app_module_protocol_test_msg[app_module_protocol_test_index++];
    app_module_protocol_t protocol = {.notify.status = 0, .notify.type = type,};
    app_module_protocol_notify(&protocol);
    /* trace_text单发无确认,发出即完成,立即推进下一条 */
    if (type == app_module_protocol_trace_text)
        app_module_protocol_test_next();
}

#if APP_MODULE_PROTOCOL_USE_NANOPB
/*@brief 确认引擎完成回调
 *       上一条notify确认/超时结束后,推进到下一测试指令
 */
static void app_module_protocol_test_advance(uint16_t msg_tag, bool ok, uint32_t user)
{
    if (!ok)
        APP_SYS_LOG_ERROR("protocol test step fail tag:%u", msg_tag);
    app_module_protocol_test_next();
}
#endif

/*@brief 协议自检
 *       回环发包,验证各协议消息收发通路(打印)
 *       由manage线程ready后调用
 */
void app_module_protocol_test(void)
{
    app_module_protocol_test_index = 0;
    app_module_protocol_test_over  = false;
#if APP_MODULE_PROTOCOL_USE_NANOPB
    /* 注册确认引擎完成回调:ACK到达后推进下一条测试指令 */
    app_nanopb_xfer_ctrl_set_done(app_module_protocol_test_advance, 0);
    app_module_protocol_test_next();
#endif
}

/* ===== 自检固态假数据:统一填充AppPB_MsgSet(APP_MODULE_PROTOCOL_TEST_ENABLE分流) ===== */

/*@brief 文件传输兜底文件名(TEST用)
 *@retval 文件名字符串
 */
const char *app_module_protocol_test_file_name(void)
{
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    return "loopback.log";
#else
    return "app.log";
#endif
}

/*@brief 文件内容兜底样本(TEST用)
 *@param image      文件内容缓冲
 *@param image_size 缓冲大小
 *@retval 生成的样本大小
 */
uint32_t app_module_protocol_test_fill_file_sample(uint8_t *image, uint32_t image_size)
{
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    for (uint32_t idx = 0; idx < image_size; idx++)
        image[idx] = (uint8_t)(idx + (idx / 7));
    return image_size;
#else
    memset(image, 0, image_size);
    return 0;
#endif
}

/*@brief 填充设备信息(AppPB_MsgSet设备信息)
 */
void app_module_protocol_test_fill_device_info(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_device_info_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_DevInfo *data = &message->payload.device_info;
    snprintf(data->model,   sizeof(data->model),   "%s", "NAL-WB00");
    snprintf(data->hw_ver,  sizeof(data->hw_ver),  "%s", "V1.0");
    snprintf(data->sw_ver,  sizeof(data->sw_ver),  "%s", "V1.0");
    snprintf(data->sn,      sizeof(data->sn),      "%s", "AT3380123");
    snprintf(data->bt_addr, sizeof(data->bt_addr), "%s", "11:22:33:44:55:66");
    snprintf(data->pid,     sizeof(data->pid),     "%s", "AT338");
    data->battery = 85;
#else
    memset(&message->payload.device_info, 0, sizeof(message->payload.device_info));
#endif
}

/*@brief 填充设备参数(AppPB_MsgSet设备参数)
 */
void app_module_protocol_test_fill_device_param(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_device_param_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    message->payload.device_param.lang_id = 0;
    message->payload.device_param.zone    = 8 * 3600;
    message->payload.device_param.is_12h  = 0;
    message->payload.device_param.is_mi   = 0;
    message->payload.device_param.brt     = 50;
#else
    memset(&message->payload.device_param, 0, sizeof(message->payload.device_param));
#endif
}

/*@brief 填充电子保卡(AppPB_MsgSet电子保卡)
 */
void app_module_protocol_test_fill_elec_card(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_elec_card_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    message->payload.elec_card.is_activate = true;
    message->payload.elec_card.is_reported = false;
#else
    memset(&message->payload.elec_card, 0, sizeof(message->payload.elec_card));
#endif
}

/*@brief 填充闹钟(AppPB_MsgSet闹钟)
 */
void app_module_protocol_test_fill_alarm(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_alarm_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Alarm *data = &message->payload.alarm;
    data->index  = 0;
    data->repeat = 0x3E;
    data->on     = 1;
    data->hour   = 7;
    data->min    = 30;
    snprintf(data->name, sizeof(data->name), "%s", "wakeup");
#else
    memset(&message->payload.alarm, 0, sizeof(message->payload.alarm));
#endif
}

/*@brief 填充天气(AppPB_MsgSet天气)
 */
void app_module_protocol_test_fill_weather(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_weather_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Weather *data = &message->payload.weather;
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
    memset(&message->payload.weather, 0, sizeof(message->payload.weather));
#endif
}

/*@brief 填充心率参数(AppPB_MsgSet心率参数)
 */
void app_module_protocol_test_fill_heart_rate(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_heart_rate_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_HeartRate *data = &message->payload.heart_rate;
    data->is_auto     = 1;
    data->lwarn_on    = 1;
    data->hwarn_on    = 1;
    data->lwarn       = 50;
    data->hwarn       = 120;
    data->test_min    = 30;
    data->static_type = 0;
#else
    memset(&message->payload.heart_rate, 0, sizeof(message->payload.heart_rate));
#endif
}

/*@brief 填充音乐(AppPB_MsgSet音乐)
 */
void app_module_protocol_test_fill_music(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_music_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Music *data = &message->payload.music;
    snprintf(data->singer,    sizeof(data->singer),    "%s", "Jay");
    snprintf(data->song_name, sizeof(data->song_name), "%s", "Cloud");
    data->play_st = 1;
    data->max_vol = 10;
    data->cur_vol = 6;
    data->app_st  = 1;
#else
    memset(&message->payload.music, 0, sizeof(message->payload.music));
#endif
}

/*@brief 填充消息通知(AppPB_MsgSet消息通知)
 */
void app_module_protocol_test_fill_msg_info(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_msg_info_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_MsgInfo *data = &message->payload.msg_info;
    snprintf(data->app_name, sizeof(data->app_name), "%s", "wechat");
    snprintf(data->contact,  sizeof(data->contact),  "%s", "tom");
    snprintf(data->content,  sizeof(data->content),  "%s", "hello");
    snprintf(data->phone,    sizeof(data->phone),    "%s", "13800138000");
    data->msg_id   = 1;
    data->msg_type = 1;
    data->vibrate  = 1;
#else
    memset(&message->payload.msg_info, 0, sizeof(message->payload.msg_info));
#endif
}

/*@brief 填充联系人(AppPB_MsgSet联系人)
 */
void app_module_protocol_test_fill_contact(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_contact_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Contact *data = &message->payload.contact;
    snprintf(data->name,  sizeof(data->name),  "%s", "tom");
    snprintf(data->phone, sizeof(data->phone), "%s", "13800138000");
    data->name_len  = 3;
    data->phone_len = 11;
    data->state     = 1;
#else
    memset(&message->payload.contact, 0, sizeof(message->payload.contact));
#endif
}

/*@brief 填充运动目标(AppPB_MsgSet运动目标)
 */
void app_module_protocol_test_fill_sport_tgt(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_sport_tgt_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportTgt *data = &message->payload.sport_tgt;
    data->motion_t  = 1;
    data->goal_type = 1;
    data->step      = 10000;
    data->kcal      = 500;
    data->meter     = 8000;
    data->duration  = 7200;
#else
    memset(&message->payload.sport_tgt, 0, sizeof(message->payload.sport_tgt));
#endif
}

/*@brief 填充用户身体信息(AppPB_MsgSet用户身体信息)
 */
void app_module_protocol_test_fill_user_phys(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_user_phys_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_UserPhys *data = &message->payload.user_phys;
    data->age       = 30;
    data->birthday  = 0;
    data->gender    = 1;
    data->height    = 175;
    data->weight    = 65;
    data->vo2max    = 45;
    data->run_step  = 80;
    data->walk_step = 70;
#else
    memset(&message->payload.user_phys, 0, sizeof(message->payload.user_phys));
#endif
}

/*@brief 填充运动汇总(AppPB_MsgSet运动汇总)
 */
void app_module_protocol_test_fill_motion_sum(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_motion_sum_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_MotionSum *data = &message->payload.motion_sum;
    data->hr_value   = 75;
    data->hr_ts      = 0;
    data->kcal       = 300;
    data->distance   = 5000;
    data->elevation  = 100;
    data->sleep_time = 480;
    data->step       = 8000;
#else
    memset(&message->payload.motion_sum, 0, sizeof(message->payload.motion_sum));
#endif
}

/*@brief 填充运动状态(AppPB_MsgSet运动状态)
 */
void app_module_protocol_test_fill_sport_state(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_sport_state_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportState *data = &message->payload.sport_state;
    data->monitor_st   = 0;
    data->operator_t   = 0;
    data->sport_type   = 1;
    data->start_time   = 0;
    data->workout_type = 1;
    data->op_time      = 0;
#else
    memset(&message->payload.sport_state, 0, sizeof(message->payload.sport_state));
#endif
}

/*@brief 填充勿扰(AppPB_MsgSet勿扰)
 */
void app_module_protocol_test_fill_not_disturb(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_not_disturb_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_NotDisturb *data = &message->payload.not_disturb;
    data->on     = 1;
    data->shour  = 22;
    data->smin   = 0;
    data->ehour  = 7;
    data->emin   = 0;
    data->repeat = 0x7F;
#else
    memset(&message->payload.not_disturb, 0, sizeof(message->payload.not_disturb));
#endif
}

/*@brief 填充地理位置(AppPB_MsgSet地理位置)
 */
void app_module_protocol_test_fill_position(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_position_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Position *data = &message->payload.position;
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
    memset(&message->payload.position, 0, sizeof(message->payload.position));
#endif
}

/*@brief 填充生理周期(AppPB_MsgSet生理周期)
 */
void app_module_protocol_test_fill_fem_cycle(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_fem_cycle_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_FemCycle *data = &message->payload.fem_cycle;
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
    memset(&message->payload.fem_cycle, 0, sizeof(message->payload.fem_cycle));
#endif
}

/*@brief 填充账户(AppPB_MsgSet账户)
 */
void app_module_protocol_test_fill_account(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_account_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_Account *data = &message->payload.account;
    snprintf(data->account, sizeof(data->account), "%s", "user01");
    data->acc_len    = 6;
    data->pair_state = 1;
    data->app_role   = 1;
#else
    memset(&message->payload.account, 0, sizeof(message->payload.account));
#endif
}

/*@brief 填充运动管理(AppPB_MsgSet运动管理)
 */
void app_module_protocol_test_fill_sport_mng(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_sport_mng_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportMng *data = &message->payload.sport_mng;
    data->max_add_num      = 10;
    data->min_add_num      = 1;
    data->sport_type_count = 3;
    data->sport_type[0]    = 1;
    data->sport_type[1]    = 2;
    data->sport_type[2]    = 3;
#else
    memset(&message->payload.sport_mng, 0, sizeof(message->payload.sport_mng));
#endif
}

/*@brief 填充运动记录(AppPB_MsgSet运动记录)
 */
void app_module_protocol_test_fill_sport_rcd(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_sport_rcd_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_SportRcd *data = &message->payload.sport_rcd;
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
    memset(&message->payload.sport_rcd, 0, sizeof(message->payload.sport_rcd));
#endif
}

/*@brief 填充世界时钟(AppPB_MsgSet世界时钟)
 */
void app_module_protocol_test_fill_world_clock(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_world_clock_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    AppPB_WorldClock *data = &message->payload.world_clock;
    data->now_index   = 0;
    data->max_count   = 1;
    snprintf(data->city_name, sizeof(data->city_name), "%s", "Beijing");
    data->zone_offset = 8 * 3600;
    data->city_id     = 0;
#else
    memset(&message->payload.world_clock, 0, sizeof(message->payload.world_clock));
#endif
}

/*@brief 填充OTA升级(AppPB_MsgSet OTA升级)
 */
void app_module_protocol_test_fill_ota(AppPB_MsgSet *message)
{
    message->which_payload = AppPB_MsgSet_ota_tag;
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    data->cmd        = 1;
    data->state      = 0;
    data->ready_cond = 0;
#else
    memset(&message->payload.ota, 0, sizeof(message->payload.ota));
#endif
}
