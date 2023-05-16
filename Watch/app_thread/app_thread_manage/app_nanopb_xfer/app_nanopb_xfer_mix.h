#ifndef APP_NANOPB_XFER_MIX_H
#define APP_NANOPB_XFER_MIX_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/*@brief 打包传输系统时钟
 */
void app_nanopb_xfer_notify_system_clock(void);

/*@brief     传输接收系统时钟
 *@param[in] message nanopb对象
 *@retval    响应是否成功
 */
bool app_nanopb_xfer_respond_system_clock(AppPB_MsgSet *message);

#endif

#endif
