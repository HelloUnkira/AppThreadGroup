#ifndef APP_MODULE_DRINK_REMIND_H
#define APP_MODULE_DRINK_REMIND_H

/* 定制化策略(泛用策略见时间表设计) */

typedef struct {
    uint8_t am_time_s[2];   /* 上午提醒起始[时,分] */
    uint8_t am_time_e[2];   /* 上午提醒结束[时,分] */
    uint8_t pm_time_s[2];   /* 下午提醒起始[时,分] */
    uint8_t pm_time_e[2];   /* 下午提醒结束[时,分] */
    uint8_t am_valid:1;     /* 启停状况 */
    uint8_t pm_valid:1;     /* 启停状况 */
    uint8_t interval:6;     /* 提醒间隔(分) */
    uint8_t week:7;         /* 周:日一二三四五六:[0, 6] */
} app_module_drink_remind_t;

/*@brief     喝水提醒设置启停
 *@param[in] am_onoff 上午启停开关
 *@param[in] pm_onoff 下午启停开关
 */
void app_module_drink_remind_set_onoff(bool am_onoff, bool pm_onoff);

/*@brief     喝水提醒设置周重复
 *@param[in] week 周位域
 */
void app_module_drink_remind_set_repeat(uint8_t week);

/*@brief     喝水提醒设置提醒间隔
 *@param[in] interval 提醒间隔(分)
 */
void app_module_drink_remind_set_interval(uint8_t interval);

/*@brief     喝水提醒设置上午开始时分
 *@param[in] hour   时(24时制)
 *@param[in] minute 分(24时制)
 */
void app_module_drink_remind_set_am_time_start(uint8_t hour, uint8_t minute);

/*@brief     喝水提醒设置上午结束时分
 *@param[in] hour   时(24时制)
 *@param[in] minute 分(24时制)
 */
void app_module_drink_remind_set_am_time_end(uint8_t hour, uint8_t minute);

/*@brief     喝水提醒设置下午开始时分
 *@param[in] hour   时(24时制)
 *@param[in] minute 分(24时制)
 */
void app_module_drink_remind_set_pm_time_start(uint8_t hour, uint8_t minute);

/*@brief     喝水提醒设置下午结束时分
 *@param[in] hour   时(24时制)
 *@param[in] minute 分(24时制)
 */
void app_module_drink_remind_set_pm_time_end(uint8_t hour, uint8_t minute);

/*@brief 喝水提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_drink_remind_xmin_update(void);

/*@brief 喝水提醒转储到外存
 */
void app_module_drink_remind_dump(void);

/*@brief 喝水提醒加载到内存
 */
void app_module_drink_remind_load(void);

/*@brief 喝水提醒模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_drink_remind_ready(void);

#endif
