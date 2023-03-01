#ifndef APP_MODULE_TIMER_H
#define APP_MODULE_TIMER_H

/*@brief 约减软件定时器
 *       内部使用: 被mix custom线程使用
 */
void app_module_timer_reduce(void);

/*@brief 软件定时器模组更新
 */
void app_module_timer_1ms_update(void);

/*@brief 软件定时器模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_timer_ready(void);

#endif
