#ifndef APP_MODULE_WATCHDOG_H
#define APP_MODULE_WATCHDOG_H

typedef struct {
    uint8_t count_fw;
    uint8_t count_sw_s[app_thread_id_s_e - app_thread_id_s_s - 1];
    uint8_t count_sw_d[app_thread_id_d_e - app_thread_id_d_s - 1];
} app_module_watchdog_t;

/*@brief 初始化软件看门狗模组
 */
void app_module_watchdog_ready(void);

#endif
