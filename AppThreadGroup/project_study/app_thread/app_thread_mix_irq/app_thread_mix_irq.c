/*实现目标:
 *    有一些事件在中断环境下产生
 *前提条件:
 *    在中断环境下产生的事件
 *    中断服务例程一般不能完整的处理该事件所有的任务
 *    从实际角度出发大家会选择"立即触发"的软件定时器
 *    这其实相当于使用OS提供的软件定时器线程
 *    提供了一个非中断的环境下处理该事件
 *    如果大量使用这种方式
 *    势必造成软件定时器的响应延迟
 *    所以混合中断线程现在代替了这种策略
 *    之所以单独起线程,则是希望通过设置较高优先级
 *    用以区分中断环境下与常规环境产生的事件紧急度
 *    向它投递一个中断事件包,触发相应的驱动或业务流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 子线程服务例程就绪部
 */
static void app_thread_mix_irq_routine_ready_cb(void)
{
    /* 相关模组初始化 */
    app_module_clock_ready();
    app_module_rtc_ready();
    app_module_watchdog_ready();
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_mix_irq_routine_package_cb(app_thread_package_t *package, uint32_t *discard_count, bool *record)
{
    switch (package->module) {
    case app_thread_mix_irq_timer: {
        if (package->event == app_thread_mix_irq_timer_reduce_update)
            app_module_timer_reduce();
       *record = false;
        return true;
    }
    case app_thread_mix_irq_clock: {
        if (package->event == app_thread_mix_irq_clock_local_update)
            app_module_clock_local_update();
        if (package->event == app_thread_mix_irq_clock_timestamp_update)
            app_module_clock_timestamp_update(package->byte_fixed);
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief 混合中断线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_mix_irq_routine)
{
    app_thread_slave_process(app_thread_id_mix_irq,
                             app_thread_mix_irq_routine_ready_cb,
                             app_thread_mix_irq_routine_package_cb);
}
