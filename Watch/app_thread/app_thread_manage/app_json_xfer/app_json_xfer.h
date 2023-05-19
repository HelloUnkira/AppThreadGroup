#ifndef APP_JSON_XFER_H
#define APP_JSON_XFER_H

#if APP_MODULE_PROTOCOL_USE_JSON

/*@brief     协议适配层,推送协议数据
 *@param[in] json_object JSON对象
 *@retval    推送是否成功
 */
bool app_json_xfer_notify(cJSON *json_object);

/*@brief     协议适配层,接收协议数据
 *@param[in] json_stream JSON数据流
 *@retval    解析是否成功
 */
bool app_json_xfer_respond(uint8_t *json_stream);

#endif

#endif
