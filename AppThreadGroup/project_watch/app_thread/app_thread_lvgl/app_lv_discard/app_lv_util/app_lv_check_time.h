#ifndef APP_LV_CHECK_TIME_H
#define APP_LV_CHECK_TIME_H

/* 一直界面常亮 */
#define APP_LV_CHECK_TIME_OVER_MAX  0xFF

/* 一直空闲等待 */
#define APP_LV_CHECK_TIME_IDLE_MAX  0xFF

/* 界面主界面空闲亮屏等待时间 */
#define APP_LV_CHECK_TIME_OVER      10

/* 界面返回主界面空闲等待时间 */
#define APP_LV_CHECK_TIME_IDLE      5

typedef struct {
    bool    dlps;    /* 需要进入dlps */
    bool    back;    /* 需要回退界面 */
    uint8_t over_time;
    uint8_t idle_time;
    uint8_t over_time_bak;
    uint8_t idle_time_bak;
} app_lv_check_time_t;

/*@brief 重置界面状态
 */
void app_lv_check_time_reset(uint8_t over_time, uint8_t idle_time);

/*@brief lvgl 时间检查更新初始化
 */
void app_lv_check_time_ready(void);

/*@brief 界面状态控制更新
 *       内部使用: 被lvgl线程使用
 */
void app_lv_check_time_update(void);

/*@brief 界面状态检查功能启停
 *@param status(true:启动界面状态检查,false:关闭界面状态检查)
 */
void app_lv_check_time_exec(bool status);

#endif
