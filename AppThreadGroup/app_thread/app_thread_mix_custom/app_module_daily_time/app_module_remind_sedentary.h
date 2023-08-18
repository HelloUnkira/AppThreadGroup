#ifndef APP_MODULE_REMIND_SEDENTARY_H
#define APP_MODULE_REMIND_SEDENTARY_H

#include "app_module_daily_time.h"
/* 时间表检查函数:app_module_daily_time_remind_sedentary_t */
APP_MODULE_DAILY_TIME_T(app_module_daily_time_remind_sedentary_t, 2);

typedef struct {
    app_module_daily_time_remind_sedentary_t daily_time;
    uint64_t utc;       /* 上次提醒(秒) */
    uint32_t interval;  /* 提醒间隔(秒) */
} app_module_remind_sedentary_t;

/*@brief     走动提醒设置
 *@param[in] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_set(app_module_remind_sedentary_t *remind_sedentary);

/*@brief      走动提醒获取
 *@param[out] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_get(app_module_remind_sedentary_t *remind_sedentary);

/*@brief      走动提醒默认设置
 *@param[out] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_reset(app_module_remind_sedentary_t *remind_sedentary);

/*@brief 走到提醒时长刷新
 *       如果产生了活动或走动,应该刷新它
 */
void app_module_remind_sedentary_reflush(void);

/*@brief 走动提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_sedentary_xmin_update(void);

#endif
