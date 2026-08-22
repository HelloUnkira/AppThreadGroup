#ifndef APP_MODULE_PROTOCOL_TEST_H
#define APP_MODULE_PROTOCOL_TEST_H

/*@brief 协议自检
 *       开机回环发包,验证各协议消息收发通路(打印)
 *       由manage线程ready后调用
 */
void app_module_protocol_test(void);

#endif
