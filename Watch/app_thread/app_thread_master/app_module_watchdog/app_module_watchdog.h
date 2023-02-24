#ifndef APP_MODULE_WATCHDOG_H
#define APP_MODULE_WATCHDOG_H

/* 最长阻塞时间(秒) */
#define APP_MODULE_WATCHDOG_XS  5

/*@brief 初始化线程软件看门狗模组
 */
void app_module_watchdog_ready(void);

#endif
