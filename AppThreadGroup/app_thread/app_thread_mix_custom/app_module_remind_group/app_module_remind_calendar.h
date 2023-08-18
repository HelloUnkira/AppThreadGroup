#ifndef APP_MODULE_REMIND_CALENDAR_H
#define APP_MODULE_REMIND_CALENDAR_H

#define APP_MODULE_REMIND_CALENDAR_MAX              (10)
#define APP_MODULE_REMIND_CALENDAR_TITLE_MAX        (64)
#define APP_MODULE_REMIND_CALENDAR_LOCATE_MAX       (64)
#define APP_MODULE_REMIND_CALENDAR_DESCRIPT_MAX     (128)

typedef struct {
    uint8_t  title[APP_MODULE_REMIND_CALENDAR_TITLE_MAX];
    uint8_t  locate[APP_MODULE_REMIND_CALENDAR_LOCATE_MAX];
    uint8_t  descript[APP_MODULE_REMIND_CALENDAR_DESCRIPT_MAX];
} app_module_remind_calendar_info_t;

/*@brief      提醒组是否为提醒日历组
 *@param[out] remind_group 提醒组
 *@retval     是否为日历组
 */
bool app_module_remind_calendar_group_check(uint32_t remind_group);

/*@brief 锁定提醒日历列表
 *       此时提醒日历列表不再迭代,也不生成事件
 *       可以直接对提醒日历列表本地修改
 */
void app_module_remind_calendar_array_lock(void);

/*@brief 解锁提醒日历列表
 *       此时提醒日历列表继续迭代,继续生成事件
 *       不可以直接对提醒日历列表本地修改
 */
void app_module_remind_calendar_array_unlock(void);

/*@brief      提醒日历列表
 *@param[out] calendar_item 提醒日历列表
 *@param[out] calendar_info 提醒日历信息列表
 */
void app_module_remind_calendar_array(app_module_remind_item_t **calendar_item, app_module_remind_calendar_info_t **calendar_info);

/*@brief 提醒日历转储到外存
 */
void app_module_remind_calendar_dump(void);

/*@brief 提醒日历加载到内存
 */
void app_module_remind_calendar_load(void);

/*@brief 初始化提醒日历组
 */
void app_module_remind_calendar_ready(void);

#endif
