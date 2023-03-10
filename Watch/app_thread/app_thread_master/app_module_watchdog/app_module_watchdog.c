/*实现目标:
 *    为每个线程追加一个软件看门狗定时器
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_module_watchdog.h"
#include "app_module_clock.h"

static app_mutex_t app_module_watchdog_mutex = {0};
static uint8_t app_module_watchdog_count[app_thread_id_num] = {0};

/*@brief     线程软件看门狗喂狗
 *@param[in] thread_id 线程编号
 */
static void app_module_watchdog_feed_work(void *argument)
{
    uint8_t thread_id = (uint8_t)(uintptr_t)argument;
    if (thread_id < app_thread_id_num) {
        app_mutex_take(&app_module_watchdog_mutex);
        app_module_watchdog_count[thread_id] = 0;
        app_mutex_give(&app_module_watchdog_mutex);
    }
}

/*@brief     线程软件看门狗控制更新
 *@param[in] clock 时钟实例
 */
void app_module_watchdog_ctrl_check(app_module_clock_t clock[1])
{
    for (uint8_t idx = 0; idx < app_thread_id_num; idx++) {
        /* 每隔1s发送一次软件看门狗喂狗事件 */
        app_package_t package = {
            .thread  = idx,
            .module  = app_thread_group_work,  /* 线程组工作模组 */
            .event   = 0,
            .dynamic = true,
            .size    = sizeof(app_sys_work_t),
            .data    = app_sys_work_make(app_module_watchdog_feed_work, (void *)(uintptr_t)idx),
        };
        app_package_notify(&package);
        /* 如果超出最大时限,出错断言 */
        app_mutex_take(&app_module_watchdog_mutex);
        if (app_module_watchdog_count[idx]++ > APP_MODULE_WATCHDOG_XS) {
            APP_SYS_LOG_ERROR("catch thread %u block", idx);
            APP_SYS_ASSERT(true == false);
        }
        app_mutex_give(&app_module_watchdog_mutex);
    }
}

/*@brief 初始化线程软件看门狗模组
 */
void app_module_watchdog_ready(void)
{
    app_mutex_process(&app_module_watchdog_mutex);
}
