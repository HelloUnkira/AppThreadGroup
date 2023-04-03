/*
 *实现目的:
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

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_module_timer.h"
#include "app_module_clock.h"
#include "app_module_vibrate.h"

/*@brief 混合中断线程模组初始化
 */
void app_thread_mix_irq_ready(void)
{
    /* 模组初始化 */
    app_module_clock_ready();
}

/*@brief 混合中断线程服务例程
 */
void app_thread_mix_irq_routine(void)
{
    app_sem_t *sem = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync(app_thread_id_mix_irq, &sem);
    app_thread_get_pipe(app_thread_id_mix_irq, &pipe);
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 将其从上面的接口中推延到此处 */ {
    }
    /* 主流程 */
    while (true) {
        app_sem_take(sem);
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread mix irq recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package);
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_mix_irq_system: {
                if (package.event == app_thread_group_work)
                    app_sys_work_execute((void *)package.data);
                break;
            }
            case app_thread_mix_irq_timer: {
                if (package.event == app_thread_mix_irq_timer_reduce_update)
                    app_module_timer_reduce();
                break;
            }
            case app_thread_mix_irq_clock: {
                if (package.event == app_thread_mix_irq_clock_local_update)
                    app_module_clock_local_update();
                if (package.event == app_thread_mix_irq_clock_timestamp_update) {
                    uint64_t utc_new = package.data != NULL ? *(uint64_t *)package.data : 0;
                    app_module_clock_timestamp_update(utc_new);
                }
                break;
            }
            case app_thread_mix_irq_vibrate: {
                if (package.event == app_thread_mix_irq_vibrate_msec_update)
                    app_module_vibrate_msec_update();
                break;
            }
            default: {
                #if APP_SYS_LOG_THREAD_CHECK
                APP_SYS_LOG_ERROR("thread mix irq pipe recv a unknown package");
                APP_SYS_LOG_ERROR("package thread:%u", package.thread);
                APP_SYS_LOG_ERROR("package module:%u", package.module);
                APP_SYS_LOG_ERROR("package event:%u",  package.event);
                APP_SYS_LOG_ERROR("package data:%p",   package.data);
                APP_SYS_LOG_ERROR("package size:%u",   package.size);
                #endif
                break;
            }
            }
        }
    }
}
