#ifndef APP_NANOPB_XFER_FUNC_H
#define APP_NANOPB_XFER_FUNC_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输闹钟
 */
void app_nanopb_xfer_notify_alarm(void);

/*@brief 传输接收闹钟
 */
bool app_nanopb_xfer_respond_alarm(AppPB_MsgSet *message);

/*@brief 打包传输天气
 */
void app_nanopb_xfer_notify_weather(void);

/*@brief 传输接收天气
 */
bool app_nanopb_xfer_respond_weather(AppPB_MsgSet *message);

/*@brief 打包传输心率参数
 */
void app_nanopb_xfer_notify_heart_rate(void);

/*@brief 传输接收心率参数
 */
bool app_nanopb_xfer_respond_heart_rate(AppPB_MsgSet *message);

/*@brief 打包传输音乐
 */
void app_nanopb_xfer_notify_music(void);

/*@brief 传输接收音乐
 */
bool app_nanopb_xfer_respond_music(AppPB_MsgSet *message);

/*@brief 打包传输消息通知
 */
void app_nanopb_xfer_notify_msg_info(void);

/*@brief 传输接收消息通知
 */
bool app_nanopb_xfer_respond_msg_info(AppPB_MsgSet *message);

/*@brief 打包传输联系人
 */
void app_nanopb_xfer_notify_contact(void);

/*@brief 传输接收联系人
 */
bool app_nanopb_xfer_respond_contact(AppPB_MsgSet *message);

/*@brief 打包传输运动目标
 */
void app_nanopb_xfer_notify_sport_tgt(void);

/*@brief 传输接收运动目标
 */
bool app_nanopb_xfer_respond_sport_tgt(AppPB_MsgSet *message);

/*@brief 打包传输用户身体信息
 */
void app_nanopb_xfer_notify_user_phys(void);

/*@brief 传输接收用户身体信息
 */
bool app_nanopb_xfer_respond_user_phys(AppPB_MsgSet *message);

/*@brief 打包传输运动汇总
 */
void app_nanopb_xfer_notify_motion_sum(void);

/*@brief 传输接收运动汇总
 */
bool app_nanopb_xfer_respond_motion_sum(AppPB_MsgSet *message);

/*@brief 打包传输运动状态
 */
void app_nanopb_xfer_notify_sport_state(void);

/*@brief 传输接收运动状态
 */
bool app_nanopb_xfer_respond_sport_state(AppPB_MsgSet *message);

/*@brief 打包传输勿扰
 */
void app_nanopb_xfer_notify_not_disturb(void);

/*@brief 传输接收勿扰
 */
bool app_nanopb_xfer_respond_not_disturb(AppPB_MsgSet *message);

/*@brief 打包传输地理位置
 */
void app_nanopb_xfer_notify_position(void);

/*@brief 传输接收地理位置
 */
bool app_nanopb_xfer_respond_position(AppPB_MsgSet *message);

/*@brief 打包传输生理周期
 */
void app_nanopb_xfer_notify_fem_cycle(void);

/*@brief 传输接收生理周期
 */
bool app_nanopb_xfer_respond_fem_cycle(AppPB_MsgSet *message);

/*@brief 打包传输账户
 */
void app_nanopb_xfer_notify_account(void);

/*@brief 传输接收账户
 */
bool app_nanopb_xfer_respond_account(AppPB_MsgSet *message);

/*@brief 打包传输运动管理
 */
void app_nanopb_xfer_notify_sport_mng(void);

/*@brief 传输接收运动管理
 */
bool app_nanopb_xfer_respond_sport_mng(AppPB_MsgSet *message);

/*@brief 打包传输运动记录
 */
void app_nanopb_xfer_notify_sport_rcd(void);

/*@brief 传输接收运动记录
 */
bool app_nanopb_xfer_respond_sport_rcd(AppPB_MsgSet *message);

/*@brief 打包传输系统时钟
 */
void app_nanopb_xfer_notify_system_clock(void);

/*@brief 传输接收系统时钟
 */
bool app_nanopb_xfer_respond_system_clock(AppPB_MsgSet *message);

/*@brief 打包传输世界时钟
 */
void app_nanopb_xfer_notify_world_clock(void);

/*@brief 传输接收世界时钟
 */
bool app_nanopb_xfer_respond_world_clock(AppPB_MsgSet *message);

#endif

#endif
