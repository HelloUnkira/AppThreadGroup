#ifndef APP_MODULE_WATCHDOG_H
#define APP_MODULE_WATCHDOG_H

/* 最长阻塞时间(秒) */
#define APP_MODULE_WATCHDOG_XS  5

typedef struct {
    uint8_t count_fw;
    uint8_t count_sw[app_thread_id_number];
} app_module_watchdog_t;

/*@brief 初始化软件看门狗模组
 */
void app_module_watchdog_ready(void);

#endif
