/*实现目标:
 *    软硬件看门狗定时器模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static app_mutex_t app_module_watchdog_mutex = {0};
static app_module_watchdog_t app_module_watchdog = {0};

/*@brief 软件看门狗喂狗
 *@param thread_id 线程编号
 */
static void app_module_watchdog_feed_work(void *argument)
{
    uint8_t thread_id = (uint8_t)(uintptr_t)argument;
    
    APP_SYS_ASSERT((app_thread_id_s_s < thread_id && thread_id < app_thread_id_s_e) ||
                   (app_thread_id_d_s < thread_id && thread_id < app_thread_id_d_e));
    
    if (app_thread_id_s_s < thread_id && thread_id < app_thread_id_s_e) {
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_take);
        app_module_watchdog.count_sw_s[thread_id - app_thread_id_s_s - 1] = 0;
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_give);
    }
    if (app_thread_id_d_s < thread_id && thread_id < app_thread_id_d_e) {
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_take);
        app_module_watchdog.count_sw_d[thread_id - app_thread_id_d_s - 1] = 0;
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_give);
    }
}

/*@brief 软件看门狗控制更新
 *@param clock 时钟实例
 */
void app_module_watchdog_ctrl_check(app_module_clock_t clock[1])
{
    if (app_module_watchdog.count_fw++ % APP_MODULE_WATCHDOG_XS == 0)
        app_dev_watchdog_feed(&app_dev_watchdog);
    /* 批量向其他子线程递进软件看门狗的喂狗任务 */
    for (uint32_t idx = app_thread_id_s_s + 1; idx <= app_thread_id_s_e - 1; idx++) {
        /* 每隔1s发送一次软件看门狗喂狗事件 */
        app_thread_package_t package = {
            .thread  = idx,
            .module  = 0,                       /* 线程组系统模组 */
            .event   = app_thread_event_work,   /* 线程组工作事件 */
            .data    = app_module_work_make(1, app_module_watchdog_feed_work, (void *)(uintptr_t)idx),
        };
        app_thread_package_notify(&package);
        /* 如果超出最大时限,出错断言 */
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_take);
        if (app_module_watchdog.count_sw_s[idx - app_thread_id_s_s - 1]++ > APP_MODULE_WATCHDOG_XS) {
            APP_SYS_LOG_ERROR("catch thread %u block", idx);
            APP_SYS_ASSERT(true == false);
        }
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_give);
    }
    /* 批量向其他子线程递进软件看门狗的喂狗任务 */
    for (uint32_t idx = app_thread_id_d_s + 1; idx <= app_thread_id_d_e - 1; idx++) {
        if (!app_thread_id_is_valid(idx))
             continue;
        /* 每隔1s发送一次软件看门狗喂狗事件 */
        app_thread_package_t package = {
            .thread  = idx,
            .module  = 0,                       /* 线程组系统模组 */
            .event   = app_thread_event_work,   /* 线程组工作事件 */
            .data    = app_module_work_make(1, app_module_watchdog_feed_work, (void *)(uintptr_t)idx),
        };
        app_thread_package_notify(&package);
        /* 如果超出最大时限,出错断言 */
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_take);
        if (app_module_watchdog.count_sw_d[idx - app_thread_id_d_s - 1]++ > APP_MODULE_WATCHDOG_XS) {
            APP_SYS_LOG_ERROR("catch thread %u block", idx);
            APP_SYS_ASSERT(true == false);
        }
        app_mutex_process(&app_module_watchdog_mutex, app_mutex_give);
    }
}

/*@brief 初始化软件看门狗模组
 */
void app_module_watchdog_ready(void)
{
    app_mutex_process(&app_module_watchdog_mutex, app_mutex_static);
    app_dev_watchdog_ready(&app_dev_watchdog);
}
