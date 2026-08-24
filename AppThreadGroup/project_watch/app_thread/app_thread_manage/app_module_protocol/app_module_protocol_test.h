#ifndef APP_MODULE_PROTOCOL_TEST_H
#define APP_MODULE_PROTOCOL_TEST_H

#include "app_ext_lib.h"
#include "app_nanopb_set.pb.h"

/*@brief 协议自检
 *       开机回环发包,验证各协议消息收发通路(打印)
 *       由manage线程ready后调用
 */
void app_module_protocol_test(void);

/*@brief 文件传输兜底文件名(TEST用)
 *       TEST: 回环样本名;否则: 生产占位名
 *@retval 文件名字符串
 */
const char *app_module_protocol_test_file_name(void);

/*@brief 文件内容兜底样本(TEST用)
 *       无日志可上传时,生成确定性字节样本保证传输链路可测
 *@param image      文件内容缓冲
 *@param image_size 缓冲大小
 *@retval 生成的样本大小
 */
uint32_t app_module_protocol_test_fill_file_sample(uint8_t *image, uint32_t image_size);

/*@brief 填充设备信息(AppPB_MsgSet设备信息)
 */
void app_module_protocol_test_fill_device_info(AppPB_MsgSet *message);

/*@brief 填充设备参数(AppPB_MsgSet设备参数)
 */
void app_module_protocol_test_fill_device_param(AppPB_MsgSet *message);

/*@brief 填充电子保卡(AppPB_MsgSet电子保卡)
 */
void app_module_protocol_test_fill_elec_card(AppPB_MsgSet *message);

/*@brief 填充闹钟(AppPB_MsgSet闹钟)
 */
void app_module_protocol_test_fill_alarm(AppPB_MsgSet *message);

/*@brief 填充天气(AppPB_MsgSet天气)
 */
void app_module_protocol_test_fill_weather(AppPB_MsgSet *message);

/*@brief 填充心率参数(AppPB_MsgSet心率参数)
 */
void app_module_protocol_test_fill_heart_rate(AppPB_MsgSet *message);

/*@brief 填充音乐(AppPB_MsgSet音乐)
 */
void app_module_protocol_test_fill_music(AppPB_MsgSet *message);

/*@brief 填充消息通知(AppPB_MsgSet消息通知)
 */
void app_module_protocol_test_fill_msg_info(AppPB_MsgSet *message);

/*@brief 填充联系人(AppPB_MsgSet联系人)
 */
void app_module_protocol_test_fill_contact(AppPB_MsgSet *message);

/*@brief 填充运动目标(AppPB_MsgSet运动目标)
 */
void app_module_protocol_test_fill_sport_tgt(AppPB_MsgSet *message);

/*@brief 填充用户身体信息(AppPB_MsgSet用户身体信息)
 */
void app_module_protocol_test_fill_user_phys(AppPB_MsgSet *message);

/*@brief 填充运动汇总(AppPB_MsgSet运动汇总)
 */
void app_module_protocol_test_fill_motion_sum(AppPB_MsgSet *message);

/*@brief 填充运动状态(AppPB_MsgSet运动状态)
 */
void app_module_protocol_test_fill_sport_state(AppPB_MsgSet *message);

/*@brief 填充勿扰(AppPB_MsgSet勿扰)
 */
void app_module_protocol_test_fill_not_disturb(AppPB_MsgSet *message);

/*@brief 填充地理位置(AppPB_MsgSet地理位置)
 */
void app_module_protocol_test_fill_position(AppPB_MsgSet *message);

/*@brief 填充生理周期(AppPB_MsgSet生理周期)
 */
void app_module_protocol_test_fill_fem_cycle(AppPB_MsgSet *message);

/*@brief 填充账户(AppPB_MsgSet账户)
 */
void app_module_protocol_test_fill_account(AppPB_MsgSet *message);

/*@brief 填充运动管理(AppPB_MsgSet运动管理)
 */
void app_module_protocol_test_fill_sport_mng(AppPB_MsgSet *message);

/*@brief 填充运动记录(AppPB_MsgSet运动记录)
 */
void app_module_protocol_test_fill_sport_rcd(AppPB_MsgSet *message);

/*@brief 填充世界时钟(AppPB_MsgSet世界时钟)
 */
void app_module_protocol_test_fill_world_clock(AppPB_MsgSet *message);

/*@brief 填充OTA升级(AppPB_MsgSet OTA升级)
 */
void app_module_protocol_test_fill_ota(AppPB_MsgSet *message);

#endif