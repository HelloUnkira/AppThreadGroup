/*实现目标:
 *    为每个线程追加一个软件看门狗定时器
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_module_system.h"
#include "app_module_watchdog.h"
#include "app_module_clock.h"

static app_mutex_t app_module_watchdog_mutex = {0};
static uint8_t app_module_watchdog_count[app_thread_id_num] = {0};

/*@brief     线程软件看门狗喂狗
 *@param[in] thread_id 线程编号
 */
void app_module_watchdog_feed(uint8_t thread_id)
{
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
            .send_tid = app_thread_id_unknown,
            .recv_tid = idx,
            .module   = 0,  /* 固定为第0号模组 */
            .event    = 0,  /* 固定为第0号事件 */
            .dynamic  = false,
            .size     = 0,
            .data     = NULL,
        };
        app_package_notify(&package);
        /* 如果超出最大时限,出错断言 */
        app_mutex_take(&app_module_watchdog_mutex);
        if (app_module_watchdog_count[idx]++ > APP_MODULE_WATCHDOG_XS) {
            APP_SYS_LOG_ERROR("app_module_watchdog catch thread %u block", idx);
            APP_MODULE_ASSERT(true == false);
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
