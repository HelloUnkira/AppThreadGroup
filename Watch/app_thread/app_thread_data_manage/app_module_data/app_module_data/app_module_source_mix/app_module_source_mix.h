#ifndef APP_MODULE_SOURCE_MIX_H
#define APP_MODULE_SOURCE_MIX_H

/*@brief 系统时钟转储到外存
 */
void app_module_clock_dump(void);

/*@brief 系统时钟加载到内存
 */
void app_module_clock_load(void);

/*@brief 秒表转储到外存
 */
void app_module_stopwatch_dump(void);

/*@brief 秒表加载到内存
 */
void app_module_stopwatch_load(void);

/*@brief 倒计时转储到外存
 */
void app_module_countdown_dump(void);

/*@brief 倒计时加载到内存
 */
void app_module_countdown_load(void);

#endif
