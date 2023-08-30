/*实现目标:
 *    一些和基础提醒运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static app_mutex_t app_module_remind_group_mutex = {0};
static app_module_remind_group_t app_module_remind_group[app_module_remind_group_number] = {0};

/*@brief     更新提醒组事件
 *@param[in] clock 时钟实例
 */
void app_module_remind_group_update(app_module_clock_t clock[1])
{
    app_thread_package_t package = {
        .thread  = app_thread_id_mix_custom,
        .module  = app_thread_mix_custom_remind_group,
        .event   = app_thread_mix_custom_remind_group_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 生成提醒组事件
 */
static void app_module_remind_group_throw_event(app_module_remind_package_t remind)
{
    app_module_remind_package_t *remind_new = app_mem_alloc(sizeof(app_module_remind_package_t));
    remind_new->remind_group = remind.remind_group;
    remind_new->remind_item  = remind.remind_item;
    remind_new->remind_type  = remind.remind_type;
    
    app_thread_package_t package = {
        .thread  = app_thread_id_mix_custom,
        .module  = app_thread_mix_custom_remind_group,
        .event   = app_thread_mix_custom_remind_group_package,
        .dynamic = true,
        .size    = sizeof(app_module_remind_package_t),
        .data    = remind_new,
    };
    app_thread_package_notify(&package);
}

/*@brief 更新提醒组
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_group_reflush(void)
{
    const uint32_t day_secs = 24 * 60 * 60;
    app_module_remind_item_t   *item   = NULL;
    app_module_remind_item_t   *list   = NULL;
    app_module_remind_group_t  *group  = NULL;
    app_module_remind_package_t remind = {0};
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 在组的访问更新流程内,完全禁止对组的操作 */
    app_mutex_process(&app_module_remind_group_mutex, app_mutex_take);
    /* 检查所有提醒组内所有提醒项 */
    for (uint32_t idx1 = 0; idx1 < app_module_remind_group_number; idx1++) {
        if ((group = &app_module_remind_group[idx1])->array == NULL)
            continue;
        /* 检查这个提醒组内所有提醒项 */
        for (uint32_t idx2 = 0; idx2 < group->number; idx2++) {
            if (!(item = &(list = group->array)[idx2])->valid)
                 continue;
            /* 提醒数据(可能使用) */
            remind.remind_group = idx1;
            remind.remind_item  = idx2;
            /* 初次命中 */
            if (clock.utc == item->clock.utc) {
                remind.remind_type = app_thread_mix_custom_remind_group_first;
                app_module_remind_group_throw_event(remind);
            }
            /* 初次偏移命中 */
            if (clock.utc == item->offset_utc) {
                remind.remind_type = app_thread_mix_custom_remind_group_first_offset;
                app_module_remind_group_throw_event(remind);
            }
            /* 时分秒命中,对天,星期,月份进行二次检查命中 */
            switch (item->type) {
            case app_module_remind_item_custom: {
                /* 初次筛选,时分秒命中 */
                uint32_t cc_sec_s = clock.utc % day_secs;
                uint32_t t1_sec_s = item->clock.utc  % day_secs;
                uint32_t t2_sec_s = item->offset_utc % day_secs;
                /* 未命中时跳过进一步检查 */
                if ((cc_sec_s <  t1_sec_s && cc_sec_s <  t2_sec_s) ||
                    (cc_sec_s != t1_sec_s && cc_sec_s != t2_sec_s))
                    break;
                /* 星期检查:今天和目标都是一个星期,且对应星期处于激活态 */
                if (item->clock.week == clock.week && (item->week & (1 < clock.week)) != 0) {
                    /* 星期命中 */
                    if (cc_sec_s == t1_sec_s) {
                        remind.remind_type = app_thread_mix_custom_remind_group_week;
                        app_module_remind_group_throw_event(remind);
                    }
                    /* 星期偏移命中 */
                    if (cc_sec_s == t2_sec_s) {
                        remind.remind_type = app_thread_mix_custom_remind_group_week_offset;
                        app_module_remind_group_throw_event(remind);
                    }
                }
                /* 月份检查:今天和目标都是同一日,且对应月份处于激活态 */
                if (item->clock.day == clock.day && (item->month & (1 < clock.month)) != 0) {
                    /* 星期命中 */
                    if (cc_sec_s == t1_sec_s) {
                        remind.remind_type = app_thread_mix_custom_remind_group_month;
                        app_module_remind_group_throw_event(remind);
                    }
                    /* 星期偏移命中 */
                    if (cc_sec_s == t2_sec_s) {
                        remind.remind_type = app_thread_mix_custom_remind_group_month_offset;
                        app_module_remind_group_throw_event(remind);
                    }
                }
                break;
            }
            case app_module_remind_item_repeat: {
                uint32_t cc_sec_s = clock.utc;
                uint32_t t1_sec_s = item->clock.utc;
                uint32_t t2_sec_s = item->offset_utc;
                /* 轮转检查:时间差求模为0 */
                uint32_t off_utc_1 = cc_sec_s > t1_sec_s ? cc_sec_s - t1_sec_s : t1_sec_s - cc_sec_s;
                uint32_t off_utc_2 = cc_sec_s > t2_sec_s ? cc_sec_s - t2_sec_s : t2_sec_s - cc_sec_s;
                if (off_utc_1 % item->repeat == 0) {
                    remind.remind_type = app_thread_mix_custom_remind_group_repeat;
                    app_module_remind_group_throw_event(remind);
                }
                if (off_utc_2 % item->repeat == 0) {
                    remind.remind_type = app_thread_mix_custom_remind_group_repeat_offset;
                    app_module_remind_group_throw_event(remind);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    app_mutex_process(&app_module_remind_group_mutex, app_mutex_give);
}

/*@brief     添加提醒组
 *@param[in] array  提醒数组
 *@param[in] number 提醒数量
 *@retval    提醒组id
 */
uint32_t app_module_remind_group_add(app_module_remind_item_t *array, uint32_t number)
{
    uint32_t remind_group_id = -1;
    app_mutex_process(&app_module_remind_group_mutex, app_mutex_take);
    for (uint32_t idx = 0; idx < app_module_remind_group_number; idx++)
        if (app_module_remind_group[idx].array == NULL) {
            app_module_remind_group[idx].array  = array;
            app_module_remind_group[idx].number = number;
            remind_group_id = idx;
            break;
        }
    app_mutex_process(&app_module_remind_group_mutex, app_mutex_give);
    return remind_group_id;
}

/*@brief     移除提醒组
 *@param[in] remind_group_id 提醒组id
 */
void app_module_remind_group_del(uint32_t remind_group_id)
{
    if (remind_group_id < app_module_remind_group_number) {
        app_mutex_process(&app_module_remind_group_mutex, app_mutex_take);
        app_module_remind_group[remind_group_id].array  = NULL;
        app_module_remind_group[remind_group_id].number = 0;
        app_mutex_process(&app_module_remind_group_mutex, app_mutex_give);
    }
}

/*@brief 初始化提醒组
 */
void app_module_remind_group_ready(void)
{
    app_mutex_process(&app_module_remind_group_mutex, app_mutex_static);
    for (uint32_t idx = 0; idx < app_module_remind_group_number; idx++) {
        app_module_remind_group[idx].array  = NULL;
        app_module_remind_group[idx].number = 0;
    }
}
