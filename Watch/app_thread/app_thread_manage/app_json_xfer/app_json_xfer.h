#ifndef APP_JSON_XFER_H
#define APP_JSON_XFER_H

/*@brief     协议适配层,推送协议数据
 *@param[in] json_object JSON对象
 *@retval    推送是否成功
 */
bool app_json_xfer_notify(void *json_object);

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_json_xfer_respond(uint8_t *json_stream);

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_json_xfer_throw(uint8_t *json_stream);

/*@brief     协议适配层,派发协议数据
 *@param[in] json_object JSON对象
 *@retval    派发是否成功
 */
bool app_json_xfer_dispatch(void *json_object);

#endif
