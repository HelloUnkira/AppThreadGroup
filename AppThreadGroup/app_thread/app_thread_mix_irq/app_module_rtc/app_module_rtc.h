#ifndef APP_MODULE_RTC_H
#define APP_MODULE_RTC_H

typedef struct {
    uint64_t utc;
} app_module_rtc_t;

/*@brief 设置RTC模组
 */
void app_module_rtc_set(app_module_rtc_t *rtc);

/*@brief 获取RTC模组
 */
void app_module_rtc_get(app_module_rtc_t *rtc);

/*@brief RTC模组1ms事件中断回调
 */
void app_module_rtc_1ms_cb(void);

/*@brief 初始化RTC模组
 */
void app_module_rtc_ready(void);

#endif
