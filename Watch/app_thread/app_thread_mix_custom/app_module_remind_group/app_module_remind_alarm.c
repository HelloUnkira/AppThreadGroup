/*实现目标:
 *    一些和提醒闹钟运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_ext_src.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_clock.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"

static uint32_t                       app_module_remind_alarm_group = -1;
static app_mutex_t                    app_module_remind_alarm_mutex = {0};
static app_module_remind_item_t       app_module_remind_alarm_item[APP_MODULE_REMIND_ALARM_MAX] = {0};
static app_module_remind_alarm_info_t app_module_remind_alarm_info[APP_MODULE_REMIND_ALARM_MAX] = {0};

/*@brief      提醒组是否为提醒闹钟组
 *@param[out] remind_group 提醒组
 *@retval     是否为闹钟组
 */
bool app_module_remind_alarm_group_check(uint32_t remind_group)
{
    return remind_group == app_module_remind_alarm_group;
}

/*@brief 锁定提醒闹钟列表
 *       此时提醒闹钟列表不再迭代,也不生成事件
 *       可以直接对提醒闹钟列表本地修改
 */
void app_module_remind_alarm_array_lock(void)
{
    /* 移除出提醒组,打断其事件更新 */
    app_mutex_take(&app_module_remind_alarm_mutex);
    app_module_remind_group_del(app_module_remind_alarm_group);
    app_module_remind_alarm_group = -1;
    app_mutex_give(&app_module_remind_alarm_mutex);
}

/*@brief 解锁提醒闹钟列表
 *       此时提醒闹钟列表继续迭代,继续生成事件
 *       不可以直接对提醒闹钟列表本地修改
 */
void app_module_remind_alarm_array_unlock(void)
{
    /* 加入进提醒组,恢复其事件更新 */
    app_mutex_take(&app_module_remind_alarm_mutex);
    app_module_remind_alarm_group = app_module_remind_group_add(app_module_remind_alarm_item, APP_MODULE_REMIND_ALARM_MAX);
    app_mutex_give(&app_module_remind_alarm_mutex);
}

/*@brief      提醒闹钟列表
 *@param[out] remind_item 提醒闹钟列表
 *@param[out] alarm_info  提醒闹钟信息列表
 */
void app_module_remind_alarm_array(app_module_remind_item_t **remind_item, app_module_remind_alarm_info_t **alarm_info)
{
    *remind_item = app_module_remind_alarm_item;
    *alarm_info  = app_module_remind_alarm_info;
}

/*@brief 提醒闹钟复位清除
 */
void app_module_remind_alarm_clean(void)
{
    app_module_remind_alarm_array_lock();
    memset(app_module_remind_alarm_item, 0, sizeof(app_module_remind_alarm_item));
    memset(app_module_remind_alarm_info, 0, sizeof(app_module_remind_alarm_info));
    app_module_remind_alarm_array_unlock();
}

/*@brief 提醒闹钟转储到外存
 */
void app_module_remind_alarm_dump(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_remind_item_t       remind_item[APP_MODULE_REMIND_ALARM_MAX];
            app_module_remind_alarm_info_t alarm_info[APP_MODULE_REMIND_ALARM_MAX];
            uint32_t crc32;
        };
    } remind_alarm_data;
    
    app_module_remind_alarm_array_lock();
    memcpy(remind_alarm_data.remind_item, app_module_remind_alarm_item, sizeof(app_module_remind_alarm_item));
    memcpy(remind_alarm_data.alarm_info,  app_module_remind_alarm_info, sizeof(app_module_remind_alarm_info));
    app_module_remind_alarm_array_unlock();
    
    uint32_t offset_data_size = (uintptr_t)&remind_alarm_data.crc32 - (uintptr_t)remind_alarm_data.buffer;
    remind_alarm_data.crc32 = app_sys_crc32(remind_alarm_data.buffer, offset_data_size);
    app_sys_ext_src_write("mix_chunk_small", "remind alarm", remind_alarm_data.buffer, sizeof(remind_alarm_data));
}

/*@brief 提醒闹钟加载到内存
 */
void app_module_remind_alarm_load(void)
{
    union {
        uint8_t buffer[0];
        struct {
            app_module_remind_item_t       remind_item[APP_MODULE_REMIND_ALARM_MAX];
            app_module_remind_alarm_info_t alarm_info[APP_MODULE_REMIND_ALARM_MAX];
            uint32_t crc32;
        };
    } remind_alarm_data;
    
    uint32_t offset_data_size = (uintptr_t)&remind_alarm_data.crc32 - (uintptr_t)remind_alarm_data.buffer;
    app_sys_ext_src_read("mix_chunk_small", "remind alarm", remind_alarm_data.buffer, sizeof(remind_alarm_data));
    uint32_t crc32 = app_sys_crc32(remind_alarm_data.buffer, offset_data_size);
    
    if (crc32 == remind_alarm_data.crc32) {
        app_module_remind_alarm_array_lock();
        memcpy(app_module_remind_alarm_item, remind_alarm_data.remind_item, sizeof(app_module_remind_alarm_item));
        memcpy(app_module_remind_alarm_info, remind_alarm_data.alarm_info,  sizeof(app_module_remind_alarm_info));
        app_module_remind_alarm_array_unlock();
    }
    if (crc32 != remind_alarm_data.crc32) {
        app_module_remind_alarm_clean();
        APP_SYS_LOG_WARN("load remind alarm fail");
    }
}

/*@brief 初始化提醒闹钟组
 */
void app_module_remind_alarm_ready(void)
{
    app_mutex_process(&app_module_remind_alarm_mutex);
}
