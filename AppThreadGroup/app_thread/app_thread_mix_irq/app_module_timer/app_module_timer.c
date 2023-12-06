/*实现目标:
 *    软件定时器线程适配层
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 软件定时器事件包吸收
 */
static void app_module_timer_package_absorb(app_thread_package_t *package_old, app_thread_package_t *package_new)
{
    package_old->byte_fixed += package_new->byte_fixed;
}

/*@brief 约减软件定时器
 *       内部使用: 被mix irq线程使用
 *@param ms 约减毫秒数
 */
void app_module_timer_reduce(uint64_t ms)
{
    /* 接口一次性只能约减1ms,这里需要做循环约减 */
    for (uint64_t cnt = 0; cnt < ms; cnt++)
        app_sys_timer_reduce();
}

/*@brief 定时器模组更新
 */
void app_module_timer_1ms_update(void)
{
    app_thread_package_t package = {
        .thread     = app_thread_id_mix_irq,
        .module     = app_thread_mix_irq_timer,
        .event      = app_thread_mix_irq_timer_reduce_update,
        .priority   = app_thread_package_priority_real_time,
        .byte_fixed = 1,
        .absorb     = app_module_timer_package_absorb,
    };
    app_thread_package_notify(&package);
}
