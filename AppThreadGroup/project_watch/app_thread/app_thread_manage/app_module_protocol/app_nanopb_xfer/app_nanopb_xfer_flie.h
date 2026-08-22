#ifndef APP_NANOPB_XFER_FLIE_H
#define APP_NANOPB_XFER_FLIE_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输OTA升级
 */
void app_nanopb_xfer_notify_ota(void);

/*@brief 传输接收OTA升级
 */
bool app_nanopb_xfer_respond_ota(AppPB_MsgSet *message);

#endif

#endif
