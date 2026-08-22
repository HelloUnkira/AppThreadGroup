#ifndef APP_NANOPB_XFER_SYS_H
#define APP_NANOPB_XFER_SYS_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输设备信息
 */
void app_nanopb_xfer_notify_device_info(void);

/*@brief 传输接收设备信息
 */
bool app_nanopb_xfer_respond_device_info(AppPB_MsgSet *message);

/*@brief 打包传输设备参数
 */
void app_nanopb_xfer_notify_device_param(void);

/*@brief 传输接收设备参数
 */
bool app_nanopb_xfer_respond_device_param(AppPB_MsgSet *message);

/*@brief 打包传输电子保卡
 */
void app_nanopb_xfer_notify_elec_card(void);

/*@brief 传输接收电子保卡
 */
bool app_nanopb_xfer_respond_elec_card(AppPB_MsgSet *message);

#endif

#endif
