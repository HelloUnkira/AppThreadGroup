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

/*@brief 初始化RTC模组
 */
void app_module_rtc_ready(void);

/*@brief 运行RTC模组
 */
void app_module_rtc_execute(void);

#endif
