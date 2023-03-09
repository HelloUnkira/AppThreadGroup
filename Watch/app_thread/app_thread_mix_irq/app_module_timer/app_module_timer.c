/*实现目标:
 *    软件定时器线程适配层
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_timer.h"
#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_module_timer.h"


/*@brief 约减软件定时器
 *       内部使用: 被mix irq线程使用
 */
void app_module_timer_reduce(void)
{
    app_sys_timer_reduce();
}

/*@brief 定时器模组更新
 */
void app_module_timer_1ms_update(void)
{
    app_package_t package = {
        .thread = app_thread_id_mix_irq,
        .module = app_thread_mix_irq_timer,
        .event  = app_thread_mix_irq_timer_reduce_update,
    };
    app_package_notify(&package);
}

/*@brief 软件定时器模组初始化
 *       内部使用: 被mix irq线程使用
 */
void app_module_timer_ready(void)
{
    app_sys_timer_ready();
}
