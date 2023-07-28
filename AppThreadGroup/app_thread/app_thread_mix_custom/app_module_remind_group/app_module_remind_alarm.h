#ifndef APP_MODULE_REMIND_ALARM_H
#define APP_MODULE_REMIND_ALARM_H

#define APP_MODULE_REMIND_ALARM_MAX         10
#define APP_MODULE_REMIND_ALARM_NAME_MAX    20

typedef struct {
    uint8_t snooze_count;   /* 延迟响应次数 */
    uint8_t duration;       /* 事件响应周期 */
    uint8_t name[APP_MODULE_REMIND_ALARM_NAME_MAX];
} app_module_remind_alarm_info_t;

/*@brief      提醒组是否为提醒闹钟组
 *@param[out] remind_group 提醒组
 *@retval     是否为闹钟组
 */
bool app_module_remind_alarm_group_check(uint32_t remind_group);

/*@brief 锁定提醒闹钟列表
 *       此时提醒闹钟列表不再迭代,也不生成事件
 *       可以直接对提醒闹钟列表本地修改
 */
void app_module_remind_alarm_array_lock(void);

/*@brief 解锁提醒闹钟列表
 *       此时提醒闹钟列表继续迭代,继续生成事件
 *       不可以直接对提醒闹钟列表本地修改
 */
void app_module_remind_alarm_array_unlock(void);

/*@brief      提醒闹钟列表
 *@param[out] remind_item 提醒闹钟列表
 *@param[out] alarm_info  提醒闹钟信息列表
 */
void app_module_remind_alarm_array(app_module_remind_item_t **remind_item, app_module_remind_alarm_info_t **alarm_info);

/*@brief 提醒闹钟复位清除
 */
void app_module_remind_alarm_clean(void);

/*@brief 提醒闹钟转储到外存
 */
void app_module_remind_alarm_dump(void);

/*@brief 提醒闹钟加载到内存
 */
void app_module_remind_alarm_load(void);

/*@brief 初始化提醒闹钟组
 */
void app_module_remind_alarm_ready(void);

/*@brief 提醒闹钟模组测试
 */
void app_module_remind_alarm_test(void);

#endif
