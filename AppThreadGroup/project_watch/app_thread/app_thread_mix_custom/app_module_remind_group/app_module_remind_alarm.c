/*实现目标:
 *    一些和提醒闹钟运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static uint32_t                       app_module_remind_alarm_group = -1;
static app_mutex_t                    app_module_remind_alarm_mutex = {0};
static app_module_remind_item_t       app_module_remind_alarm_item[APP_MODULE_REMIND_ALARM_MAX] = {0};
static app_module_remind_alarm_info_t app_module_remind_alarm_info[APP_MODULE_REMIND_ALARM_MAX] = {0};

/*@brief 提醒组是否为提醒闹钟组
 *@param remind_group 提醒组
 *@retval 是否为闹钟组
 */
bool app_module_remind_alarm_group_check(uint32_t remind_group)
{
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_take);
    bool retval = remind_group == app_module_remind_alarm_group;
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_give);
    return retval;
}

/*@brief 锁定提醒闹钟列表
 *       此时提醒闹钟列表不再迭代,也不生成事件
 *       可以直接对提醒闹钟列表本地修改
 */
void app_module_remind_alarm_array_lock(void)
{
    /* 移除出提醒组,打断其事件更新 */
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_take);
    app_module_remind_group_del(app_module_remind_alarm_group);
    app_module_remind_alarm_group = -1;
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_give);
}

/*@brief 解锁提醒闹钟列表
 *       此时提醒闹钟列表继续迭代,继续生成事件
 *       不可以直接对提醒闹钟列表本地修改
 */
void app_module_remind_alarm_array_unlock(void)
{
    /* 加入进提醒组,恢复其事件更新 */
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_take);
    app_module_remind_alarm_group = app_module_remind_group_add(app_module_remind_alarm_item, APP_MODULE_REMIND_ALARM_MAX);
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_give);
}

/*@brief 提醒闹钟列表
 *@param alarm_item 提醒闹钟列表
 *@param alarm_info 提醒闹钟信息列表
 */
void app_module_remind_alarm_array(app_module_remind_item_t **alarm_item, app_module_remind_alarm_info_t **alarm_info)
{
    *alarm_item = app_module_remind_alarm_item;
    *alarm_info = app_module_remind_alarm_info;
}

/*@brief 提醒闹钟转储到外存
 */
void app_module_remind_alarm_dump(void)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    app_module_remind_alarm_array_lock();
    memcpy(&data_center_src->module_remind_alarm.alarm_item, app_module_remind_alarm_item, sizeof(app_module_remind_alarm_item));
    memcpy(&data_center_src->module_remind_alarm.alarm_info, app_module_remind_alarm_info, sizeof(app_module_remind_alarm_info));
    app_module_remind_alarm_array_unlock();
    app_module_data_center_give();
}

/*@brief 提醒闹钟加载到内存
 */
void app_module_remind_alarm_load(void)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    app_module_remind_alarm_array_lock();
    memcpy(app_module_remind_alarm_item, &data_center_src->module_remind_alarm.alarm_item, sizeof(app_module_remind_alarm_item));
    memcpy(app_module_remind_alarm_info, &data_center_src->module_remind_alarm.alarm_info, sizeof(app_module_remind_alarm_info));
    app_module_remind_alarm_array_unlock();
    app_module_data_center_give();
}

/*@brief 初始化提醒闹钟组
 */
void app_module_remind_alarm_ready(void)
{
    app_mutex_process(&app_module_remind_alarm_mutex, app_mutex_static);
}

/*@brief 提醒闹钟模组测试
 */
void app_module_remind_alarm_test(void)
{
    app_module_remind_item_t       *alarm_item = NULL;
    app_module_remind_alarm_info_t *alarm_info = NULL;
    
    app_module_remind_alarm_array_lock();
    app_module_remind_alarm_array(&alarm_item, &alarm_info);
    /* 提醒闹钟0(常规模式): */
    const char *alarm_name_0 = "Alarm 0";
    alarm_info[0].snooze_count = 0;
    alarm_info[0].duration = 300;
    memcpy(alarm_info[0].name, alarm_name_0, sizeof(alarm_name_0));
    alarm_item[0].valid = true;
    alarm_item[0].onoff = true;
    alarm_item[0].type  = app_module_remind_item_custom;
    alarm_item[0].month = 0b00000001000;
    alarm_item[0].week  = 0b0000100;
    alarm_item[0].clock.year   = 2023;
    alarm_item[0].clock.month  = 1;
    alarm_item[0].clock.day    = 1;
    alarm_item[0].clock.hour   = 0;
    alarm_item[0].clock.minute = 0;
    alarm_item[0].clock.second = 2;
    app_sys_clock_to_utc(&alarm_item[0].clock);
    app_sys_clock_to_week(&alarm_item[0].clock);
    alarm_item[0].offset_utc = alarm_item[0].clock.utc /* +-xxx sec */;
    /* 提醒闹钟1(滚动模式) */
    const char *alarm_name_1 = "Alarm 1";
    alarm_info[1].snooze_count = 0;
    alarm_info[1].duration = 300;
    memcpy(alarm_info[1].name, alarm_name_1, sizeof(alarm_name_1));
    alarm_item[1].valid  = true;
    alarm_item[1].onoff  = true;
    alarm_item[1].type   = app_module_remind_item_repeat;
    alarm_item[1].repeat = 3;
    alarm_item[1].clock.year   = 2023;
    alarm_item[1].clock.month  = 1;
    alarm_item[1].clock.day    = 1;
    alarm_item[1].clock.hour   = 1;
    alarm_item[1].clock.minute = 1;
    alarm_item[1].clock.second = 2;
    app_sys_clock_to_utc(&alarm_item[1].clock);
    app_sys_clock_to_week(&alarm_item[1].clock);
    alarm_item[1].offset_utc = alarm_item[1].clock.utc /* +-xxx sec */;
    /* 继续添加 */
    app_module_remind_alarm_array_unlock();
}
