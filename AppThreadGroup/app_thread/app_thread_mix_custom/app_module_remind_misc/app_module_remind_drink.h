#ifndef APP_MODULE_REMIND_DRINK_H
#define APP_MODULE_REMIND_DRINK_H

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
} app_module_remind_drink_t;

/*@brief     喝水提醒设置
 *@param[in] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_set(app_module_remind_drink_t *remind_drink);

/*@brief      喝水提醒获取
 *@param[out] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_get(app_module_remind_drink_t *remind_drink);

/*@brief      喝水提醒默认设置
 *@param[out] remind_drink 喝水提醒参数
 */
void app_module_remind_drink_reset(app_module_remind_drink_t *remind_drink);

/*@brief 喝水提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_drink_xmin_update(void);

#endif
