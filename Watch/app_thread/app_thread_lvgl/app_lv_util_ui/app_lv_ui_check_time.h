#ifndef APP_LV_UI_CHECK_TIME_H
#define APP_LV_UI_CHECK_TIME_H

/* 一直界面常亮 */
#define APP_LV_UI_OVER_TIME_MAX  0xFF

/* 一直空闲等待 */
#define APP_LV_UI_IDLE_TIME_MAX  0xFF

/* 界面主界面空闲亮屏等待时间 */
#define APP_LV_UI_OVER_TIME      10

/* 界面返回主界面空闲等待时间 */
#define APP_LV_UI_IDLE_TIME      5

/*@brief 重置界面状态
 */
void app_lv_ui_check_time_reset(uint8_t over_time, uint8_t idle_time);

/*@brief lvgl 时间检查更新初始化
 */
void app_lv_ui_check_time_ready(void);

/*@brief 界面状态控制更新
 *       内部使用: 被lvgl线程使用
 */
void app_lv_ui_check_time_update(void);

/*@brief     界面状态检查功能启停
 *@param[in] status(true:启动界面状态检查,false:关闭界面状态检查)
 */
void app_lv_ui_check_time_exec(bool status);

#endif
