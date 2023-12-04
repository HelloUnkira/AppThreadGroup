#ifndef APP_MODULE_REMIND_DRINK_H
#define APP_MODULE_REMIND_DRINK_H

/* 时间表检查函数:app_module_daily_time_remind_drink_t */
APP_MODULE_DAILY_TIME_T(app_module_daily_time_remind_drink_t, 2);

typedef struct {
    app_module_daily_time_remind_drink_t daily_time;
    uint64_t utc;       /* 上次提醒(秒) */
    uint32_t interval;  /* 提醒间隔(秒) */
}app_module_remind_drink_t;

/*@brief 喝水提醒设置
 *@param remind_drink 喝水提醒参数
 */
void app_module_remind_drink_set(app_module_remind_drink_t *remind_drink);

/*@brief 喝水提醒获取
 *@param remind_drink 喝水提醒参数
 */
void app_module_remind_drink_get(app_module_remind_drink_t *remind_drink);

/*@brief 喝水提醒默认设置
 *@param remind_drink 喝水提醒参数
 */
void app_module_remind_drink_reset(app_module_remind_drink_t *remind_drink);

/*@brief 喝水提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_drink_xmin_update(void);

#endif
