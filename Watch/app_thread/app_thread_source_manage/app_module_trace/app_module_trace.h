#ifndef APP_MODULE_TRACE_H
#define APP_MODULE_TRACE_H

/*当前提供模式:
 *    1.函数追踪队列(搭配lst或map进一步确认调用链)
 *    2.日志追踪队列
 */

/*@brief 函数追踪队列
 */
void app_module_trace_func(void *func);

/* 单次存储日志文本最大长度(包括字符串结尾'\0') */
#define APP_MODULE_TRACE_LOG_MAX    7/*1*/
 
/*@brief 日志追踪队列初始化
 */
void app_module_trace_text_ready(void);

/*@brief 日志追踪队列重置
 */
void app_module_trace_text_reset(void);

/*@brief      日志追踪队列尾加入一段日志
 *@param[in]  text 日志文本
 */
void app_module_trace_text_dump(char text[APP_MODULE_TRACE_LOG_MAX]);

/*@brief      日志追踪队列头取出一段日志
 *@param[out] text 日志文本
 */
bool app_module_trace_text_load(char text[APP_MODULE_TRACE_LOG_MAX]);

#endif
