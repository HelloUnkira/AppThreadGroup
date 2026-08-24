#ifndef APP_JSON_XFER_PACK_H
#define APP_JSON_XFER_PACK_H

#if APP_MODULE_PROTOCOL_USE_JSON

#include "cJSON.h"
#include "app_nanopb_set.pb.h"

/*@brief 打包转换 ACK 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_ack(const AppPB_ACK *msg);

/*@brief 解析转换 JSON 对象为 ACK 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_ack(cJSON *obj, AppPB_ACK *msg);

/*@brief 打包转换 Account 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_account(const AppPB_Account *msg);

/*@brief 解析转换 JSON 对象为 Account 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_account(cJSON *obj, AppPB_Account *msg);

/*@brief 打包转换 Alarm 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_alarm(const AppPB_Alarm *msg);

/*@brief 解析转换 JSON 对象为 Alarm 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_alarm(cJSON *obj, AppPB_Alarm *msg);

/*@brief 打包转换 Contact 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_contact(const AppPB_Contact *msg);

/*@brief 解析转换 JSON 对象为 Contact 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_contact(cJSON *obj, AppPB_Contact *msg);

/*@brief 打包转换 DevInfo 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_dev_info(const AppPB_DevInfo *msg);

/*@brief 解析转换 JSON 对象为 DevInfo 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_dev_info(cJSON *obj, AppPB_DevInfo *msg);

/*@brief 打包转换 DevParam 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_dev_param(const AppPB_DevParam *msg);

/*@brief 解析转换 JSON 对象为 DevParam 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_dev_param(cJSON *obj, AppPB_DevParam *msg);

/*@brief 打包转换 ElecCard 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_elec_card(const AppPB_ElecCard *msg);

/*@brief 解析转换 JSON 对象为 ElecCard 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_elec_card(cJSON *obj, AppPB_ElecCard *msg);

/*@brief 打包转换 FemCycle 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_fem_cycle(const AppPB_FemCycle *msg);

/*@brief 解析转换 JSON 对象为 FemCycle 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_fem_cycle(cJSON *obj, AppPB_FemCycle *msg);

/*@brief 打包转换 FileDes 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_file_des(const AppPB_FileDes *msg);

/*@brief 解析转换 JSON 对象为 FileDes 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_file_des(cJSON *obj, AppPB_FileDes *msg);

/*@brief 打包转换 FileEnd 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_file_end(const AppPB_FileEnd *msg);

/*@brief 解析转换 JSON 对象为 FileEnd 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_file_end(cJSON *obj, AppPB_FileEnd *msg);

/*@brief 打包转换 HeartRate 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_heart_rate(const AppPB_HeartRate *msg);

/*@brief 解析转换 JSON 对象为 HeartRate 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_heart_rate(cJSON *obj, AppPB_HeartRate *msg);

/*@brief 打包转换 MotionSum 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_motion_sum(const AppPB_MotionSum *msg);

/*@brief 解析转换 JSON 对象为 MotionSum 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_motion_sum(cJSON *obj, AppPB_MotionSum *msg);

/*@brief 打包转换 MsgInfo 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_msg_info(const AppPB_MsgInfo *msg);

/*@brief 解析转换 JSON 对象为 MsgInfo 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_msg_info(cJSON *obj, AppPB_MsgInfo *msg);

/*@brief 打包转换 Music 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_music(const AppPB_Music *msg);

/*@brief 解析转换 JSON 对象为 Music 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_music(cJSON *obj, AppPB_Music *msg);

/*@brief 打包转换 NotDisturb 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_not_disturb(const AppPB_NotDisturb *msg);

/*@brief 解析转换 JSON 对象为 NotDisturb 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_not_disturb(cJSON *obj, AppPB_NotDisturb *msg);

/*@brief 打包转换 Ota 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_ota(const AppPB_Ota *msg);

/*@brief 解析转换 JSON 对象为 Ota 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_ota(cJSON *obj, AppPB_Ota *msg);

/*@brief 打包转换 Position 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_position(const AppPB_Position *msg);

/*@brief 解析转换 JSON 对象为 Position 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_position(cJSON *obj, AppPB_Position *msg);

/*@brief 打包转换 SportMng 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_mng(const AppPB_SportMng *msg);

/*@brief 解析转换 JSON 对象为 SportMng 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_mng(cJSON *obj, AppPB_SportMng *msg);

/*@brief 打包转换 SportRcd 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_rcd(const AppPB_SportRcd *msg);

/*@brief 解析转换 JSON 对象为 SportRcd 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_rcd(cJSON *obj, AppPB_SportRcd *msg);

/*@brief 打包转换 SportState 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_state(const AppPB_SportState *msg);

/*@brief 解析转换 JSON 对象为 SportState 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_state(cJSON *obj, AppPB_SportState *msg);

/*@brief 打包转换 SportTgt 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_tgt(const AppPB_SportTgt *msg);

/*@brief 解析转换 JSON 对象为 SportTgt 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_tgt(cJSON *obj, AppPB_SportTgt *msg);

/*@brief 打包转换 SysClock 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sys_clock(const AppPB_SysClock *msg);

/*@brief 解析转换 JSON 对象为 SysClock 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sys_clock(cJSON *obj, AppPB_SysClock *msg);

/*@brief 打包转换 TraceTxt 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_trace_txt(const AppPB_TraceTxt *msg);

/*@brief 解析转换 JSON 对象为 TraceTxt 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_trace_txt(cJSON *obj, AppPB_TraceTxt *msg);

/*@brief 打包转换 UserPhys 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_user_phys(const AppPB_UserPhys *msg);

/*@brief 解析转换 JSON 对象为 UserPhys 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_user_phys(cJSON *obj, AppPB_UserPhys *msg);

/*@brief 打包转换 Weather 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_weather(const AppPB_Weather *msg);

/*@brief 解析转换 JSON 对象为 Weather 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_weather(cJSON *obj, AppPB_Weather *msg);

/*@brief 打包转换 WeatherDay 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_weather_day(const AppPB_WeatherDay *msg);

/*@brief 解析转换 JSON 对象为 WeatherDay 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_weather_day(cJSON *obj, AppPB_WeatherDay *msg);

/*@brief 打包转换 WorldClock 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_world_clock(const AppPB_WorldClock *msg);

/*@brief 解析转换 JSON 对象为 WorldClock 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_world_clock(cJSON *obj, AppPB_WorldClock *msg);

#endif

#endif
