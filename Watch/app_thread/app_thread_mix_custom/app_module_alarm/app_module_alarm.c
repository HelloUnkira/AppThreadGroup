/*实现目标:
 *    一些和闹钟运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_clock.h"
#include "app_module_alarm.h"

static app_module_alarm_group_t app_module_alarm_group[APP_MODULE_ALARM_GROUP_MAX] = {0};

/*@brief        更新闹钟
 *@param[in]    alarm 闹钟实例
 *@param[in]    clock 时钟实例
 */
void app_module_alarm_update(app_module_alarm_t *alarm, app_module_clock_t *clock)
{
    if (!alarm->valid)
        return;
    uint32_t event = ~0;
    switch (alarm->type) {
    case app_module_alarm_custom: {
        /* 初次更新策略,闹钟设置时 */
        if (clock->utc == alarm->clock_base.utc) {
            event = app_thread_mix_custom_alarm_expired;
            alarm->clock_month = alarm->clock_base;
            alarm->clock_week  = alarm->clock_base;
            app_module_clock_to_week(&alarm->clock_month);
            app_module_clock_to_week(&alarm->clock_week);
        }
        /* 再次更新时,以当前时间线计算下一个提醒时间点 */
        app_module_clock_t clock_month = alarm->clock_month;
        app_module_clock_t clock_week  = alarm->clock_week;
        /* 再次更新策略,月迭代更新 */
        while (alarm->field_month != 0) {
            for (uint8_t idx = 0; idx < 12; idx++) {
                 uint8_t off = 1 << ((idx + clock_month.month) % 12);
                if ((alarm->field_month & off) != 0)
                    break;
                if ((clock_month.month += 1) > 12) {
                     clock_month.month -= 12;
                     clock_month.year++;
                }
            }
                if ((clock_month.month += 1) > 12) {
                     clock_month.month -= 12;
                     clock_month.year++;
                }
            app_module_clock_to_utc(&clock_month);
            app_module_clock_to_week(&clock_month);
            /* 再次更新策略,月迭代更新 */
            if (clock->utc == clock_month.utc) {
                event = app_thread_mix_custom_alarm_month;
                alarm->clock_month = clock_month;
            }
            /* 这里需要额外考虑,如果设备长期未启用 */
            /* 第一次同步时间时 */
            if (clock->utc <= clock_month.utc)
                break;
            alarm->clock_month = clock_month;
        }
        /* 再次更新策略,周迭代更新 */
        while (alarm->field_week != 0) {
            for (uint8_t idx = 0; idx < 7; idx++) {
                 uint8_t off = 1 << ((idx + clock_week.week + 1) % 7);
                if ((alarm->field_week & off) != 0)
                    break;
                clock_week.utc += 24 * 60 * 60;
            }
                clock_week.utc += 24 * 60 * 60;
            app_module_clock_to_dtime(&clock_week);
            app_module_clock_to_week(&clock_week);
            /* 再次更新策略,周迭代更新 */
            if (clock->utc == clock_week.utc) {
                event = app_thread_mix_custom_alarm_week;
                alarm->clock_week = clock_week;
            }
            /* 这里需要额外考虑,如果设备长期未启用 */
            /* 第一次同步时间时 */
            if (clock->utc <= clock_week.utc)
                break;
            alarm->clock_week = clock_week;
        }
        /* 只要闹钟是合法的 */
        /* 即使它是关闭状态也需要继续更新 */
        if (!alarm->onoff || event == ~0)
            break;
        #if APP_MODULE_CHECK
        app_module_clock_t *clock_now = NULL;
        APP_SYS_LOG_INFO("event:%u\n", event);
        APP_SYS_LOG_INFO("clock:\n");
        clock_now = clock;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("clock_base:\n");
        clock_now = &alarm->clock_base;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("clock_month:\n");
        clock_now = &clock_month;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("clock_week:\n");
        clock_now = &clock_week;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("field_month:%x\n", alarm->field_month);
        APP_SYS_LOG_INFO("field_week:%x\n",  alarm->field_week);
        APP_SYS_LOG_INFO("valid:%u\n",       alarm->valid);
        APP_SYS_LOG_INFO("onoff:%u\n",       alarm->onoff);
        #endif
        break;
    }
    case app_module_alarm_repeat: {
        /* 初次更新策略,闹钟设置时 */
        if (clock->utc == alarm->clock_base.utc) {
            event = app_thread_mix_custom_alarm_expired;
            alarm->clock_repeat = alarm->clock_base;
            app_module_clock_to_week(&alarm->clock_repeat);
        }
        /* 初次更新时,初始化一次策略,将其同步到一个时间线 */
        app_module_clock_t clock_repeat = alarm->clock_repeat;
        /* 再次更新时,轮转迭代更新 */
        while (alarm->repeat) {
            clock_repeat.utc += alarm->repeat;
            app_module_clock_to_dtime(&clock_repeat);
            app_module_clock_to_week(&clock_repeat);
            /* 再次更新策略,轮转迭代更新 */
            if (clock->utc == clock_repeat.utc) {
                event = app_thread_mix_custom_alarm_repeat;
                alarm->clock_repeat = clock_repeat;
            }
            /* 这里需要额外考虑,如果设备长期未启用 */
            /* 第一次同步时间时 */
            if (clock->utc <= clock_repeat.utc)
                break;
            alarm->clock_repeat = clock_repeat;
        }
        /* 只要闹钟是合法的 */
        /* 即使它是关闭状态也需要继续更新 */
        if (!alarm->onoff || event == ~0)
            break;
        #if APP_MODULE_CHECK
        app_module_clock_t *clock_now = NULL;
        APP_SYS_LOG_INFO("event:%u\n", event);
        APP_SYS_LOG_INFO("clock:\n");
        clock_now = clock;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("clock_base:\n");
        clock_now = &alarm->clock_base;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("clock_repeat:\n");
        clock_now = &clock_repeat;
        APP_SYS_LOG_INFO("utc=%lu,%u, %u-%u-%u, %u:%u:%u\n",
                         clock_now->utc,clock_now->week,
                         clock_now->year,clock_now->month,clock_now->day,
                         clock_now->hour,clock_now->minute,clock_now->second);
        APP_SYS_LOG_INFO("repeat:%u\n",  alarm->repeat);
        APP_SYS_LOG_INFO("valid:%u\n",   alarm->valid);
        APP_SYS_LOG_INFO("onoff:%u\n",   alarm->onoff);
        #endif
        break;
    }
    default:
        break;
    }
    /* 发送闹钟事件 */
    if (event != ~0) {
        app_package_t package = {
            .send_tid = app_thread_id_mix_custom,
            .recv_tid = app_thread_id_mix_custom,
            .module   = app_thread_mix_custom_alarm,
            .event    = event,
            .dynamic  = false,
            .size     = sizeof(app_module_alarm_t),
            .data     = alarm,
        };
        app_thread_package_notify(&package);
    }
}

/*@brief        更新闹钟组
 *@param[in]    clock 时钟实例
 */
void app_module_alarm_group_update(app_module_clock_t clock[1])
{
    for (uint32_t idx = 0; idx < APP_MODULE_ALARM_GROUP_MAX; idx++) {
        app_mutex_take(&app_module_alarm_group[idx].mutex);
        if (app_module_alarm_group[idx].array != NULL)
            for (uint32_t idx1 = 0; idx1 < app_module_alarm_group[idx].number; idx1++)
                app_module_alarm_update(&app_module_alarm_group[idx].array[idx1], clock);
        app_mutex_give(&app_module_alarm_group[idx].mutex);
    }
}

/*@brief 初始化闹钟组
 */
void app_module_alarm_group_ready(void)
{
    for (uint32_t idx = 0; idx < APP_MODULE_ALARM_GROUP_MAX; idx++) {
        app_module_alarm_group[idx].array  = NULL;
        app_module_alarm_group[idx].number = 0;
        app_module_alarm_group[idx].used   = 0;
        app_mutex_process(&app_module_alarm_group[idx].mutex);
    }
}

/*@brief        注册闹钟组
 *@param[in]    array  闹钟数组
 *@param[in]    number 闹钟数量
 *@retval       闹钟组id(错误为 -1)
 */
uint32_t app_module_alarm_group_register(app_module_alarm_t *array, uint32_t number)
{
    uint32_t alarm_group_id = -1;
    for (uint32_t idx = 0; idx < APP_MODULE_ALARM_GROUP_MAX; idx++) {
        app_mutex_take(&app_module_alarm_group[idx].mutex);
        if (app_module_alarm_group[idx].array == NULL) {
            app_module_alarm_group[idx].array  = array;
            app_module_alarm_group[idx].number = number;
            alarm_group_id = idx;
        }
        app_mutex_give(&app_module_alarm_group[idx].mutex);
        if (alarm_group_id != -1)
            for (uint32_t idx1 = 0; idx1 < app_module_alarm_group[idx].number; idx1++)
                 app_module_alarm_group[idx].array[idx1].valid = 0;
        if (alarm_group_id != -1)
             break;
    }
    return alarm_group_id;
}

/*@brief        闹钟组添加闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm          闹钟实例
 *@retval       闹钟组id(错误为 -1)
 */
uint32_t app_module_alarm_add(uint32_t alarm_group_id, app_module_alarm_t *alarm)
{
    uint32_t alarm_id = -1;
    app_mutex_take(&app_module_alarm_group[alarm_group_id].mutex);
    for (uint32_t idx = 0; idx < app_module_alarm_group[alarm_group_id].number; idx++)
        if (!app_module_alarm_group[alarm_group_id].array[idx].valid) {
             app_module_alarm_group[alarm_group_id].array[idx] = *alarm;
             app_module_alarm_group[alarm_group_id].array[idx].valid = true;
             app_module_alarm_group[alarm_group_id].used++;
             alarm_id = idx;
             break;
        }
    app_mutex_give(&app_module_alarm_group[alarm_group_id].mutex);
    return alarm_id;
}

/*@brief        闹钟组启动闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm          闹钟实例
 */
void app_module_alarm_start(uint32_t alarm_group_id, uint32_t alarm_id)
{
    app_mutex_take(&app_module_alarm_group[alarm_group_id].mutex);
    app_module_alarm_group[alarm_group_id].array[alarm_id].onoff = true;
    app_mutex_give(&app_module_alarm_group[alarm_group_id].mutex);
}

/*@brief        闹钟组停止闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm          闹钟实例
 */
void app_module_alarm_stop(uint32_t alarm_group_id, uint32_t alarm_id)
{
    app_mutex_take(&app_module_alarm_group[alarm_group_id].mutex);
    app_module_alarm_group[alarm_group_id].array[alarm_id].onoff = false;
    app_mutex_give(&app_module_alarm_group[alarm_group_id].mutex);
}

/*@brief        闹钟组移除闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm_id       闹钟id
 */
void app_module_alarm_del(uint32_t alarm_group_id, uint32_t alarm_id)
{
    app_mutex_take(&app_module_alarm_group[alarm_group_id].mutex);
    app_module_alarm_group[alarm_group_id].array[alarm_id].valid = false;
    app_module_alarm_group[alarm_group_id].used--;
    app_mutex_give(&app_module_alarm_group[alarm_group_id].mutex);
}

/*@brief        设置闹钟组中的闹钟实例
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm_id       闹钟id
 *@param[out]   alarm          闹钟实例
 */
void app_module_alarm_set(uint32_t alarm_group_id, uint32_t alarm_id, app_module_alarm_t *alarm)
{
    app_mutex_take(&app_module_alarm_group[alarm_group_id].mutex);
    app_module_alarm_group[alarm_group_id].array[alarm_id] = *alarm;
    app_mutex_give(&app_module_alarm_group[alarm_group_id].mutex);
}

/*@brief        获得闹钟组中的闹钟实例
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm_id       闹钟id
 *@param[out]   alarm          闹钟实例
 */
void app_module_alarm_get(uint32_t alarm_group_id, uint32_t alarm_id, app_module_alarm_t *alarm)
{
    app_mutex_take(&app_module_alarm_group[alarm_group_id].mutex);
    *alarm = app_module_alarm_group[alarm_group_id].array[alarm_id];
    app_mutex_give(&app_module_alarm_group[alarm_group_id].mutex);
}
