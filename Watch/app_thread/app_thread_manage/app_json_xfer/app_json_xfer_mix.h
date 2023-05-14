#ifndef APP_JSON_XFER_MIX_H
#define APP_JSON_XFER_MIX_H

/*@brief 打包传输系统时钟
 */
void app_json_xfer_notify_system_clock(void);

/*@brief     传输接收系统时钟
 *@param[in] json_object JSON对象
 *@retval    响应是否成功
 */
bool app_json_xfer_respond_system_clock(void *json_object);

#endif
